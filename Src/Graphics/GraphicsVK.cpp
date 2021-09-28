#include "GraphicsVK.h"

#include <PGE/Exception/Exception.h>

#include "Shader/ShaderVK.h"
#include "Mesh/MeshVK.h"
#include "Texture/TextureVK.h"

using namespace PGE;

GraphicsVK::GraphicsVK(const String& name, int w, int h, WindowMode wm, int x, int y)
    : GraphicsSpecialized("Vulkan", name, w, h, wm, x, y, (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI)), resourceManager(*this) {
    // Layers.
    std::vector<const char*> layers;
#ifdef DEBUG
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    instance = resourceManager.addNewResource<VKInstance>(sdlWindow, name, layers);

    surface = resourceManager.addNewResource<VKSurface>(instance, sdlWindow);

    // The device extensions we need.
    std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    // Selecting a physical device and setting up queues.
    // Currently we just pick the one that supports what we need and has the most VRAM.
    bool foundCompatibleDevice = false;
    vk::DeviceSize selectedPdSize = 0;
    std::vector<vk::SurfaceFormatKHR> selectedPdFormats;
    std::vector<vk::PresentModeKHR> selectedPdPresentModes;
    for (vk::PhysicalDevice pd : instance->enumeratePhysicalDevices()) {
        // Doesn't support sampler anisotropy, ignore it.
        if (!pd.getFeatures().samplerAnisotropy) {
            continue;
        }

        // The physical device does not support any formats or present modes on this surface, ignore it.
        std::vector<vk::SurfaceFormatKHR> pdFormats;
        std::vector<vk::PresentModeKHR> pdPresentModes;
        if ((pdFormats = pd.getSurfaceFormatsKHR(surface)).empty() || (pdPresentModes = pd.getSurfacePresentModesKHR(surface)).empty()) {
            continue;
        }

        // Device must support our needed extensions.
        std::unordered_set<String::Key> extensionCheckSet;
        for (const char* de : deviceExtensions) {
            extensionCheckSet.insert(String(de));
        }
        for (vk::ExtensionProperties ep : pd.enumerateDeviceExtensionProperties()) {
            extensionCheckSet.erase(String((const char*)ep.extensionName));
            if (extensionCheckSet.empty()) {
                break;
            }
        }
        // Physical device does not support all needed extensions.
        if (!extensionCheckSet.empty()) { continue; }

        // Checking and setting the queue indices.
        uint32_t queueIndex = 0;
        bool foundGraphics = false;
        bool foundPresent = false;
        bool foundTransfer = false;
        for (const vk::QueueFamilyProperties& qfp : pd.getQueueFamilyProperties()) {
            if (!foundGraphics && qfp.queueFlags & vk::QueueFlagBits::eGraphics) {
                graphicsQueueIndex = queueIndex;
                foundGraphics = true;
            }
            if (!foundPresent && pd.getSurfaceSupportKHR(queueIndex, surface)) {
                presentQueueIndex = queueIndex;
                foundPresent = true;
            }
            if (!foundTransfer && qfp.queueFlags & vk::QueueFlagBits::eTransfer && !(qfp.queueFlags & vk::QueueFlagBits::eGraphics)) {
                transferQueueIndex = queueIndex;
                foundTransfer = true;
            }
            if (foundGraphics && foundPresent) {
                break;
            }
            queueIndex++;
        }
        // Something we need is not supported on this GPU, ignore it.
        if (!foundGraphics || !foundPresent) { continue; }

        // We didn't find a seperate transfer queue, so use the graphics one.
        if (!foundTransfer) {
            transferQueueIndex = graphicsQueueIndex;
        }

        // Calculate complete VRAM size.
        vk::DeviceSize pdSize = 0;
        vk::PhysicalDeviceMemoryProperties pdmp = pd.getMemoryProperties();
        for (int i = 0; i < (int)pdmp.memoryHeapCount; i++) {
            pdSize += pdmp.memoryHeaps.at(i).size;
        }

        if (pdSize > selectedPdSize) {
            foundCompatibleDevice = true;
            physicalDevice = pd;
            selectedPdFormats = pdFormats;
            selectedPdPresentModes = pdPresentModes;
            selectedPdSize = pdSize;
        }
    }
    PGE_ASSERT(foundCompatibleDevice, "No Vulkan compatible GPU found");

    device = resourceManager.addNewResource<VKDevice>(physicalDevice, std::unordered_set { graphicsQueueIndex, presentQueueIndex, transferQueueIndex }, layers, deviceExtensions);

    graphicsQueue = device->getQueue(graphicsQueueIndex, 0);
    presentQueue = device->getQueue(presentQueueIndex, 0);
    transferQueue = device->getQueue(transferQueueIndex, 0);

    // Selecting the right surface format.
    // If we don't find the one we want we'll settle for the first available one.
    swapchainFormat = selectedPdFormats[0];
    for (const auto& f : selectedPdFormats) {
        if (f.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear && f.format == vk::Format::eB8G8R8A8Srgb) {
            swapchainFormat = f;
            break;
        }
    }

    createSwapchain(true);

    imageAvailableSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.reserve(MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        imageAvailableSemaphores.emplace_back(resourceManager.addNewResource<VKSemaphore>(device));
        renderFinishedSemaphores.emplace_back(resourceManager.addNewResource<VKSemaphore>(device));
        inFlightFences.emplace_back(resourceManager.addNewResource<VKFence>(device, i != 0));
    }
    imagesInFlight.resize(MAX_FRAMES_IN_FLIGHT);
    acquireNextImage();

    sampler = resourceManager.addNewResource<VKSampler>(device, false);
    samplerRT = resourceManager.addNewResource<VKSampler>(device, true);

    open = true;
    focused = true;
}

