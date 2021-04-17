#include "GraphicsVK.h"

#include <set>

#include <Exception/Exception.h>

#include "../Shader/ShaderVK.h"
#include "../Mesh/MeshVK.h"
#include "../Texture/TextureVK.h"

using namespace PGE;

GraphicsVK::GraphicsVK(const String& name, int w, int h, bool fs) : GraphicsInternal(name, w, h, fs, SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI), resourceManager(this, 199) {
    // Layers.
    std::vector<const char*> layers;
#ifdef DEBUG
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    instance = VKInstance::createRef(resourceManager, sdlWindow, name, layers);

    surface = VKSurface::createRef(resourceManager, instance, sdlWindow);

    // The device extensions we need.
    std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    // Selecting a physical device and setting up queues.
    // Currently we just pick the one that supports what we need and has the most VRAM.
    bool foundCompatibleDevice = false;
    vk::DeviceSize selectedPdSize = 0;
    std::vector<vk::SurfaceFormatKHR> selectedPdFormats;
    std::vector<vk::PresentModeKHR> selectedPdPresentModes;
    for (vk::PhysicalDevice pd : instance->enumeratePhysicalDevices()) {
        // The physical device does not support any formats or present modes on this surface, ignore it.
        std::vector<vk::SurfaceFormatKHR> pdFormats;
        std::vector<vk::PresentModeKHR> pdPresentModes;
        if ((pdFormats = pd.getSurfaceFormatsKHR(surface)).empty() || (pdPresentModes = pd.getSurfacePresentModesKHR(surface)).empty()) {
            break;
        }

        // Device must support our needed extensions.
        std::set<long long> extensionCheckSet;
        for (const char* de : deviceExtensions) {
            extensionCheckSet.insert(String(de).getHashCode());
        }
        for (vk::ExtensionProperties ep : pd.enumerateDeviceExtensionProperties()) {
            extensionCheckSet.erase(String((const char*)ep.extensionName).getHashCode());
            if (extensionCheckSet.empty()) {
                break;
            }
        }
        // Physical device does not support all needed extensions.
        if (!extensionCheckSet.empty()) { break; }

        // Checking and setting the queue indices.
        uint32_t i = 0;
        bool foundGraphics = false;
        bool foundPresent = false;
        bool foundTransfer = false;
        for (vk::QueueFamilyProperties qfp : pd.getQueueFamilyProperties()) {
            if (!foundGraphics && qfp.queueFlags & vk::QueueFlagBits::eGraphics) {
                graphicsQueueIndex = i;
                foundGraphics = true;
            }
            if (!foundPresent && pd.getSurfaceSupportKHR(i, surface)) {
                presentQueueIndex = i;
                foundPresent = true;
            }
            if (!foundTransfer && qfp.queueFlags & vk::QueueFlagBits::eTransfer && !(qfp.queueFlags & vk::QueueFlagBits::eGraphics)) {
                transferQueueIndex = i;
                foundTransfer = true;
            }
            if (foundGraphics && foundPresent) {
                break;
            }
            i++;
        }
        // Something we need is not supported on this GPU, ignore it.
        if (!foundGraphics || !foundPresent) { break; }

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
    __ASSERT(foundCompatibleDevice, "No Vulkan compatible GPU found");

    device = VKDevice::createRef(resourceManager, physicalDevice, std::set { graphicsQueueIndex, presentQueueIndex, transferQueueIndex }, layers, deviceExtensions);

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

    imageAvailableSemaphores = ResourceReferenceVector<vk::Semaphore>::withSize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores = ResourceReferenceVector<vk::Semaphore>::withSize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences = ResourceReferenceVector<vk::Fence>::withSize(MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        imageAvailableSemaphores[i] = VKSemaphore::createRef(resourceManager, device);
        renderFinishedSemaphores[i] = VKSemaphore::createRef(resourceManager, device);
        inFlightFences[i] = VKFence::createRef(resourceManager, device, i != 0);
    }
    imagesInFlight.resize(MAX_FRAMES_IN_FLIGHT);
    acquireNextImage();

    open = true;
    focused = true;
}

void GraphicsVK::swap() {
    endRender();
    
    acquireNextImage();
}

void GraphicsVK::endRender() {
    comBuffers[backBufferIndex].endRenderPass();
    comBuffers[backBufferIndex].end();

    vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    device->resetFences(inFlightFences[currentFrame]);
    vk::SubmitInfo submitInfo = vk::SubmitInfo(1, &imageAvailableSemaphores[currentFrame], &waitStages, 1, &comBuffers[backBufferIndex], 1, &renderFinishedSemaphores[currentFrame]);
    vk::Result result;
    result = graphicsQueue.submit(1, &submitInfo, inFlightFences[currentFrame]);
    __ASSERT(result == vk::Result::eSuccess, "Failed to submit to graphics queue (VKERROR: " + String::fromInt((int)result) + ")");

    vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR(1, &renderFinishedSemaphores[currentFrame], 1, &swapchain, (uint32_t*)&backBufferIndex, nullptr);
    result = presentQueue.presentKHR(presentInfo);
    __ASSERT(result == vk::Result::eSuccess, "Failed to submit to present queue (VKERROR: " + String::fromInt((int)result) + ")");

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    // Wait until the current frames in flight are less than the max.
    result = device->waitForFences(inFlightFences[currentFrame], false, UINT64_MAX);
    __ASSERT(result == vk::Result::eSuccess, "Failed to wait for fences (VKERROR: " + String::fromInt((int)result) + ")");
}

void GraphicsVK::acquireNextImage() {
    backBufferIndex = device->acquireNextImageKHR(swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], nullptr).value;

    // Is the image we just acquired currently still being submitted?
    vk::Result result;
    if (imagesInFlight[backBufferIndex] != VK_NULL_HANDLE) {
        // If so, wait on it!
        result = device->waitForFences(imagesInFlight[backBufferIndex], false, UINT64_MAX);
        __ASSERT(result == vk::Result::eSuccess, "Failed to wait for fences (VKERROR: " + String::fromInt((int)result) + ")");
        imagesInFlight[backBufferIndex] = vk::Fence(nullptr);
    }
    imagesInFlight[backBufferIndex] = inFlightFences[currentFrame];

    device->resetCommandPool(comPools[backBufferIndex], {});

    comBuffers[backBufferIndex].begin(vk::CommandBufferBeginInfo());
    vk::RenderPassBeginInfo beginInfo = vk::RenderPassBeginInfo(renderPass, framebuffers[backBufferIndex], scissor);
    comBuffers[backBufferIndex].beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
}

