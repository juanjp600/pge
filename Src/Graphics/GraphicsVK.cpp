#include "GraphicsVK.h"

#include <set>

#include <SDL_vulkan.h>

#include <Exception/Exception.h>

#include "../Shader/ShaderVK.h"

using namespace PGE;

GraphicsVK::OpDeviceIdle::OpDeviceIdle(vk::Device device) {
    this->device = device;
}

void GraphicsVK::OpDeviceIdle::exec() {
    device.waitIdle();
}

GraphicsVK::GraphicsVK(String name, int w, int h, bool fs) : GraphicsInternal(name, w, h, fs), MAX_FRAMES_IN_FLIGHT(3) {
    currentFrame = 0;

    // SDL window creation.
    sdlWindow = SDL_CreateWindow(name.cstr(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (sdlWindow() == nullptr) {
        throw Exception("WindowVK", "Failed to create SDL window: " + String(SDL_GetError()));
    }

    // Layers.
    std::vector<const char*> layers;
#ifdef DEBUG
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    // Extensions (currently only SDL required ones).
    unsigned int extensionCount = 0;
    SDL_Vulkan_GetInstanceExtensions(sdlWindow(), &extensionCount, nullptr);
    const char** extensionData = new const char*[extensionCount];
    SDL_Vulkan_GetInstanceExtensions(sdlWindow(), &extensionCount, extensionData);
    std::vector<const char*> extensions = std::vector<const char*>(extensionData, extensionData + extensionCount);
    delete[] extensionData;

    vk::ApplicationInfo vkAppInfo = vk::ApplicationInfo(name.cstr(), VK_MAKE_VERSION(0, 0, 0), "pulsegun engine", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_1);
    vkInstance = vk::createInstance(vk::InstanceCreateInfo({}, &vkAppInfo, layers, extensions));

    // Creating the window's surface via SDL.
    VkSurfaceKHR tmpSurface;
    if (!SDL_Vulkan_CreateSurface(sdlWindow(), (VkInstance)vkInstance, &tmpSurface)) {
        throw Exception("WindowVK", "Failed to create Vulkan surface: " + String(SDL_GetError()));
    }
    vkSurface = vk::SurfaceKHR(tmpSurface);

    // The device extensions we need.
    std::array deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    // Selecting a physical device and setting up queues.
    // Currently we just pick the one that supports what we need and has the most VRAM.
    bool foundCompatibleDevice = false;
    vk::DeviceSize selectedPdSize = 0;
    std::vector<vk::SurfaceFormatKHR> selectedPdFormats;
    std::vector<vk::PresentModeKHR> selectedPdPresentModes;
    for (vk::PhysicalDevice& pd : vkInstance.enumeratePhysicalDevices()) {
        // The physical device does not support any formats or present modes on this surface, ignore it.
        std::vector<vk::SurfaceFormatKHR> pdFormats;
        std::vector<vk::PresentModeKHR> pdPresentModes;
        if ((pdFormats = pd.getSurfaceFormatsKHR(vkSurface)).empty() || (pdPresentModes = pd.getSurfacePresentModesKHR(vkSurface)).empty()) {
            break;
        }

        // Device must support our needed extensions.
        std::set<String> extensionCheckSet;
        for (const char* de : deviceExtensions) {
            extensionCheckSet.insert(de);
        }
        for (const auto& ep : pd.enumerateDeviceExtensionProperties()) {
            extensionCheckSet.erase((const char*)ep.extensionName);
            if (extensionCheckSet.empty()) {
                break;
            }
        }
        if (!extensionCheckSet.empty()) { break; }

        // Checking and setting the queue indices.
        uint32_t i = 0;
        bool foundGraphics = false;
        bool foundPresent = false;
        bool foundTransfer = false;
        for (const auto& qfp : pd.getQueueFamilyProperties()) {
            if (!foundGraphics && qfp.queueFlags & vk::QueueFlagBits::eGraphics) {
                graphicsQueueIndex = i;
                foundGraphics = true;
            }
            if (!foundPresent && pd.getSurfaceSupportKHR(i, vkSurface)) {
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
    if (!foundCompatibleDevice) {
        throw Exception("WindowVK", "No Vulkan compatible GPU found!");
    }

    // Creating the logical device.
    float queuePriority = 1.f;
    std::set<uint32_t> queueIndices = { graphicsQueueIndex, presentQueueIndex, transferQueueIndex };
    std::vector<vk::DeviceQueueCreateInfo> infos;
    for (int index : queueIndices) {
        infos.push_back(vk::DeviceQueueCreateInfo({}, index, 1, &queuePriority));
    }
    device = physicalDevice.createDevice(vk::DeviceCreateInfo({}, infos, layers, deviceExtensions));

    graphicsQueue = device.getQueue(graphicsQueueIndex, 0);
    presentQueue = device.getQueue(presentQueueIndex, 0);
    transferQueue = device.getQueue(transferQueueIndex, 0);

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

    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        imageAvailableSemaphores[i] = device.createSemaphore(vk::SemaphoreCreateInfo({}));
        renderFinishedSemaphores[i] = device.createSemaphore(vk::SemaphoreCreateInfo({}));
        inFlightFences[i] = device.createFence(vk::FenceCreateInfo({ i != 0 ? vk::FenceCreateFlagBits::eSignaled : (vk::FenceCreateFlags)0 }));
    }
    imagesInFlight.resize(swapchainImageViews.size());
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

    device.resetFences(inFlightFences[currentFrame]);
    vk::SubmitInfo submitInfo = vk::SubmitInfo(1, &imageAvailableSemaphores[currentFrame], &waitStages, 1, &comBuffers[backBufferIndex], 1, &renderFinishedSemaphores[currentFrame]);
    vk::Result result;
    if ((result = graphicsQueue.submit(1, &submitInfo, inFlightFences[currentFrame])) != vk::Result::eSuccess) {
        throw Exception("GraphicsVK::endRender", "Failed to submit to graphics queue with error: " + String::fromInt((int)result));
    }

    vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR(1, &renderFinishedSemaphores[currentFrame], 1, &swapchain, &backBufferIndex, nullptr);
    if ((result = presentQueue.presentKHR(presentInfo)) != vk::Result::eSuccess) {
        throw Exception("GraphicsVK::endRender", "Failed to submit to present queue with error: " + String::fromInt((int)result));
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    // Wait until the current frames in flight are less than the max.
    if ((result = device.waitForFences(inFlightFences[currentFrame], false, UINT64_MAX)) != vk::Result::eSuccess) {
        throw Exception("GraphicsVK::endRender", "Failed to wait for fences with error: " + String::fromInt((int)result));
    }
}

void GraphicsVK::acquireNextImage() {
    backBufferIndex = device.acquireNextImageKHR(swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], nullptr).value;

    // Is the image we just acquired currently still being submitted?
    vk::Result result;
    if (imagesInFlight[backBufferIndex] != VK_NULL_HANDLE) {
        // If so, wait on it!
        if ((result = device.waitForFences(imagesInFlight[backBufferIndex], false, UINT64_MAX)) != vk::Result::eSuccess) {
            throw Exception("GraphicsVK::acquireNextImage", "Failed to wait for fences with error: " + String::fromInt((int)result));
        }
        imagesInFlight[backBufferIndex] = vk::Fence(nullptr);
    }
    imagesInFlight[backBufferIndex] = inFlightFences[currentFrame];

    device.resetCommandPool(comPools[backBufferIndex], {});

    comBuffers[backBufferIndex].begin(vk::CommandBufferBeginInfo());
    vk::RenderPassBeginInfo beginInfo = vk::RenderPassBeginInfo(renderPass, framebuffers[backBufferIndex], scissor);
    comBuffers[backBufferIndex].beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
}

void GraphicsVK::clear(Color color) {
    vk::ClearAttachment cl = vk::ClearAttachment(vk::ImageAspectFlagBits::eColor, 0, vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{ { color.red, color.green, color.blue, color.alpha }})));
    vk::ClearRect rect = vk::ClearRect(scissor, 0, 1);
    comBuffers[backBufferIndex].clearAttachments(cl, rect);
}

// TODO: Remove if unneeded.
int GraphicsVK::validNextHighestMemoryRange(int input) {
    vk::DeviceSize sizeMultiplier = physicalDevice.getProperties().limits.nonCoherentAtomSize;
    return (int) (std::ceil((double)input / sizeMultiplier) * sizeMultiplier);
}

// TODO: Move?
int GraphicsVK::findMemoryType(int typeFilter, vk::MemoryPropertyFlags memPropFlags) {
    vk::PhysicalDeviceMemoryProperties memProps = physicalDevice.getMemoryProperties();
    for (int i = 0; i < (int)memProps.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & memPropFlags) == memPropFlags) {
            return i;
        }
    }
    throw Exception("findMemoryType", "Found no suitable memory type!");
}