void GraphicsVK::swap() {
    endRender();
    
    if (!trashBin.empty()) {
        device->waitIdle();
        // Clear trashbin.
        for (ResourceBase* b : trashBin) {
            delete b;
        }
        trashBin.clear();
    }

    acquireNextImage();
}

void GraphicsVK::endRender() {
    comBuffers[backBufferIndex].endRenderPass();
    comBuffers[backBufferIndex].end();

    vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    device->resetFences(inFlightFences[currentFrame].get());
    vk::SubmitInfo submitInfo = vk::SubmitInfo(1, &imageAvailableSemaphores[currentFrame], &waitStages, 1, &comBuffers[backBufferIndex], 1, &renderFinishedSemaphores[currentFrame]);
    vk::Result result;
    result = graphicsQueue.submit(1, &submitInfo, inFlightFences[currentFrame]);
    PGE_ASSERT(result == vk::Result::eSuccess, "Failed to submit to graphics queue (VKERROR: " + String::hexFromInt((u32)result) + ")");

    vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR(1, &renderFinishedSemaphores[currentFrame], 1, &swapchain, (uint32_t*)&backBufferIndex, nullptr);
    result = presentQueue.presentKHR(presentInfo);
    PGE_ASSERT(result == vk::Result::eSuccess, "Failed to submit to present queue (VKERROR: " + String::hexFromInt((u32)result) + ")");

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    // Wait until the current frames in flight are less than the max.
    result = device->waitForFences(inFlightFences[currentFrame].get(), false, UINT64_MAX);
    PGE_ASSERT(result == vk::Result::eSuccess, "Failed to wait for fences (VKERROR: " + String::hexFromInt((u32)result) + ")");
}

void GraphicsVK::acquireNextImage() {
    backBufferIndex = device->acquireNextImageKHR(swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], nullptr).value;

    // Is the image we just acquired currently still being submitted?
    vk::Result result;
    if (imagesInFlight[backBufferIndex] != (vk::Fence)VK_NULL_HANDLE) {
        // If so, wait on it!
        result = device->waitForFences(imagesInFlight[backBufferIndex], false, UINT64_MAX);
        PGE_ASSERT(result == vk::Result::eSuccess, "Failed to wait for fences (VKERROR: " + String::hexFromInt((u32)result) + ")");
        imagesInFlight[backBufferIndex] = vk::Fence(nullptr);
    }
    imagesInFlight[backBufferIndex] = inFlightFences[currentFrame];

    device->resetCommandPool(comPools[backBufferIndex], {});

    comBuffers[backBufferIndex].begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    vk::RenderPassBeginInfo beginInfo = vk::RenderPassBeginInfo(renderPass, framebuffers[backBufferIndex], scissor);
    comBuffers[backBufferIndex].beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
}

void GraphicsVK::startTransfer() {
    device->resetCommandPool(transferComPool, {});
    transferComBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
}

