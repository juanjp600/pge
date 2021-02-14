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

        void startRender();

        vk::Device getDevice() const;
        vk::PipelineViewportStateCreateInfo* getViewportInfo();
        vk::PipelineColorBlendStateCreateInfo* getColorBlendInfo();
        vk::PipelineRasterizationStateCreateInfo* getRasterizationInfo();
        vk::PipelineMultisampleStateCreateInfo* getMultisamplerInfo();
        vk::RenderPass* getRenderPass();
        std::vector<vk::CommandBuffer> getCommandBuffers();

    private:
        bool startedRender;

        vk::Instance vkInstance;
        vk::Device device;
        vk::SurfaceKHR vkSurface;

        vk::Queue graphicsQueue;
        vk::Queue presentQueue;

        vk::SwapchainKHR swapchain;
        vk::Extent2D swapchainExtent;
        vk::SurfaceFormatKHR swapchainFormat;
        std::vector<vk::ImageView> swapchainImageViews;

        vk::Viewport viewport;
        vk::Rect2D scissor;
        vk::PipelineViewportStateCreateInfo viewportInfo;

        vk::PipelineColorBlendAttachmentState colorBlendAttachmentState;
        vk::PipelineColorBlendStateCreateInfo colorBlendInfo;

        vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
        vk::PipelineMultisampleStateCreateInfo multisamplerInfo;

        vk::RenderPass renderPass;

        std::vector<vk::Framebuffer> framebuffers;

        vk::CommandPool comPool;
        std::vector<vk::CommandBuffer> comBuffers;

        vk::Semaphore imageAvailableSemaphore;
        vk::Semaphore renderFinishedSemaphore;

        uint32_t backBufferIndex;

        void cleanup() override;
        void throwException(String func, String details) override;
};

}
