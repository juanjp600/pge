#include "GraphicsVK.h"

#include <PGE/Exception/Exception.h>

#include "Shader/ShaderVK.h"
#include "Mesh/MeshVK.h"
#include "Texture/TextureVK.h"

using namespace PGE;

GraphicsVK::GraphicsVK(const String& name, int w, int h, WindowMode wm, int x, int y)
    : GraphicsSpecialized("Vulkan", name, w, h, wm, x, y, SDL_WINDOW_VULKAN), resourceManager(*this) {
    std::vector<const char*> layers;
#ifdef DEBUG
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    instance = resourceManager.addNewResource<VKInstance>(sdlWindow, name, layers);
    dispatch = vk::DispatchLoaderDynamic(instance.get(), vkGetInstanceProcAddr);

    surface = resourceManager.addNewResource<VKSurface>(instance, sdlWindow);

    // The device extensions we need.
    std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_EXT_EXTENDED_DYNAMIC_STATE_EXTENSION_NAME };

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
            atomSize = pd.getProperties().limits.nonCoherentAtomSize;
        }
    }
    PGE_ASSERT(foundCompatibleDevice, "No Vulkan compatible GPU found");

    device = resourceManager.addNewResource<VKDevice>(physicalDevice,
        std::unordered_set { graphicsQueueIndex, presentQueueIndex, transferQueueIndex }, deviceExtensions);

    graphicsQueue = device->getQueue(graphicsQueueIndex, 0);
    presentQueue = device->getQueue(presentQueueIndex, 0);
    transferQueue = device->getQueue(transferQueueIndex, 0);

    // Selecting the right surface format.
    // If we don't find the one we want we'll settle for the first available one.
    swapchainFormat = selectedPdFormats[0];
    for (const auto& f : selectedPdFormats) {
        if (f.format == vk::Format::eR8G8B8Unorm) {
            swapchainFormat = f;
            break;
        }
    }

    scissor = vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(w, h));
    pipelineInfo.viewportInfo.setScissors(scissor);

    setViewport(Rectanglei(0, 0, w, h));

    vsync = true;
    createSwapchain();

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
    startRender();

    setDepthTest(true);

    sampler = resourceManager.addNewResource<VKSampler>(device, false);
    samplerRT = resourceManager.addNewResource<VKSampler>(device, true);

    open = true;
    focused = true;
}

GraphicsVK::~GraphicsVK() {
    clearBin();
}

void GraphicsVK::swap() {
    endRender();
    present();

    checkCachedBufferShrink();
    clearBin();

    advanceFrame();

    acquireNextImage();
    startRender();
}

void GraphicsVK::endRender() {
    comBuffers[backBufferIndex].endRenderPass();
    comBuffers[backBufferIndex].end();

    vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    device->resetFences(inFlightFences[currentFrame].get());
    vk::SubmitInfo submitInfo;
    submitInfo.setWaitDstStageMask(waitStages);
    submitInfo.setWaitSemaphores(imageAvailableSemaphores[currentFrame].get());
    submitInfo.setCommandBuffers(comBuffers[backBufferIndex]);
    submitInfo.setSignalSemaphores(renderFinishedSemaphores[currentFrame].get());
    vk::Result result = graphicsQueue.submit(1, &submitInfo, inFlightFences[currentFrame]);
    PGE_ASSERT(result == vk::Result::eSuccess, "Failed to submit to graphics queue (VKERROR: " + String::hexFromInt((u32)result) + ")");
}

void GraphicsVK::present() {
    vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR(1, &renderFinishedSemaphores[currentFrame], 1, &swapchain, (uint32_t*)&backBufferIndex, nullptr);
    vk::Result result = presentQueue.presentKHR(presentInfo);
    PGE_ASSERT(result == vk::Result::eSuccess, "Failed to submit to present queue (VKERROR: " + String::hexFromInt((u32)result) + ")");
}

void GraphicsVK::advanceFrame() {
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    // Wait until the current frames in flight are less than the max.
    vk::Result result = device->waitForFences(inFlightFences[currentFrame].get(), false, UINT64_MAX);
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
}