void GraphicsVK::endTransfer() {
    transferComBuffer.end();
    vk::SubmitInfo sui;
    sui.setCommandBuffers(transferComBuffer);
    transferQueue.submit(sui, nullptr);
    transferQueue.waitIdle();
}

void GraphicsVK::clear(const Color& color) {
    vk::ClearAttachment cl = vk::ClearAttachment(vk::ImageAspectFlagBits::eColor, 0, vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{ { color.red, color.green, color.blue, color.alpha }})));
    vk::ClearRect rect = vk::ClearRect(scissor, 0, 1);
    comBuffers[backBufferIndex].clearAttachments(cl, rect);
}

void GraphicsVK::createSwapchain(bool vsync) {
    VKSwapchain::View oldChain = swapchain;
    swapchain = resourceManager.addNewResource<VKSwapchain>(device, physicalDevice, surface, swapchainExtent, dimensions.x, dimensions.y, swapchainFormat,
        graphicsQueueIndex, presentQueueIndex, transferQueueIndex, vsync, swapchain.isHoldingResource() ? swapchain.get() : VK_NULL_HANDLE);
    resourceManager.deleteResource(oldChain);

    // Creating image views for our swapchain images to ultimately write to.
    std::vector<vk::Image> swapchainImages = device->getSwapchainImagesKHR(swapchain);
    swapchainImageViews.resize((int)swapchainImages.size());
    for (int i = 0; i < (int)swapchainImages.size(); i++) {
        resourceManager.deleteResource(swapchainImageViews[i]);
        swapchainImageViews[i] = resourceManager.addNewResource<VKImageView>(device, swapchainImages[i], swapchainFormat.format);
    }
    
    scissor = vk::Rect2D(vk::Offset2D(0, 0), swapchainExtent);

    pipelineInfo.init(swapchainExtent, scissor);
    Culling oldCull = cullingMode;
    cullingMode = Culling::NONE;
    setCulling(oldCull);

    resourceManager.deleteResource(renderPass);
    renderPass = resourceManager.addNewResource<VKRenderPass>(device, swapchainFormat);

    framebuffers.resize(swapchainImageViews.size());
    for (int i = 0; i < (int)swapchainImageViews.size(); i++) {
        resourceManager.deleteResource(framebuffers[i]);
        framebuffers[i] = resourceManager.addNewResource<VKFramebuffer>(device, renderPass, swapchainImageViews[i], swapchainExtent);
    }

    comPools.resize(framebuffers.size());
    comBuffers.resize(framebuffers.size());
    vk::Result result;
    for (int i = 0; i < (int)framebuffers.size(); i++) {
        resourceManager.deleteResource(comPools[i]);
        comPools[i] = resourceManager.addNewResource<VKCommandPool>(device, graphicsQueueIndex);
        vk::CommandBufferAllocateInfo comBufAllInfo = vk::CommandBufferAllocateInfo(comPools[i], vk::CommandBufferLevel::ePrimary, 1);
        result = device->allocateCommandBuffers(&comBufAllInfo, &comBuffers[i]);
        PGE_ASSERT(result == vk::Result::eSuccess, "Failed to allocate command buffers (VKERROR: " + String::hexFromInt((u32)result) + ")");
    }

    resourceManager.deleteResource(transferComPool);
    transferComPool = resourceManager.addNewResource<VKCommandPool>(device, transferQueueIndex);
    // TODO: How many buffers should we have?
    vk::CommandBufferAllocateInfo transferComBufferInfo = vk::CommandBufferAllocateInfo(transferComPool, vk::CommandBufferLevel::ePrimary, 1);
    result = device->allocateCommandBuffers(&transferComBufferInfo, &transferComBuffer);
    PGE_ASSERT(result == vk::Result::eSuccess, "Failed to allocate transfer command buffers (VKERROR: " + String::hexFromInt((u32)result) + ")");
}

