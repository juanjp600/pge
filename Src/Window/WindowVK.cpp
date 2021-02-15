#include "WindowVK.h"

#include <SDL_vulkan.h>

#include <Exception/Exception.h>

#include "../SysEvents/SysEventsInternal.h"

using namespace PGE;

WindowVK::WindowVK(String c, int w, int h, bool fs) : MAX_FRAMES_IN_FLIGHT(3) {
    caption = c;
    width = w;
    height = h;
    fullscreen = fs;

    currentFrame = 0;

    // SDL window creation.
    sdlWindow = SDL_CreateWindow(c.cstr(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (sdlWindow == nullptr) {
        throwException("WindowVK", "Failed to create SDL window: " + String(SDL_GetError()));
    }

    eventSubscriber = new SysEventsInternal::SubscriberInternal(this, SysEventsInternal::SubscriberInternal::EventType::WINDOW);
    SysEventsInternal::subscribe(eventSubscriber);
    
    // Layers.
    std::vector<const char*> layers;
#ifdef DEBUG
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    // Extensions (currently only SDL required ones).
    unsigned int extensionCount = 0;
    SDL_Vulkan_GetInstanceExtensions(sdlWindow, &extensionCount, nullptr);
    const char** extensionData = (const char**) malloc(extensionCount * sizeof(const char*));
    SDL_Vulkan_GetInstanceExtensions(sdlWindow, &extensionCount, extensionData);
    std::vector<const char*> extensions = std::vector<const char*>(extensionData, extensionData + extensionCount);
    free(extensionData);
    
    vk::ApplicationInfo vkAppInfo = vk::ApplicationInfo(c.cstr(), VK_MAKE_VERSION(0, 0, 0), "Pulse-Gun Engine", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_1);
    vkInstance = vk::createInstance(vk::InstanceCreateInfo({}, &vkAppInfo, layers, extensions));

    // Creating the window's surface via SDL.
    VkSurfaceKHR tmpSurface;
    if (!SDL_Vulkan_CreateSurface(sdlWindow, (VkInstance)vkInstance, &tmpSurface)) {
        throwException("WindowVK", "Failed to create Vulkan surface: " + String(SDL_GetError()));
    }
    vkSurface = vk::SurfaceKHR(tmpSurface);

    SysEventsInternal::update();

    // The device extensions we need.
    std::array deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    // Declaring the indices for needed queues.
    uint32_t graphicsQueueIndex;
    uint32_t presentQueueIndex;

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
            extensionCheckSet.erase((const char*) ep.extensionName);
            if (extensionCheckSet.empty()) {
                break;
            }
        }
        if (!extensionCheckSet.empty()) { break; }

        // Checking and setting the queue indices.
        uint32_t i = 0;
        bool foundGraphics = false;
        bool foundPresent = false;
        for (const auto& qfp : pd.getQueueFamilyProperties()) {
            if (!foundGraphics && qfp.queueFlags & vk::QueueFlagBits::eGraphics) {
                graphicsQueueIndex = i;
                foundGraphics = true;
            }
            if (!foundPresent && pd.getSurfaceSupportKHR(i, vkSurface)) {
                presentQueueIndex = i;
                foundPresent = true;
            }
            if (foundGraphics && foundPresent) {
                break;
            }
            i++;
        }
        // Something we need is not supported on this GPU, ignore it.
        if (!foundGraphics || !foundPresent) { break; }

        // Calculate complete VRAM size.
        vk::DeviceSize pdSize = 0;
        vk::PhysicalDeviceMemoryProperties pdmp = pd.getMemoryProperties();
        for (int i = 0; i < pdmp.memoryHeapCount; i++) {
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
        throwException("WindowVK", "No Vulkan compatible GPU found!");
    }

    // Creating the logical device.
    float queuePriority = 1.f;
    std::set<uint32_t> queueIndices = { graphicsQueueIndex, presentQueueIndex };
    std::vector<vk::DeviceQueueCreateInfo> infos;
    for (int index : queueIndices) {
        infos.push_back(vk::DeviceQueueCreateInfo({}, index, 1, &queuePriority));
    }
    device = physicalDevice.createDevice(vk::DeviceCreateInfo({}, infos, layers, deviceExtensions));

    graphicsQueue = device.getQueue(graphicsQueueIndex, 0);
    presentQueue = device.getQueue(presentQueueIndex, 0);

    // Selecting the right surface format.
    // If we don't find the one we want we'll settle for the first available one.
    swapchainFormat = selectedPdFormats[0];
    for (const auto& f : selectedPdFormats) {
        if (f.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear && f.format == vk::Format::eB8G8R8A8Srgb) {
            swapchainFormat = f;
            break;
        }
    }

    // TODO: Selecting the present mode.
    // Remember: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkPresentModeKHR.html

    // Setting the size of the swap chain images.
    vk::SurfaceCapabilitiesKHR sc = physicalDevice.getSurfaceCapabilitiesKHR(vkSurface);
    // 0xFFFFFFFF indicates to just rely on the size of the window clamped by the given maxs and mins.
    if (sc.currentExtent.width != 0xFFFFFFFF) {
        swapchainExtent = sc.currentExtent;
    } else {
        // TODO: Move MathUtil to PGE and use it here.
        swapchainExtent = {
            w < sc.minImageExtent.width ? sc.minImageExtent.width : w > sc.maxImageExtent.width ? sc.maxImageExtent.width : w,
            h < sc.minImageExtent.height ? sc.minImageExtent.height : h > sc.maxImageExtent.height ? sc.maxImageExtent.height : h,
        };
    }

    // Setting the amount of images in the swap chain.
    uint32_t imageCount = sc.minImageCount + 1;
    if (sc.maxImageCount > 0 && imageCount > sc.maxImageCount) {
        imageCount = sc.maxImageCount;
    }

    // Creating the swap chain.
    vk::SwapchainCreateInfoKHR sci = vk::SwapchainCreateInfoKHR({}, vkSurface, imageCount, swapchainFormat.format, swapchainFormat.colorSpace, swapchainExtent, 1, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, 0, nullptr, sc.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, vk::PresentModeKHR::eImmediate, VK_TRUE, nullptr);
    if (graphicsQueueIndex != presentQueueIndex) {
        sci.setImageSharingMode(vk::SharingMode::eConcurrent);
        sci.setQueueFamilyIndices(std::vector<uint32_t> { graphicsQueueIndex, presentQueueIndex });
    }
    swapchain = device.createSwapchainKHR(sci);

    // Creating image views for our swapchain images to ultimately write to.
    std::vector<vk::Image> swapchainImages = device.getSwapchainImagesKHR(swapchain);
    swapchainImageViews = std::vector<vk::ImageView>(swapchainImages.size());
    for (int i = 0; i < swapchainImages.size(); i++) {
        vk::ImageViewCreateInfo ivci = vk::ImageViewCreateInfo({}, swapchainImages[i], vk::ImageViewType::e2D, swapchainFormat.format);
        ivci.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
        swapchainImageViews[i] = device.createImageView(ivci);
    }

    // Creating a viewport.
    viewport = vk::Viewport(0, swapchainExtent.height, swapchainExtent.width, -(float)swapchainExtent.height, 0.f, 1.f);
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

    rasterizationInfo = vk::PipelineRasterizationStateCreateInfo({}, false, false, vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise, false, 0.f, 0.f, 0.f, 1.f);
    multisamplerInfo = vk::PipelineMultisampleStateCreateInfo({}, vk::SampleCountFlagBits::e1, false, 0.f, nullptr, false, false);

    framebuffers = std::vector<vk::Framebuffer>(swapchainImageViews.size());
    for (int i = 0; i < swapchainImageViews.size(); i++) {
        vk::FramebufferCreateInfo framebufferInfo = vk::FramebufferCreateInfo({}, renderPass, 1, &swapchainImageViews[i], swapchainExtent.width, swapchainExtent.height, 1);
        framebuffers[i] = device.createFramebuffer(framebufferInfo);
    }

    comPools = std::vector<vk::CommandPool>(framebuffers.size());
    comBuffers = std::vector<vk::CommandBuffer>(framebuffers.size());
    for (int i = 0; i < framebuffers.size(); i++) {
        vk::CommandPoolCreateInfo commandPoolInfo = vk::CommandPoolCreateInfo({}, graphicsQueueIndex);
        comPools[i] = device.createCommandPool(commandPoolInfo);
        vk::CommandBufferAllocateInfo comBufAllInfo = vk::CommandBufferAllocateInfo(comPools[i], vk::CommandBufferLevel::ePrimary, 1);
        device.allocateCommandBuffers(&comBufAllInfo, &comBuffers[i]);
    }

    imageAvailableSemaphores = std::vector<vk::Semaphore>(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores = std::vector<vk::Semaphore>(MAX_FRAMES_IN_FLIGHT);
    inFlightFences = std::vector<vk::Fence>(MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        imageAvailableSemaphores[i] = device.createSemaphore(vk::SemaphoreCreateInfo({}));
        renderFinishedSemaphores[i] = device.createSemaphore(vk::SemaphoreCreateInfo({}));
        inFlightFences[i] = device.createFence(vk::FenceCreateInfo({ i != 0 ? vk::FenceCreateFlagBits::eSignaled : (vk::FenceCreateFlags)0 }));
    }
    imagesInFlight = std::vector<vk::Fence>(swapchainImages.size(), vk::Fence(nullptr));
    acquireNextImage();

    open = true;
    focused = true;
}

WindowVK::~WindowVK() {
    cleanup();
}

void WindowVK::update() {
    SDL_Event event;
    while (((SysEventsInternal::SubscriberInternal*)eventSubscriber)->popEvent(event)) {
        if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
            open = false;
        } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
            focused = true;
        } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
            focused = false;
        }
    }
}