void GraphicsVK::startRender() {
    device->resetCommandPool(comPools[backBufferIndex], {});

    comBuffers[backBufferIndex].begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    vk::RenderPassBeginInfo beginInfo;
    if (renderTarget == nullptr) {
        frameScissor = scissor;
        beginInfo.renderPass = renderPass;
        beginInfo.framebuffer = framebuffers[backBufferIndex];
    } else {
        frameScissor = renderTarget->getScissor();
        beginInfo.renderPass = renderTarget->getRenderPass();
        beginInfo.framebuffer = renderTarget->getFramebuffer();
    }
    beginInfo.renderArea = frameScissor;
    comBuffers[backBufferIndex].beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
    comBuffers[backBufferIndex].setViewport(0, vkViewport);
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

void GraphicsVK::clear(const Color& cc) {
    vk::ClearAttachment color(vk::ImageAspectFlagBits::eColor, 0, vk::ClearValue(vk::ClearColorValue(std::array{ cc.red, cc.green, cc.blue, cc.alpha })));
    vk::ClearAttachment depth;
    depth.aspectMask = vk::ImageAspectFlagBits::eDepth;
    depth.clearValue = vk::ClearValue(vk::ClearDepthStencilValue(1.f));
    vk::ClearRect rect = vk::ClearRect(frameScissor, 0, 1);

    std::array attachments { color, depth };

    comBuffers[backBufferIndex].clearAttachments(attachments, rect);
}

void GraphicsVK::clearBin() {
    if (!trashBin.empty()) {
        device->waitIdle();
        // Clear trashbin.
        for (ResourceBase* b : trashBin) {
            delete b;
        }
        trashBin.clear();
    }
}

void GraphicsVK::createSwapchain() {
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

    resourceManager.deleteResource(depthBuffer);
    depthBuffer = resourceManager.addNewResource<RawWrapper<TextureVK>>(*this, dimensions.x, dimensions.y);

    Culling oldCull = cullingMode;
    cullingMode = Culling::NONE;
    setCulling(oldCull);

    resourceManager.deleteResource(renderPass);
    renderPass = resourceManager.addNewResource<VKRenderPass>(device, swapchainFormat.format);

    framebuffers.resize(swapchainImageViews.size());
    for (int i = 0; i < (int)swapchainImageViews.size(); i++) {
        resourceManager.deleteResource(framebuffers[i]);
        framebuffers[i] = resourceManager.addNewResource<VKFramebuffer>(device, renderPass, swapchainImageViews[i], depthBuffer->getImageView(), swapchainExtent);
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

void GraphicsVK::generateMipmaps(vk::Image img, int w, int h, int miplevels) {
    startTransfer();

    vk::ImageMemoryBarrier barrier = createBasicBarrier(img, 1);
    barrier.subresourceRange.levelCount = 1;

    int mipWidth = w; int mipHeight = h;
    for (int i = 1; i < miplevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        pipelineBarrier<ImageLayout::TRANSFER_DST, ImageLayout::TRANSFER_SRC>(barrier);

        vk::ImageBlit blit;
        blit.srcOffsets[1] = vk::Offset3D(mipWidth, mipHeight, 1);
        blit.srcSubresource = createBasicImgSubresLayers(i - 1);
        if (mipWidth > 1) { mipWidth /= 2; }
        if (mipHeight > 1) { mipHeight /= 2; }
        blit.dstOffsets[1] = vk::Offset3D(mipWidth, mipHeight, 1);
        blit.dstSubresource = createBasicImgSubresLayers(i);

        // TODO: Allow for blitting to not be supported?
        // TODO: Can only blit on queue with gfx support, consider that.
        transferComBuffer.blitImage(
            img, vk::ImageLayout::eTransferSrcOptimal,
            img, vk::ImageLayout::eTransferDstOptimal,
            blit, vk::Filter::eLinear
        );
    }

    barrier.subresourceRange.baseMipLevel = miplevels - 1;
    pipelineBarrier<ImageLayout::TRANSFER_DST, ImageLayout::SHADER_READ>(barrier);

    // TODO: Keep this in the loop? (Benchmark)
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = miplevels - 1;
    pipelineBarrier<ImageLayout::TRANSFER_SRC, ImageLayout::SHADER_READ>(barrier);

    endTransfer();
}

void GraphicsVK::transferToImage(const vk::Buffer& src, const vk::Image& dst, int w, int h, int miplevel) {
    startTransfer();

    vk::BufferImageCopy copy;
    copy.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
    copy.imageSubresource.layerCount = 1;
    copy.imageSubresource.mipLevel = miplevel;
    copy.imageExtent = vk::Extent3D(w, h, 1);

    transferComBuffer.copyBufferToImage(src, dst, vk::ImageLayout::eTransferDstOptimal, copy);

    endTransfer();
}

void GraphicsVK::setRenderTarget(Texture& rt) {
    if (renderTarget == nullptr) { oldSwapchainBackBufferIndex = backBufferIndex; }

    endRender();

    renderTarget = (TextureVK*)&rt;

    advanceFrame();

    backBufferIndex = (backBufferIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    startRender();
}

void GraphicsVK::setRenderTargets(const ReferenceVector<Texture>& rt) {
    comBuffers[backBufferIndex].endRenderPass();
    comBuffers[backBufferIndex].end();

}

void GraphicsVK::resetRenderTarget() {
    endRender();

    renderTarget = nullptr;

    advanceFrame();

    backBufferIndex = oldSwapchainBackBufferIndex;
    startRender();
}

static constexpr vk::Viewport convertViewport(const Rectanglei& rect) {
    return vk::Viewport(rect.topLeftCorner().x, rect.topLeftCorner().y + rect.height(), rect.width(), -rect.height(), 0.f, 1.f);
}

void GraphicsVK::setViewport(const Rectanglei& vp) {
    viewport = vp;
    vkViewport = convertViewport(viewport);
}

void GraphicsVK::setDepthTest(bool isEnabled) {
    depthTest = isEnabled;
    comBuffers[backBufferIndex].setDepthTestEnableEXT(isEnabled, dispatch);
}

void GraphicsVK::setVsync(bool isEnabled) {
    if (isEnabled != vsync) {
        vsync = isEnabled;
        endRender();
        present(); // TODO: Why present?
        advanceFrame();
        device->waitIdle();
        // TODO: Clean.
        // Don't when ending.
        createSwapchain();
        acquireNextImage();
        startRender();
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
    reuploadPipelines();
    
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

const VKPipelineInfo& GraphicsVK::getPipelineInfo() const {
    return pipelineInfo;
}

const vk::Sampler& GraphicsVK::getSampler(bool rt) const {
    return rt ? samplerRT : sampler;
}

vk::DeviceSize GraphicsVK::getAtomSize() const {
    return atomSize;
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

void GraphicsVK::addShader(ShaderVK& m) {
    shaders.emplace(&m);
}

void GraphicsVK::removeShader(ShaderVK& m) {
    shaders.erase(&m);
}

void GraphicsVK::trash(ResourceBase& res) {
    trashBin.push_back(&res);
}

vk::RenderPass GraphicsVK::getRenderPass(vk::Format fmt) {
    return *renderPasses[fmt].pass;
}

vk::RenderPass GraphicsVK::requestRenderPass(vk::Format fmt) {
    FormatRenderPass& pass = renderPasses[fmt];
    if (pass.count == 0) { pass.pass = new VKRenderPass(device, fmt, true); }
    pass.count++;
    return *pass.pass;
}

void GraphicsVK::returnRenderPass(vk::Format fmt) {
    FormatRenderPass& pass = renderPasses[fmt];
    pass.count--;
    if (pass.count == 0) { delete pass.pass; }
}

std::optional<vk::Format> GraphicsVK::getRenderTargetFormat() const {
    if (renderTarget == nullptr) {
        return { };
    } else {
        return renderTarget->getFormat();
    }
}

// Explanation of the staging buffer cache:
// By utilizing getTempStagingBuffer anyone can get a potentially cached buffer.
// If getTempStagingBuffer needs to create a new buffer, that one is cached.
// We check if we should shrink the cached buffer after every presentation BEFORE the trashbin is emptied.
// (We need to do it before since we need to wait for device idle and we otherwise can't rely on a non-empty trashbin.)
// All in all this system should allow even static meshes to have access to appropriately sized cached buffers,
// at least in cases where a lot of them are created in the same frame (e.g. when loading).

VKMemoryBuffer& GraphicsVK::getTempStagingBuffer(int size) {
    if (size > cachedBufferSize) { updateCachedBuffer(size); }
    return *cachedBuffer;
}

VKMemoryBuffer& GraphicsVK::registerStagingBuffer(int size) {
    cachedBufferSizesSet.emplace(size);
    return getTempStagingBuffer(size);
}

void GraphicsVK::unregisterStagingBuffer(int size) {
    cachedBufferSizesSet.erase(cachedBufferSizesSet.find(size));
}

void GraphicsVK::checkCachedBufferShrink() {
    if (cachedBufferSizesSet.empty()) { return; }
    int max = *cachedBufferSizesSet.rbegin();
    if (max * 4 < cachedBufferSize) {
        updateCachedBuffer(max);
    }
}

void GraphicsVK::updateCachedBuffer(int size) {
    resourceManager.trash(cachedBuffer);
    cachedBuffer = resourceManager.addNewResource<RawWrapper<VKMemoryBuffer>>(device, physicalDevice, size, VKMemoryBuffer::Type::STAGING);
    cachedBufferSize = size;
}

void GraphicsVK::reuploadPipelines() {
    for (ShaderVK* sh : shaders) {
        sh->uploadPipelines();
    }
}
