#ifndef PGEINTERNAL_GRAPHICS_VK_H_INCLUDED
#define PGEINTERNAL_GRAPHICS_VK_H_INCLUDED

#include "GraphicsInternal.h"

#include "Shader/ShaderVK.h"
#include "Mesh/MeshVK.h"
#include "Texture/TextureVK.h"

#include "../ResourceManagement/ResourceManagerVK.h"

namespace PGE {

class ShaderVK;

class GraphicsVK : public GraphicsSpecialized<ShaderVK, MeshVK, TextureVK> {
    public:
        GraphicsVK(const String& name, int w, int h, WindowMode wm, int x, int y);

        void swap() override;

        void clear(const Color& color) override;

        void transfer(const vk::Buffer& src, const vk::Buffer& dst, int size);

        void setRenderTarget(Texture& renderTarget) override;
        void setRenderTargets(const ReferenceVector<Texture>& renderTargets) override;
        void resetRenderTarget() override;

        void setViewport(const Rectanglei& vp) override;

        void setCulling(Culling mode) override;

        void setVsync(bool isEnabled) override;

        vk::Device getDevice() const;
        vk::PhysicalDevice getPhysicalDevice() const;
        vk::RenderPass getRenderPass() const;
        vk::CommandBuffer getCurrentCommandBuffer() const;
        const VKPipelineInfo* getPipelineInfo() const;

        void addMesh(MeshVK& m);
        void removeMesh(MeshVK& m);

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
        std::vector<VKImageView::View> swapchainImageViews;

        vk::Rect2D scissor;

        VKPipelineInfo pipelineInfo;

        VKRenderPass::View renderPass;

        std::vector<VKFramebuffer::View> framebuffers;

        std::vector<VKCommandPool::View> comPools;
        std::vector<vk::CommandBuffer> comBuffers;
        VKCommandPool::View transferComPool;
        vk::CommandBuffer transferComBuffer;

        std::vector<VKSemaphore::View> imageAvailableSemaphores;
        std::vector<VKSemaphore::View> renderFinishedSemaphores;
        std::vector<VKFence::View> inFlightFences;
        // We don't actually own any resource here.
        std::vector<vk::Fence> imagesInFlight;

        std::unordered_set<MeshVK*> meshes;

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