// TODO: Optimize.
void GraphicsVK::transfer(const vk::Buffer& src, const vk::Buffer& dst, int size) {
    device.resetCommandPool(transferComPool, {});
    transferComBuffer.begin(vk::CommandBufferBeginInfo({}));
    transferComBuffer.copyBuffer(src, dst, vk::BufferCopy(0, 0, size));
    transferComBuffer.end();
    vk::SubmitInfo sui;
    sui.setCommandBuffers(transferComBuffer);
    transferQueue.submit(sui, nullptr);
    transferQueue.waitIdle();
}

void GraphicsVK::createSwapchain(bool vsync) {
    // Setting the size of the swap chain images.
    vk::SurfaceCapabilitiesKHR sc = physicalDevice.getSurfaceCapabilitiesKHR(vkSurface);
    // 0xFFFFFFFF indicates to just rely on the size of the window clamped by the given maxs and mins.
    if (sc.currentExtent.width != 0xFFFFFFFF) {
        swapchainExtent = sc.currentExtent;
    } else {
        // TODO: Move MathUtil to PGE and use it here.
        swapchainExtent = vk::Extent2D {
            (uint32_t)width < sc.minImageExtent.width ? sc.minImageExtent.width : (uint32_t)width > sc.maxImageExtent.width ? sc.maxImageExtent.width : (uint32_t)width,
            (uint32_t)height < sc.minImageExtent.height ? sc.minImageExtent.height : (uint32_t)height > sc.maxImageExtent.height ? sc.maxImageExtent.height : (uint32_t)height,
        };
    }

    // Setting the amount of images in the swap chain.
    uint32_t imageCount = sc.minImageCount + 1;
    if (sc.maxImageCount > 0 && imageCount > sc.maxImageCount) {
        imageCount = sc.maxImageCount;
    }

    // Creating the swap chain.
    vk::SwapchainCreateInfoKHR sci = vk::SwapchainCreateInfoKHR({}, vkSurface, imageCount, swapchainFormat.format, swapchainFormat.colorSpace, swapchainExtent, 1, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, 0, nullptr, sc.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, vsync ? vk::PresentModeKHR::eFifoRelaxed : vk::PresentModeKHR::eMailbox, VK_TRUE, nullptr);
    // TODO: Remove.
    if (graphicsQueueIndex != presentQueueIndex || presentQueueIndex != transferQueueIndex) {
        sci.setImageSharingMode(vk::SharingMode::eConcurrent);
        std::vector<uint32_t> queueIndices = std::vector<uint32_t>{ graphicsQueueIndex, presentQueueIndex, transferQueueIndex };
        sci.setQueueFamilyIndices(queueIndices);
    }
    swapchain = device.createSwapchainKHR(sci);

    // Creating image views for our swapchain images to ultimately write to.
    std::vector<vk::Image> swapchainImages = device.getSwapchainImagesKHR(swapchain);
    swapchainImageViews = std::vector<vk::ImageView>(swapchainImages.size());
    for (int i = 0; i < (int)swapchainImages.size(); i++) {
        vk::ImageViewCreateInfo ivci = vk::ImageViewCreateInfo({}, swapchainImages[i], vk::ImageViewType::e2D, swapchainFormat.format);
        ivci.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
        swapchainImageViews[i] = device.createImageView(ivci);
    }

    // Creating a viewport.
    viewport = vk::Viewport(0.f, (float)swapchainExtent.height, (float)swapchainExtent.width, -(float)swapchainExtent.height, 0.f, 1.f);
    scissor = vk::Rect2D(vk::Offset2D(0, 0), swapchainExtent);
    viewportInfo = vk::PipelineViewportStateCreateInfo({}, 1, &viewport, 1, &scissor);

    // Color blending.
    colorBlendAttachmentState = vk::PipelineColorBlendAttachmentState(false);
    colorBlendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG;
    colorBlendInfo = vk::PipelineColorBlendStateCreateInfo({}, false, vk::LogicOp::eClear, 1, &colorBlendAttachmentState);

    // Creating a render pass.
    vk::AttachmentDescription colorAttachment = vk::AttachmentDescription({}, swapchainFormat.format, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
    vk::AttachmentReference colorAttachmentRef = vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::SubpassDescription subpass = vk::SubpassDescription({}, vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &colorAttachmentRef);
    vk::SubpassDependency dependency = vk::SubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput, {}, vk::AccessFlagBits::eColorAttachmentWrite);
    vk::RenderPassCreateInfo renderPassInfo = vk::RenderPassCreateInfo({}, 1, &colorAttachment, 1, &subpass, 1, &dependency);
    renderPass = device.createRenderPass(renderPassInfo);

    rasterizationInfo = vk::PipelineRasterizationStateCreateInfo({}, false, false, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise, false, 0.f, 0.f, 0.f, 1.f);
    multisamplerInfo = vk::PipelineMultisampleStateCreateInfo({}, vk::SampleCountFlagBits::e1, false, 0.f, nullptr, false, false);

    inputAssemblyLines = vk::PipelineInputAssemblyStateCreateInfo({}, vk::PrimitiveTopology::eLineList, false);
    inputAssemblyTris = vk::PipelineInputAssemblyStateCreateInfo({}, vk::PrimitiveTopology::eTriangleList, false);

    framebuffers.resize(swapchainImageViews.size());
    for (int i = 0; i < (int)swapchainImageViews.size(); i++) {
        vk::FramebufferCreateInfo framebufferInfo = vk::FramebufferCreateInfo({}, renderPass, 1, &swapchainImageViews[i], swapchainExtent.width, swapchainExtent.height, 1);
        framebuffers[i] = device.createFramebuffer(framebufferInfo);
    }

    comPools.resize(framebuffers.size());
    comBuffers.resize(framebuffers.size());
    vk::Result result;
    for (int i = 0; i < (int)framebuffers.size(); i++) {
        vk::CommandPoolCreateInfo commandPoolInfo = vk::CommandPoolCreateInfo({}, graphicsQueueIndex);
        comPools[i] = device.createCommandPool(commandPoolInfo);
        vk::CommandBufferAllocateInfo comBufAllInfo = vk::CommandBufferAllocateInfo(comPools[i], vk::CommandBufferLevel::ePrimary, 1);
        if ((result = device.allocateCommandBuffers(&comBufAllInfo, &comBuffers[i])) != vk::Result::eSuccess) {
            throw Exception("GraphicsVK::createSwapchain", "Failed to allocate command buffers with error: " + String::fromInt((int)result));
        }
    }

    vk::CommandPoolCreateInfo transferComPoolInfo = vk::CommandPoolCreateInfo({}, transferQueueIndex);
    transferComPool = device.createCommandPool(transferComPoolInfo);
    // TODO: How many buffers should we have?
    vk::CommandBufferAllocateInfo transferComBufferInfo = vk::CommandBufferAllocateInfo(transferComPool, vk::CommandBufferLevel::ePrimary, 1);
    if ((result = device.allocateCommandBuffers(&transferComBufferInfo, &transferComBuffer)) != vk::Result::eSuccess) {
        throw Exception("GraphicsVK::createSwapchain", "Failed to allocate transfer command buffers with error: " + String::fromInt((int)result));
    }
}