void WindowVK::swap(bool vsyncEnabled) {
    comBuffers[backBufferIndex].endRenderPass();
    comBuffers[backBufferIndex].end();

    vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    device.resetFences(inFlightFences[currentFrame]);
    vk::SubmitInfo submitInfo = vk::SubmitInfo(1, &imageAvailableSemaphores[currentFrame], &waitStages, 1, &comBuffers[backBufferIndex], 1, &renderFinishedSemaphores[currentFrame]);
    graphicsQueue.submit(1, &submitInfo, inFlightFences[currentFrame]);

    vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR(1, &renderFinishedSemaphores[currentFrame], 1, &swapchain, &backBufferIndex, nullptr);
    presentQueue.presentKHR(presentInfo);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    // Wait until the current frames in flight are less than the max.
    device.waitForFences(inFlightFences[currentFrame], false, UINT64_MAX);

    acquireNextImage();
}

void WindowVK::acquireNextImage() {
    backBufferIndex = device.acquireNextImageKHR(swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], nullptr).value;

    // Is the image we just acquired currently still being submitted?
    if (imagesInFlight[backBufferIndex] != VK_NULL_HANDLE) {
        // If so, wait on it!
        device.waitForFences(imagesInFlight[backBufferIndex], false, UINT64_MAX);
        imagesInFlight[backBufferIndex] = vk::Fence(nullptr);
    }
    imagesInFlight[backBufferIndex] = inFlightFences[currentFrame];

    device.resetCommandPool(comPools[backBufferIndex], {});

    comBuffers[backBufferIndex].begin(vk::CommandBufferBeginInfo());
    vk::RenderPassBeginInfo beginInfo = vk::RenderPassBeginInfo(renderPass, framebuffers[backBufferIndex], scissor);
    comBuffers[backBufferIndex].beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
}