void GraphicsVK::clear(Color color) {
    vk::ClearAttachment cl = vk::ClearAttachment(vk::ImageAspectFlagBits::eColor, 0, vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{ { color.red, color.green, color.blue, color.alpha }})));
    vk::ClearRect rect = vk::ClearRect(scissor, 0, 1);
    comBuffers[backBufferIndex].clearAttachments(cl, rect);
}

// TODO: Optimize.
void GraphicsVK::transfer(const vk::Buffer& src, const vk::Buffer& dst, int size) {
    device->resetCommandPool(transferComPool, {});
    transferComBuffer.begin(vk::CommandBufferBeginInfo({}));
    transferComBuffer.copyBuffer(src, dst, vk::BufferCopy(0, 0, size));
    transferComBuffer.end();
    vk::SubmitInfo sui;
    sui.setCommandBuffers(transferComBuffer);
    transferQueue.submit(sui, nullptr);
    transferQueue.waitIdle();
}

void GraphicsVK::createSwapchain(bool vsync) {
    resourceManager.deleteResourcefromReference(swapchain);
    swapchain = VKSwapchain::createRef(resourceManager, device, physicalDevice, surface, &swapchainExtent, width, height, swapchainFormat,
        graphicsQueueIndex, presentQueueIndex, transferQueueIndex, vsync);

    // Creating image views for our swapchain images to ultimately write to.
    std::vector<vk::Image> swapchainImages = device->getSwapchainImagesKHR(swapchain);
    swapchainImageViews.resize((int)swapchainImages.size());
    for (int i = 0; i < (int)swapchainImages.size(); i++) {
        resourceManager.deleteResource(swapchainImageViews[i]);
        swapchainImageViews[i] = VKImageView::createRef(resourceManager, device, swapchainImages[i], swapchainFormat);
    }
    
    scissor = vk::Rect2D(vk::Offset2D(0, 0), swapchainExtent);

    pipelineInfo.init(swapchainExtent, &scissor);

    resourceManager.deleteResourcefromReference(renderPass);
    renderPass = VKRenderPass::createRef(resourceManager, device, swapchainFormat);

    framebuffers.resize(swapchainImageViews.size());
    for (int i = 0; i < (int)swapchainImageViews.size(); i++) {
        resourceManager.deleteResource(framebuffers[i]);
        framebuffers[i] = VKFramebuffer::createRef(resourceManager, device, renderPass, swapchainImageViews[i], swapchainExtent);
    }

    comPools.resize(framebuffers.size());
    comBuffers.resize(framebuffers.size());
    vk::Result result;
    for (int i = 0; i < (int)framebuffers.size(); i++) {
        resourceManager.deleteResource(comPools[i]);
        comPools[i] = VKCommandPool::createRef(resourceManager, device, graphicsQueueIndex);
        vk::CommandBufferAllocateInfo comBufAllInfo = vk::CommandBufferAllocateInfo(comPools[i], vk::CommandBufferLevel::ePrimary, 1);
        result = device->allocateCommandBuffers(&comBufAllInfo, &comBuffers[i]);
        __ASSERT(result == vk::Result::eSuccess, "Failed to allocate command buffers (VKERROR: " + String::fromInt((int)result) + ")");
    }

    resourceManager.deleteResourcefromReference(transferComPool);
    transferComPool = VKCommandPool::createRef(resourceManager, device, transferQueueIndex);
    // TODO: How many buffers should we have?
    vk::CommandBufferAllocateInfo transferComBufferInfo = vk::CommandBufferAllocateInfo(transferComPool, vk::CommandBufferLevel::ePrimary, 1);
    result = device->allocateCommandBuffers(&transferComBufferInfo, &transferComBuffer);
    __ASSERT(result == vk::Result::eSuccess, "Failed to allocate transfer command buffers (VKERROR: " + String::fromInt((int)result) + ")");
}

void GraphicsVK::setRenderTarget(Texture* renderTarget) {

}

void GraphicsVK::setRenderTargets(const std::vector<Texture*>& renderTargets) {

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

vk::Device GraphicsVK::getDevice() const {
    return device;
}

vk::PhysicalDevice GraphicsVK::getPhysicalDevice() const {
    return physicalDevice;
}

VKPipelineInfo& GraphicsVK::getPipelineInfo() {
    return pipelineInfo;
}

vk::RenderPass GraphicsVK::getRenderPass() const {
    return renderPass;
}

vk::CommandBuffer GraphicsVK::getCurrentCommandBuffer() const {
    return comBuffers[backBufferIndex];
}

__GFX_OBJ_DEF(VK)