void GraphicsVK::setRenderTarget(Texture* renderTarget) {

}

void GraphicsVK::setRenderTargets(std::vector<Texture*> renderTargets) {

}

void GraphicsVK::resetRenderTarget() {

}

void GraphicsVK::setViewport(Rectanglei vp) {

}

void GraphicsVK::setVsync(bool isEnabled) {
    if (isEnabled != vsync) {
        vsync = isEnabled;
        endRender();
        device.waitIdle();
        // TODO: Clean.
        // Don't when ending.
        createSwapchain(isEnabled);
        acquireNextImage();
    }
}

vk::Pipeline GraphicsVK::createPipeline(ShaderVK* shader, Primitive::TYPE primitive) const {
    const vk::PipelineInputAssemblyStateCreateInfo* inputInfo;
    switch (primitive) {
        case Primitive::TYPE::LINE: {
            inputInfo = &inputAssemblyLines;
        } break;
        default:
        case Primitive::TYPE::TRIANGLE: {
            inputInfo = &inputAssemblyTris;
        } break;
    }
    vk::GraphicsPipelineCreateInfo pipelineInfo = vk::GraphicsPipelineCreateInfo({}, 2, shader->getShaderStageInfo(), shader->getVertexInputInfo(), inputInfo, nullptr, &viewportInfo, &rasterizationInfo, &multisamplerInfo, nullptr, &colorBlendInfo, nullptr, *shader->getLayout(), renderPass, 0, {}, -1);
    return device.createGraphicsPipeline(nullptr, pipelineInfo).value;
}

vk::Device GraphicsVK::getDevice() const {
    return device;
}

vk::CommandBuffer GraphicsVK::getCurrentCommandBuffer() {
    return comBuffers[backBufferIndex];
}
