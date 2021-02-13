#include <Window/Window.h>
#include "WindowInternal.h"

#include <vulkan/vulkan.hpp>

namespace PGE {

class WindowVK : public WindowInternal {
    public:
        WindowVK(String c, int w, int h, bool fs);
        ~WindowVK();

        void update() override;
        void swap(bool vsyncEnabled=true) override;

    private:
        vk::Instance vkInstance;
        vk::Device vkDevice;
        vk::SurfaceKHR vkSurface;

        vk::Queue graphicsQueue;
        vk::Queue presentQueue;

        vk::SwapchainKHR swapchain;
        vk::Extent2D swapchainExtent;
        vk::SurfaceFormatKHR swapchainFormat;
        std::vector<vk::ImageView> swapchainImageViews;

        void cleanup() override;
        void throwException(String func, String details) override;
};

}
