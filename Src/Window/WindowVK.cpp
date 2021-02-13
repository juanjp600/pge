#include "WindowVK.h"

#include <SDL_vulkan.h>

#include <Exception/Exception.h>

#include "../SysEvents/SysEventsInternal.h"

using namespace PGE;

#include <iostream>

WindowVK::WindowVK(String c, int w, int h, bool fs) {
    caption = c;
    width = w;
    height = h;
    fullscreen = fs;

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
    
    vk::ApplicationInfo vkAppInfo = vk::ApplicationInfo(c.cstr(), VK_MAKE_VERSION(0, 0, 0), "Pulse-Gun Engine", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);
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
    vk::PhysicalDevice selectedDevice;
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
            selectedDevice = pd;
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
    vkDevice = selectedDevice.createDevice(vk::DeviceCreateInfo({}, infos, layers, deviceExtensions));

    graphicsQueue = vkDevice.getQueue(graphicsQueueIndex, 0);
    presentQueue = vkDevice.getQueue(presentQueueIndex, 0);

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
    vk::SurfaceCapabilitiesKHR sc = selectedDevice.getSurfaceCapabilitiesKHR(vkSurface);
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
    vk::SwapchainCreateInfoKHR sci = vk::SwapchainCreateInfoKHR({}, vkSurface, imageCount, swapchainFormat.format, swapchainFormat.colorSpace, swapchainExtent, 1, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, 0, nullptr, sc.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, vk::PresentModeKHR::eFifo, VK_TRUE, nullptr);
    if (graphicsQueueIndex != presentQueueIndex) {
        sci.setImageSharingMode(vk::SharingMode::eConcurrent);
        sci.setQueueFamilyIndices(std::vector<uint32_t> { graphicsQueueIndex, presentQueueIndex });
    }
    swapchain = vkDevice.createSwapchainKHR(sci);

    // Creating image views for our swapchain images to ultimately write to.
    std::vector<vk::Image> swapchainImages = vkDevice.getSwapchainImagesKHR(swapchain);
    std::vector<vk::ImageView> swapchainImageViews = std::vector<vk::ImageView>(swapchainImages.size());
    for (vk::Image image : swapchainImages) {
        vk::ImageViewCreateInfo ivci = vk::ImageViewCreateInfo({}, image, vk::ImageViewType::e2D, swapchainFormat.format);
        ivci.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
        swapchainImageViews.push_back(vkDevice.createImageView(ivci));
    }

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
    
}

void WindowVK::cleanup() {
    SysEventsInternal::unsubscribe(eventSubscriber);
    for (vk::ImageView iw : swapchainImageViews) {
        vkDevice.destroyImageView(iw);
    }
    vkDevice.destroySwapchainKHR(swapchain);
    vkDevice.destroy();
    vkInstance.destroySurfaceKHR(vkSurface);
    vkInstance.destroy();
    if (sdlWindow != nullptr) { SDL_DestroyWindow(sdlWindow); }
}

void WindowVK::throwException(String func, String details) {
    cleanup();
    throw Exception("WindowVK::" + func, details);
}
