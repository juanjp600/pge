#include "GraphicsVK.h"

#include <PGE/Exception/Exception.h>
#include <PGE/Math/Hasher.h>

#include "Shader/ShaderVK.h"
#include "Mesh/MeshVK.h"
#include "Texture/TextureVK.h"

using namespace PGE;

// TODO: <3 doesn't work! Also replace the vectors with arrays.
constexpr int MAX_FRAMES_IN_FLIGHT = 3;

static constexpr vk::Viewport convertViewport(const Rectanglei& rect) {
    return vk::Viewport(rect.topLeftCorner().x, rect.topLeftCorner().y + rect.height(), rect.width(), -rect.height(), 0.f, 1.f);
}

GraphicsVK::GraphicsVK(const String& name, int w, int h, WindowMode wm, int x, int y)
    : GraphicsSpecialized(name, w, h, wm, x, y, SDL_WINDOW_VULKAN), resourceManager(*this) {
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

        // Doesn't support depth buffering, ignore it.
        if (!(pd.getFormatProperties(UtilVK::DEPTH_FORMAT).optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)) {
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
        for (u32 i : Range(pdmp.memoryHeapCount)) {
            pdSize += pdmp.memoryHeaps[i].size;
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
    asrt(foundCompatibleDevice, "No Vulkan compatible GPU found");

    device = resourceManager.addNewResource<VKDevice>(physicalDevice,
        std::unordered_set { graphicsQueueIndex, presentQueueIndex, transferQueueIndex }, deviceExtensions);

    trashBin.init(device);

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

    viewport = Rectanglei(0, 0, w, h);
    vkViewport = convertViewport(viewport);

    scissor = vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(w, h));
    pipelineInfo.viewportInfo.setScissors(scissor);

    vsync = true;
    createSwapchain();

    imageAvailableSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.reserve(MAX_FRAMES_IN_FLIGHT);
    fenceSentWithComBuffer.resize(MAX_FRAMES_IN_FLIGHT);
    for (int i : Range(MAX_FRAMES_IN_FLIGHT)) {
        imageAvailableSemaphores.emplace_back(resourceManager.addNewResource<VKSemaphore>(device));
        renderFinishedSemaphores.emplace_back(resourceManager.addNewResource<VKSemaphore>(device));
        inFlightFences.emplace_back(resourceManager.addNewResource<VKFence>(device));
        fenceSentWithComBuffer[i] = inFlightFences[i];
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

void GraphicsVK::swap() {
    submit<true>();

    checkCachedBufferShrink();
    trashBin.clear();

    frameCounter++;

    acquireNextImage();
    startRender();
}

template <bool PRESENT>
void GraphicsVK::submit() {
    comBuffers[backBufferIndex].endRenderPass();
    comBuffers[backBufferIndex].end();

    vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    device->resetFences(inFlightFences[currentFrame].get());
    vk::SubmitInfo submitInfo;
    if constexpr (PRESENT) {
        submitInfo.setWaitDstStageMask(waitStages);
        submitInfo.setWaitSemaphores(imageAvailableSemaphores[acquiredIndex].get());
        acquiredIndex = (acquiredIndex + 1) % swapchainImageViews.size();

        submitInfo.setSignalSemaphores(renderFinishedSemaphores[currentFrame].get());
    }
    submitInfo.setCommandBuffers(comBuffers[backBufferIndex]);
    vk::Result result;
    result = graphicsQueue.submit(1, &submitInfo, inFlightFences[currentFrame]);
    fenceSentWithComBuffer[backBufferIndex] = inFlightFences[currentFrame];
    assertVKResult(result, "Failed to submit to graphics queue");

    if constexpr (PRESENT) {
        vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR(1, &renderFinishedSemaphores[currentFrame], 1, &swapchain, (uint32_t*)&backBufferIndex);
        result = presentQueue.presentKHR(presentInfo);
        assertVKResult(result, "Failed to submit to present queue");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void GraphicsVK::acquireNextImage() {
    backBufferIndex = device->acquireNextImageKHR(swapchain, UINT64_MAX, imageAvailableSemaphores[acquiredIndex], VK_NULL_HANDLE).value;

    // Is the image we just acquired currently still being submitted?
    vk::Result result;
    if (imagesInFlight[backBufferIndex]) {
        // If so, wait on it!
        result = device->waitForFences(imagesInFlight[backBufferIndex], false, UINT64_MAX);
        assertVKResult(result, "Failed to wait for fences");
    }
    imagesInFlight[backBufferIndex] = inFlightFences[currentFrame];
}

void GraphicsVK::startRender() {
    vk::Result result = device->waitForFences(fenceSentWithComBuffer[backBufferIndex], false, UINT64_MAX);
    assertVKResult(result, "Failed to wait for fences");
    device->resetCommandPool(comPools[backBufferIndex], {});

    comBuffers[backBufferIndex].begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
    vk::RenderPassBeginInfo beginInfo;
    if (rtRenderInfoOverride == nullptr) {
        frameScissor = scissor;
        beginInfo.renderPass = renderPass;
        beginInfo.framebuffer = framebuffers[backBufferIndex];
    } else {
        frameScissor = rtScissor;
        beginInfo.renderPass = rtRenderInfoOverride->pass;
        beginInfo.framebuffer = rtRenderInfoOverride->buffer;
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
    transferQueue.submit(sui, VK_NULL_HANDLE);
    transferQueue.waitIdle();
}

void GraphicsVK::clear(const Color& color) {
    vk::ClearRect rect = vk::ClearRect(frameScissor, 0, 1);

    vk::ClearAttachment colorAttachment;
    colorAttachment.aspectMask = vk::ImageAspectFlagBits::eColor;
    colorAttachment.clearValue = vk::ClearColorValue(std::array{ color.red, color.green, color.blue, color.alpha });

    static const vk::ClearAttachment DEPTH_CLEAR_ATTACHMENT = []() {
        vk::ClearAttachment depth;
        depth.aspectMask = vk::ImageAspectFlagBits::eDepth;
        depth.clearValue = vk::ClearDepthStencilValue(1.f);
        return depth;
    }();

    if (rtCount > 1) {
        if (rtCount > clearAttachments.size()) {
            clearAttachments.resize(rtCount + 1);
        }
        for (vk::ClearAttachment& attach : clearAttachments) {
            attach = colorAttachment;
        }
        clearAttachments[rtCount] = DEPTH_CLEAR_ATTACHMENT;

        comBuffers[backBufferIndex].clearAttachments(rtCount + 1, clearAttachments.data(), 1, &rect);
    } else {
        comBuffers[backBufferIndex].clearAttachments({ colorAttachment, DEPTH_CLEAR_ATTACHMENT }, rect);
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
    for (size_t i : Range(swapchainImages.size())) {
        resourceManager.deleteResource(swapchainImageViews[i]);
        swapchainImageViews[i] = resourceManager.addNewResource<VKImageView>(device, swapchainImages[i], swapchainFormat.format);
    }

    resourceManager.deleteResource(depthBuffer);
    depthBuffer = resourceManager.addNewResource<RawWrapper<TextureVK>>(*this, dimensions.x, dimensions.y);

    CullingMode oldCull = cullingMode;
    cullingMode = CullingMode::NONE;
    setCulling(oldCull);

    resourceManager.deleteResource(renderPass);
    renderPass = resourceManager.addNewResource<VKRenderPass>(device, swapchainFormat.format);

    framebuffers.resize(swapchainImageViews.size());
    for (size_t i : Range(swapchainImageViews.size())) {
        resourceManager.deleteResource(framebuffers[i]);
        framebuffers[i] = resourceManager.addNewResource<VKFramebuffer>(device, renderPass, swapchainImageViews[i], depthBuffer->getImageView(), swapchainExtent);
    }

    comPools.resize(framebuffers.size());
    comBuffers.resize(framebuffers.size());
    vk::Result result;
    for (size_t i : Range(framebuffers.size())) {
        resourceManager.deleteResource(comPools[i]);
        comPools[i] = resourceManager.addNewResource<VKCommandPool>(device, graphicsQueueIndex);
        vk::CommandBufferAllocateInfo comBufAllInfo = vk::CommandBufferAllocateInfo(comPools[i], vk::CommandBufferLevel::ePrimary, 1);
        result = device->allocateCommandBuffers(&comBufAllInfo, &comBuffers[i]);
        assertVKResult(result, "Failed to allocate command buffers");
    }

    resourceManager.deleteResource(transferComPool);
    transferComPool = resourceManager.addNewResource<VKCommandPool>(device, transferQueueIndex);
    // TODO: How many buffers should we have?
    vk::CommandBufferAllocateInfo transferComBufferInfo = vk::CommandBufferAllocateInfo(transferComPool, vk::CommandBufferLevel::ePrimary, 1);
    result = device->allocateCommandBuffers(&transferComBufferInfo, &transferComBuffer);
    assertVKResult(result, "Failed to allocate transfer command buffers");
}

constexpr vk::ImageSubresourceLayers createBasicImgSubresLayers(int miplevel) {
    vk::ImageSubresourceLayers layers;
    layers.aspectMask = vk::ImageAspectFlagBits::eColor;
    layers.mipLevel = miplevel;
    layers.layerCount = 1;
    return layers;
}

void GraphicsVK::generateMipmaps(vk::Image img, int w, int h, int miplevels) {
    startTransfer();

    vk::ImageMemoryBarrier barrier = UtilVK::createBasicBarrier(img, 1);
    barrier.subresourceRange.levelCount = 1;

    int mipWidth = w; int mipHeight = h;
    for (int i : Range(1, miplevels)) {
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
    if (rtRenderInfoOverride == nullptr) { oldSwapchainBackBufferIndex = backBufferIndex; }
    
    submit<false>();

    pipelineInfo.resize(1);
    pipelineInfo.viewportInfo.setScissors(((RenderTextureVK&)rt).getScissor());

    RenderTextureVK& target = ((RenderTextureVK&)rt);
    rtRenderInfoOverride = &target.getRenderInfo();
    rtScissor = target.getScissor();
    rtCount = 1;

    backBufferIndex = (backBufferIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    startRender();
}

void GraphicsVK::setRenderTargets(const ReferenceVector<Texture>& rt) {
    if (rtRenderInfoOverride == nullptr) { oldSwapchainBackBufferIndex = backBufferIndex; }

    submit<false>();

    pipelineInfo.resize(rt.size());
    pipelineInfo.viewportInfo.setScissors(((RenderTextureVK&)rt[0].get()).getScissor());

    Hasher hasher;
    for (const Texture& tex : rt) {
        hasher.feed((size_t)&tex);
    }
    MultiRTID hash = hasher.getHash();
    auto it = multiRenderTargetInfos.find(hash);
    if (it == multiRenderTargetInfos.end()) {
        MultiRTResources newRes;
        newRes.id = hash;
        // TODO: C++20 Do this with views.
        std::vector<vk::Format> fmts; fmts.reserve(rt.size());
        std::vector<vk::ImageView> views; views.reserve(rt.size() + 1);
        for (const Texture& t : rt) {
            fmts.push_back(((RenderTextureVK&)t).getFormat());
            views.push_back(((TextureVK&)t).getImageView());
            ((RenderTextureVK&)t).associateMultiRTResources(hash);
        }
        views.push_back(((RenderTextureVK&)rt[0].get()).getDepthView());
        newRes.pass = new VKRenderPass(device, fmts);
        newRes.buffer = new VKFramebuffer(device, *newRes.pass, views, vk::Extent2D(rt[0]->getWidth(), rt[0]->getHeight()));
        newRes.info = RenderInfo{ *newRes.pass, *newRes.buffer };
        it = multiRenderTargetInfos.emplace(hash, std::move(newRes)).first;
    }
    rtRenderInfoOverride = &it->second.info;
    rtScissor = ((RenderTextureVK&)rt[0].get()).getScissor();
    rtCount = rt.size();

    backBufferIndex = (backBufferIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    startRender();
}

void GraphicsVK::resetRenderTarget() {
    submit<false>();

    pipelineInfo.resize(1);
    pipelineInfo.viewportInfo.setScissors(scissor);

    rtRenderInfoOverride = nullptr;
    rtCount = 1;

    backBufferIndex = oldSwapchainBackBufferIndex;
    startRender();
}

void GraphicsVK::setViewport(const Rectanglei& vp) {
    viewport = vp;
    vkViewport = convertViewport(viewport);
    comBuffers[backBufferIndex].setViewport(0, convertViewport(viewport));
}

void GraphicsVK::setDepthTest(bool isEnabled) {
    depthTest = isEnabled;
    comBuffers[backBufferIndex].setDepthTestEnableEXT(isEnabled, dispatch);
}

void GraphicsVK::setVsync(bool isEnabled) {
    if (isEnabled != vsync) {
        vsync = isEnabled;
        // We need to present here, this causes a 1 frame flicker, shame!
        submit<true>();
        device->waitIdle();
        // TODO: Clean.
        // Don't when ending.
        createSwapchain();
        acquireNextImage();
        startRender();
    }
}

void GraphicsVK::setCulling(CullingMode mode) {
    if (mode == cullingMode) { return; }

    vk::CullModeFlagBits flags;
    switch (mode) {
        using enum CullingMode;
        case BACK: { flags = vk::CullModeFlagBits::eBack; } break;
        case FRONT: { flags = vk::CullModeFlagBits::eFront; } break;
        case NONE: { flags = vk::CullModeFlagBits::eNone; } break;
        default: { throw Exception("Unexpected culling mode"); }
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

vk::RenderPass GraphicsVK::getBasicRenderPass() const {
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

u64 GraphicsVK::getFrameCounter() const {
    return frameCounter;
}

vk::DeviceSize GraphicsVK::getAtomSize() const {
    return atomSize;
}

void GraphicsVK::destroyMultiRTResources(MultiRTID id) {
    auto it = multiRenderTargetInfos.find(id);
    if (it != multiRenderTargetInfos.end()) {
        delete it->second.pass;
        delete it->second.buffer;
        multiRenderTargetInfos.erase(it);
    }
}

const vk::DescriptorSetLayout& GraphicsVK::getDescriptorSetLayout(int count) {
    auto it = dSetLayouts.find(count);
    if (it == dSetLayouts.end()) {
        return *dSetLayouts.emplace(count, DescriptorSetLayoutEntry{ new VKDescriptorSetLayout(device, count), 1 }).first->second.layout;
    } else {
        it->second.count++;
        return *it->second.layout;
    }
}

void GraphicsVK::dropDescriptorSetLayout(int count) {
    auto it = dSetLayouts.find(count);
    it->second.count--;
    if (it->second.count <= 0) {
        delete it->second.layout;
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
    trashBin.add(res);
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
    cachedBuffer = resourceManager.addNewResource<RawWrapper<VKMemoryBuffer>>(device, physicalDevice, atomSize, size, VKMemoryBuffer::Type::STAGING);
    cachedBufferSize = size;
}

void GraphicsVK::reuploadPipelines() {
    for (ShaderVK* sh : shaders) {
        sh->uploadPipelines();
    }
}

const RenderInfo* GraphicsVK::getRenderInfo() const {
    return rtRenderInfoOverride;
}
