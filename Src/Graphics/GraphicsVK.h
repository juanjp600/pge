#ifndef PGEINTERNAL_GRAPHICS_VK_H_INCLUDED
#define PGEINTERNAL_GRAPHICS_VK_H_INCLUDED

#include "GraphicsInternal.h"

#include "Material/MaterialVK.h"
#include "Texture/TextureVK.h"
#include "Shader/ShaderVK.h"
#include "Mesh/MeshVK.h"

#include "../ResourceManagement/ResourceManagerVK.h"

namespace PGE {

class ShaderVK;

class GraphicsVK : public GraphicsSpecialized<ShaderVK, MeshVK, TextureVK, MaterialVK> {
    public:
        enum class ImageLayout {
            UNDEFINED,
            TRANSFER_SRC,
            TRANSFER_DST,
            SHADER_READ,
        };

        GraphicsVK(const String& name, int w, int h, WindowMode wm, int x, int y);

        void swap() override;

        void clear(const Color& color) override;


        void generateMipmaps(vk::Image img, int w, int h, int miplevels);

        template <ImageLayout OLD, ImageLayout NEW>
        void transformImage(vk::Image img, vk::Format fmt, int miplevels) {
            startTransfer();

            vk::ImageMemoryBarrier barrier = createBasicBarrier(img, miplevels);
            pipelineBarrier<OLD, NEW>(barrier);

            endTransfer();
        }

        // TODO: Optimize.
        void transfer(const vk::Buffer& src, const vk::Buffer& dst, int size) {
            startTransfer();
            transferComBuffer.copyBuffer(src, dst, vk::BufferCopy(0, 0, size));
            endTransfer();
        }
        
        void transferToImage(const vk::Buffer& src, const vk::Image& dst, int w, int h);

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
        const vk::Sampler& getSampler(bool rt) const;

        const vk::DescriptorSetLayout& getDescriptorSetLayout(int count);
        void dropDescriptorSetLayout(int count);

        void addMesh(MeshVK& m);
        void removeMesh(MeshVK& m);

        void trash(ResourceBase& res);

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

        VKSampler::View sampler;
        VKSampler::View samplerRT;

        // Maps number of textures to a layout.
        struct DescriptorSetLayoutEntry {
            VKDescriptorSetLayout::View layout;
            int count;
        };
        std::unordered_map<int, DescriptorSetLayoutEntry> dSetLayouts;

        static constexpr int MAX_FRAMES_IN_FLIGHT = 3;
        int currentFrame = 0;

        int backBufferIndex;

        ResourceManagerVK resourceManager;
        std::vector<ResourceBase*> trashBin;

        void createSwapchain(bool vsync);

        // TODO: Refactor this out of Graphics?
        // TODO: consteval C++20.
        static constexpr vk::ImageMemoryBarrier createBasicBarrier(vk::Image img, int miplevels) {
            vk::ImageMemoryBarrier barrier;
            barrier.image = img;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            barrier.subresourceRange.layerCount = 1;
            barrier.subresourceRange.levelCount = miplevels;
            return barrier;
        }

        static constexpr vk::ImageSubresourceLayers createBasicImgSubresLayers(int miplevel) {
            vk::ImageSubresourceLayers layers;
            layers.aspectMask = vk::ImageAspectFlagBits::eColor;
            layers.mipLevel = miplevel;
            layers.layerCount = 1;
            return layers;
        }

        struct TransitionInfo {
            vk::ImageLayout layout;
            vk::AccessFlags accessFlags;
            vk::PipelineStageFlags stageFlags;
        };

        template <ImageLayout LAYOUT>
        static constexpr TransitionInfo infoFromLayout() {
            TransitionInfo info;
            switch (LAYOUT) {
                case ImageLayout::UNDEFINED: {
                    info.layout = vk::ImageLayout::eUndefined;
                    info.accessFlags = vk::AccessFlagBits::eNoneKHR;
                    info.stageFlags = vk::PipelineStageFlagBits::eTopOfPipe;
                } break;
                case ImageLayout::TRANSFER_DST: {
                    info.layout = vk::ImageLayout::eTransferDstOptimal;
                    info.accessFlags = vk::AccessFlagBits::eTransferWrite;
                    info.stageFlags = vk::PipelineStageFlagBits::eTransfer;
                } break;
                case ImageLayout::TRANSFER_SRC: {
                    info.layout = vk::ImageLayout::eTransferSrcOptimal;
                    info.accessFlags = vk::AccessFlagBits::eTransferRead;
                    info.stageFlags = vk::PipelineStageFlagBits::eTransfer;
                } break;
                case ImageLayout::SHADER_READ: {
                    info.layout = vk::ImageLayout::eShaderReadOnlyOptimal;
                    info.accessFlags = vk::AccessFlagBits::eShaderRead;
                    info.stageFlags = vk::PipelineStageFlagBits::eFragmentShader;
                } break;
            }
            return info;
        }

        template <ImageLayout SRC, ImageLayout DST>
        void pipelineBarrier(vk::ImageMemoryBarrier& barrier) {
            TransitionInfo srcInfo = infoFromLayout<SRC>();
            TransitionInfo dstInfo = infoFromLayout<DST>();

            barrier.oldLayout = srcInfo.layout;
            barrier.newLayout = dstInfo.layout;
            barrier.srcAccessMask = srcInfo.accessFlags;
            barrier.dstAccessMask = dstInfo.accessFlags;

            transferComBuffer.pipelineBarrier(
                srcInfo.stageFlags, dstInfo.stageFlags,
                (vk::DependencyFlags)0, VK_NULL_HANDLE, VK_NULL_HANDLE, barrier
            );
        }

        void endRender();
        void acquireNextImage();

        void startTransfer();
        void endTransfer();
};

}

#endif // PGEINTERNAL_GRAPHICS_VK_H_INCLUDED
