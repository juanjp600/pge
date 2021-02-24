#ifndef PGEINTERNAL_GRAPHICS_VK_H_INCLUDED
#define PGEINTERNAL_GRAPHICS_VK_H_INCLUDED

#include <vulkan/vulkan.hpp>

#include "GraphicsInternal.h"

namespace PGE {

class GraphicsVK : public GraphicsInternal {
    public:
        class OpDeviceIdle : public OpContainer {
            public:
                OpDeviceIdle(const vk::Device& device);
                virtual void exec() override;

            private:
                vk::Device device;
        };

        GraphicsVK(String name, int w = 1280, int h = 720, bool fs = false);

        void swap() override;

        void clear(Color color) override;

        int validNextHighestMemoryRange(int input);
        int findMemoryType(int typeFilter, vk::MemoryPropertyFlags memPropFlags);

        void transfer(const vk::Buffer& src, const vk::Buffer& dst, int size);

        void setRenderTarget(Texture* renderTarget) override;
        void setRenderTargets(std::vector<Texture*> renderTargets) override;
        void resetRenderTarget() override;

        void setViewport(Rectanglei vp) override;

        vk::Device getDevice() const;
        vk::PipelineViewportStateCreateInfo* getViewportInfo();
        vk::PipelineColorBlendStateCreateInfo* getColorBlendInfo();
        vk::PipelineRasterizationStateCreateInfo* getRasterizationInfo();
        vk::PipelineMultisampleStateCreateInfo* getMultisamplerInfo();
        vk::RenderPass* getRenderPass();
        vk::CommandBuffer getCurrentCommandBuffer();

    private:
        vk::Instance vkInstance;
        vk::PhysicalDevice physicalDevice;
        vk::Device device;
        vk::SurfaceKHR vkSurface;

        vk::Queue graphicsQueue;
        vk::Queue presentQueue;
        vk::Queue transferQueue;

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

        std::vector<vk::CommandPool> comPools;
        std::vector<vk::CommandBuffer> comBuffers;
        vk::CommandPool transferComPool;
        vk::CommandBuffer transferComBuffer;

        std::vector<vk::Semaphore> imageAvailableSemaphores;
        std::vector<vk::Semaphore> renderFinishedSemaphores;
        std::vector<vk::Fence> inFlightFences;
        std::vector<vk::Fence> imagesInFlight;

        const int MAX_FRAMES_IN_FLIGHT;
        int currentFrame;

        uint32_t backBufferIndex;

        void acquireNextImage();
};

}

#endif // PGEINTERNAL_GRAPHICS_VK_H_INCLUDED
