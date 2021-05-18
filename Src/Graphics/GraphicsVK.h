#ifndef PGEINTERNAL_GRAPHICS_VK_H_INCLUDED
#define PGEINTERNAL_GRAPHICS_VK_H_INCLUDED

#include "GraphicsInternal.h"

#include <ResourceManagement/ResourceViewVector.h>
#include "../ResourceManagement/ResourceManagerVK.h"
#include "../ResourceManagement/VK.h"

namespace PGE {

class ShaderVK;

class GraphicsVK : public GraphicsInternal {
    public:
        GraphicsVK(const String& name, int w = 1280, int h = 720, bool fs = false);

        void swap() override;

        void clear(Color color) override;

        void transfer(const vk::Buffer& src, const vk::Buffer& dst, int size);

        void setRenderTarget(Texture* renderTarget) override;
        void setRenderTargets(const std::vector<Texture*>& renderTargets) override;
        void resetRenderTarget() override;

        void setViewport(const Rectanglei& vp) override;

        void setVsync(bool isEnabled) override;

        vk::Device getDevice() const;
        vk::PhysicalDevice getPhysicalDevice() const;
        vk::RenderPass getRenderPass() const;
        vk::CommandBuffer getCurrentCommandBuffer() const;
        const VKPipelineInfo* getPipelineInfo() const;

        PGE_GFX_OBJ_DEC

    private:
        // TODO: Remove.
        uint32_t graphicsQueueIndex;
        uint32_t presentQueueIndex;
        uint32_t transferQueueIndex;

        VKInstance::View instance;
        vk::PhysicalDevice physicalDevice;
        VKDevice::View device;
        VKSurface::View surface;

        vk::Queue graphicsQueue;
        vk::Queue presentQueue;
        vk::Queue transferQueue;

        // TODO: Wrap this?
        VKSwapchain::View swapchain;
        vk::Extent2D swapchainExtent;
        vk::SurfaceFormatKHR swapchainFormat;
        ResourceViewVector<vk::ImageView> swapchainImageViews;

        vk::Rect2D scissor;

        VKPipelineInfo pipelineInfo;

        VKRenderPass::View renderPass;

        ResourceViewVector<vk::Framebuffer> framebuffers;

        ResourceViewVector<vk::CommandPool> comPools;
        std::vector<vk::CommandBuffer> comBuffers;
        VKCommandPool::View transferComPool;
        vk::CommandBuffer transferComBuffer;

        ResourceViewVector<vk::Semaphore> imageAvailableSemaphores;
        ResourceViewVector<vk::Semaphore> renderFinishedSemaphores;
        ResourceViewVector<vk::Fence> inFlightFences;
        // We don't actually own any resource here.
        std::vector<vk::Fence> imagesInFlight;

        const int MAX_FRAMES_IN_FLIGHT = 3;
        int currentFrame = 0;

        int backBufferIndex;

        ResourceManagerVK resourceManager;

        void createSwapchain(bool vsync);

        void endRender();
        void acquireNextImage();
};

}

#endif // PGEINTERNAL_GRAPHICS_VK_H_INCLUDED