void WindowVK::clear(Color color) {
    vk::ClearAttachment cl = vk::ClearAttachment(vk::ImageAspectFlagBits::eColor, 0, vk::ClearValue(vk::ClearColorValue(std::array<float, 4>{ { color.red, color.green, color.blue, color.alpha }})));
    vk::ClearRect rect = vk::ClearRect(scissor, 0, 1);
    comBuffers[backBufferIndex].clearAttachments(cl, rect);
}

void WindowVK::cleanup() {
    SysEventsInternal::unsubscribe(eventSubscriber);
    device.waitIdle();
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        device.destroySemaphore(imageAvailableSemaphores[i]);
        device.destroySemaphore(renderFinishedSemaphores[i]);
        device.destroyFence(inFlightFences[i]);
    }
    for (int i = 0; i < framebuffers.size(); i++) {
        device.destroyCommandPool(comPools[i]);
        device.destroyImageView(swapchainImageViews[i]);
        device.destroyFramebuffer(framebuffers[i]);
    }
    device.destroySwapchainKHR(swapchain);
    device.destroyRenderPass(renderPass);
    device.destroy();
    vkInstance.destroySurfaceKHR(vkSurface);
    vkInstance.destroy();
    if (sdlWindow != nullptr) { SDL_DestroyWindow(sdlWindow); }
}

void WindowVK::throwException(String func, String details) {
    cleanup();
    throw Exception("WindowVK::" + func, details);
}

int WindowVK::findMemoryType(int typeFilter, vk::MemoryPropertyFlags memPropFlags) {
    vk::PhysicalDeviceMemoryProperties memProps = physicalDevice.getMemoryProperties();
    int safeIndex = -1;
    for (int i = 0; i < memProps.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & memPropFlags) == memPropFlags) {
            // We prefer device memory.
            if (memProps.memoryHeaps[memProps.memoryTypes[i].heapIndex].flags & vk::MemoryHeapFlagBits::eDeviceLocal) {
                return i;
            } else {
                safeIndex = i;
            }
        }
    }
    if (safeIndex != -1) {
        return safeIndex;
    }
    throwException("findMemoryType", "Found no suitable memory type!");
}

vk::Device WindowVK::getDevice() const {
    return device;
}

vk::PipelineViewportStateCreateInfo* WindowVK::getViewportInfo() {
    return &viewportInfo;
}

vk::PipelineColorBlendStateCreateInfo* WindowVK::getColorBlendInfo() {
    return &colorBlendInfo;
}

vk::PipelineRasterizationStateCreateInfo* WindowVK::getRasterizationInfo() {
    return &rasterizationInfo;
}

vk::PipelineMultisampleStateCreateInfo* WindowVK::getMultisamplerInfo() {
    return &multisamplerInfo;
}

vk::RenderPass* WindowVK::getRenderPass() {
    return &renderPass;
}

vk::CommandBuffer WindowVK::getCurrentCommandBuffer() {
    return comBuffers[backBufferIndex];
}