void GraphicsVK::transformImage(vk::Image img, vk::Format fmt, vk::ImageLayout oldL, vk::ImageLayout newL) {
    startTransfer();

    vk::ImageMemoryBarrier barrier;
    barrier.setImage(img);
    barrier.setOldLayout(oldL);
    barrier.setNewLayout(newL);
    barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
    barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;

    vk::PipelineStageFlags srcStage;
    vk::PipelineStageFlags dstStage;

    if (oldL == vk::ImageLayout::eUndefined && newL == vk::ImageLayout::eTransferDstOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eNoneKHR;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
        srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
        dstStage = vk::PipelineStageFlagBits::eTransfer;
    } else if (oldL == vk::ImageLayout::eTransferDstOptimal && newL == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        srcStage = vk::PipelineStageFlagBits::eTransfer;
        dstStage = vk::PipelineStageFlagBits::eFragmentShader;
    } else {
        throw PGE_CREATE_EX("Invalid conversion");
    }

    transferComBuffer.pipelineBarrier(srcStage, dstStage, (vk::DependencyFlags)0, nullptr, nullptr, barrier);

    endTransfer();
}

void GraphicsVK::transferToImage(const vk::Buffer& src, const vk::Image& dst, int w, int h) {
    startTransfer();

    vk::BufferImageCopy copy;
    copy.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    copy.imageSubresource.layerCount = 1;
    copy.imageExtent = vk::Extent3D(w, h, 1);

    transferComBuffer.copyBufferToImage(src, dst, vk::ImageLayout::eTransferDstOptimal, copy);

    endTransfer();
}

void GraphicsVK::setRenderTarget(Texture& renderTarget) {

}

void GraphicsVK::setRenderTargets(const ReferenceVector<Texture>& renderTargets) {

}

void GraphicsVK::resetRenderTarget() {

}

void GraphicsVK::setViewport(const Rectanglei& vp) {

}

void GraphicsVK::setVsync(bool isEnabled) {
    if (isEnabled != vsync) {
        vsync = isEnabled;
        endRender();
        device->waitIdle();
        // TODO: Clean.
        // Don't when ending.
        createSwapchain(isEnabled);
        acquireNextImage();
    }
}

void GraphicsVK::setCulling(Culling mode) {
    if (mode == cullingMode) { return; }

    vk::CullModeFlagBits flags;
    switch (mode) {
        case Graphics::Culling::BACK: { flags = vk::CullModeFlagBits::eBack; } break;
        case Graphics::Culling::FRONT: { flags = vk::CullModeFlagBits::eFront; } break;
        case Graphics::Culling::NONE: { flags = vk::CullModeFlagBits::eNone; } break;
        default: { throw PGE_CREATE_EX("Unexpected culling mode"); }
    }
    pipelineInfo.rasterizationInfo.cullMode = flags;

    for (MeshVK* mesh : meshes) {
        mesh->uploadPipeline();
    }
    
    cullingMode = mode;
}

vk::Device GraphicsVK::getDevice() const {
    return device;
}

vk::PhysicalDevice GraphicsVK::getPhysicalDevice() const {
    return physicalDevice;
}

vk::RenderPass GraphicsVK::getRenderPass() const {
    return renderPass;
}

vk::CommandBuffer GraphicsVK::getCurrentCommandBuffer() const {
    return comBuffers[backBufferIndex];
}

// I fucking hate this, but C++ is stupid.
// When you want to pass something by reference the copy constructor must be available, even though it shouldn't be used!
const VKPipelineInfo* GraphicsVK::getPipelineInfo() const {
    return &pipelineInfo;
}

const vk::Sampler& GraphicsVK::getSampler(bool rt) const {
    return rt ? samplerRT : sampler;
}

const vk::DescriptorSetLayout& GraphicsVK::getDescriptorSetLayout(int count) {
    auto it = dSetLayouts.find(count);
    if (it == dSetLayouts.end()) {
        return dSetLayouts.emplace(count, DescriptorSetLayoutEntry{ resourceManager.addNewResource<VKDescriptorSetLayout>(device, count), 1 }).first->second.layout;
    } else {
        it->second.count++;
        return it->second.layout;
    }
}

void GraphicsVK::dropDescriptorSetLayout(int count) {
    auto it = dSetLayouts.find(count);
    it->second.count--;
    if (it->second.count <= 0) {
        resourceManager.deleteResource(it->second.layout);
        dSetLayouts.erase(it);
    }
}

void GraphicsVK::addMesh(MeshVK& m) {
    meshes.emplace(&m);
}

void GraphicsVK::removeMesh(MeshVK& m) {
    meshes.erase(&m);
}

void GraphicsVK::trash(ResourceBase& res) {
    trashBin.push_back(&res);
}
