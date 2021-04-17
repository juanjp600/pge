#ifndef PGEINTERNAL_GRAPHICS_VK_H_INCLUDED
#define PGEINTERNAL_GRAPHICS_VK_H_INCLUDED

#include "GraphicsInternal.h"

#include <ResourceManagement/ResourceReferenceVector.h>
#include "../ResourceManagement/ResourceManagerVK.h"
#include "../ResourceManagement/VK.h"

namespace PGE {

class ShaderVK;

class GraphicsVK : public GraphicsInternal {
    public:
        GraphicsVK(String name, int w = 1280, int h = 720, bool fs = false);

        void swap() override;

        void clear(Color color) override;

        void transfer(const vk::Buffer& src, const vk::Buffer& dst, int size);

        void setRenderTarget(Texture* renderTarget) override;
        void setRenderTargets(std::vector<Texture*> renderTargets) override;
        void resetRenderTarget() override;

        void setViewport(Rectanglei vp) override;

        void setVsync(bool isEnabled) override;

        vk::Device getDevice() const;
        vk::PhysicalDevice getPhysicalDevice() const;
        VKPipelineInfo& getPipelineInfo();
        vk::RenderPass getRenderPass() const;
        vk::CommandBuffer getCurrentCommandBuffer() const;

        __GFX_OBJ_DEC

    private:
        // TODO: Remove.
        uint32_t graphicsQueueIndex;
        uint32_t presentQueueIndex;
        uint32_t transferQueueIndex;

        VKInstance::Ref instance;
        vk::PhysicalDevice physicalDevice;
        VKDevice::Ref device;
        VKSurface::Ref surface;

        vk::Queue graphicsQueue;
        vk::Queue presentQueue;
        vk::Queue transferQueue;

        // TODO: Wrap this?
        VKSwapchain::Ref swapchain;
        vk::Extent2D swapchainExtent;
        vk::SurfaceFormatKHR swapchainFormat;
        ResourceReferenceVector<vk::ImageView> swapchainImageViews;

        vk::Rect2D scissor;

        VKPipelineInfo pipelineInfo;

        VKRenderPass::Ref renderPass;

        ResourceReferenceVector<vk::Framebuffer> framebuffers;

        ResourceReferenceVector<vk::CommandPool> comPools;
        std::vector<vk::CommandBuffer> comBuffers;
        VKCommandPool::Ref transferComPool;
        vk::CommandBuffer transferComBuffer;

        ResourceReferenceVector<vk::Semaphore> imageAvailableSemaphores;
        ResourceReferenceVector<vk::Semaphore> renderFinishedSemaphores;
        ResourceReferenceVector<vk::Fence> inFlightFences;
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
