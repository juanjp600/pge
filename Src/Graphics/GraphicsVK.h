#ifndef PGEINTERNAL_GRAPHICS_VK_H_INCLUDED
#define PGEINTERNAL_GRAPHICS_VK_H_INCLUDED

#include "GraphicsInternal.h"

#include <set>

#include "Material/MaterialVK.h"
#include "Texture/RenderTextureVK.h"
#include "Texture/TextureVK.h"
#include "Shader/ShaderVK.h"
#include "Mesh/MeshVK.h"

#include "../ResourceManagement/ResourceManagerVK.h"
#include <PGE/ResourceManagement/RawWrapper.h>

namespace PGE {

class ShaderVK;

class GraphicsVK : public GraphicsSpecialized<ShaderVK, MeshVK, TextureVK, MaterialVK, RenderTextureVK> {
    public:
        enum class ImageLayout {
            UNDEFINED,
            TRANSFER_SRC,
            TRANSFER_DST,
            SHADER_READ,
            RENDER_TARGET,
        };

        using MultiRTID = u64;

        GraphicsVK(const String& name, int w, int h, WindowMode wm, int x, int y);

        void swap() override;

        void clear(const Color& color) override;

        void setRenderTarget(Texture& rt) override;
        void setRenderTargets(const ReferenceVector<Texture>& rt) override;
        void resetRenderTarget() override;

        void setViewport(const Rectanglei& vp) override;

        void setCulling(Culling mode) override;

        void setDepthTest(bool isEnabled) override;

        void setVsync(bool isEnabled) override;


        void generateMipmaps(vk::Image img, int w, int h, int miplevels);

        template <ImageLayout OLD, ImageLayout NEW>
        void transformImage(vk::Image img, int miplevels) {
            startTransfer();

            vk::ImageMemoryBarrier barrier = UtilVK::createBasicBarrier(img, miplevels);
            pipelineBarrier<OLD, NEW>(barrier);

            endTransfer();
        }

        // TODO: Optimize.
        void transfer(const vk::Buffer& src, const vk::Buffer& dst, int size) {
            startTransfer();
            transferComBuffer.copyBuffer(src, dst, vk::BufferCopy(0, 0, size));
            endTransfer();
        }

        void transferToImage(const vk::Buffer& src, const vk::Image& dst, int w, int h, int miplevel = 0);

        void trash(ResourceBase& res);

        void destroyMultiRTResources(MultiRTID id);

        const vk::DescriptorSetLayout& getDescriptorSetLayout(int count);
        void dropDescriptorSetLayout(int count);

        vk::RenderPass getRenderPass(vk::Format fmt);
        vk::RenderPass requestRenderPass(vk::Format fmt);
        void returnRenderPass(vk::Format fmt);

        // See .cpp for documentation.
        VKMemoryBuffer& getTempStagingBuffer(int size);
        VKMemoryBuffer& registerStagingBuffer(int size);
        void unregisterStagingBuffer(int size);

        const RenderInfo* getRenderInfo() const;

        u64 getFrameCounter() const;

        vk::DeviceSize getAtomSize() const;

        vk::Device getDevice() const;
        vk::PhysicalDevice getPhysicalDevice() const;
        vk::RenderPass getBasicRenderPass() const;
        vk::CommandBuffer getCurrentCommandBuffer() const;
        const VKPipelineInfo& getPipelineInfo() const;
        const vk::Sampler& getSampler(bool rt) const;

        // Shaders register and unregister themself here.
        void addShader(ShaderVK& sh);
        void removeShader(ShaderVK& sh);

    private:
        ResourceManagerVK resourceManager;
        TrashBinVK trashBin;

        vk::DispatchLoaderDynamic dispatch;

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

        RawWrapper<TextureVK>::View depthBuffer;

        vk::Rect2D scissor;
        vk::Rect2D frameScissor;

        VKPipelineInfo pipelineInfo;

        VKRenderPass::View renderPass;

        std::vector<VKFramebuffer::View> framebuffers;

        std::vector<VKCommandPool::View> comPools;
        std::vector<vk::CommandBuffer> comBuffers;
        VKCommandPool::View transferComPool;
        vk::CommandBuffer transferComBuffer;

        std::vector<VKSemaphore::View> imageAvailableSemaphores;
        int acquiredIndex = 0;

        std::vector<VKSemaphore::View> renderFinishedSemaphores;
        std::vector<VKFence::View> inFlightFences;
        // We don't actually own any resource here.
        std::vector<vk::Fence> imagesInFlight;
        std::vector<vk::Fence> fenceSentWithComBuffer;

        std::unordered_set<ShaderVK*> shaders;

        VKSampler::View sampler;
        VKSampler::View samplerRT;

        // Maps number of textures to a layout.
        struct DescriptorSetLayoutEntry {
            VKDescriptorSetLayout* layout;
            int count;
        };
        std::unordered_map<int, DescriptorSetLayoutEntry> dSetLayouts;

        u64 frameCounter = 0;

        int currentFrame = 0;

        // There exist exactly as many command buffers as swapchain images and the two are usually in sync.
        // Rendering to a render target possibly requires advancing the command buffer index,
        // while not submitting anything to the swapchain.
        // This desync is handled by setting the resetting the backBufferIndex accordingly
        // when changing the rendering to/from a render target.
        int backBufferIndex;
        int oldSwapchainBackBufferIndex;

        vk::DeviceSize atomSize;

        struct FormatRenderPass {
            int count = 0;
            VKRenderPass* pass;
        };
        std::unordered_map<vk::Format, FormatRenderPass> renderPasses;

        const RenderInfo* rtRenderInfoOverride = nullptr;
        vk::Rect2D rtScissor;
        struct MultiRTResources {
            MultiRTID id;
            RenderInfo info;
            VKRenderPass* pass;
            VKFramebuffer* buffer;
        };
        std::unordered_map<MultiRTID, MultiRTResources> multiRenderTargetInfos;
        int rtCount = 1;

        std::vector<vk::ClearAttachment> clearAttachments;

        std::multiset<int> cachedBufferSizesSet;
        int cachedBufferSize = 0;
        RawWrapper<VKMemoryBuffer>::View cachedBuffer;
        void checkCachedBufferShrink();
        void updateCachedBuffer(int size);

        void createSwapchain();

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
                case ImageLayout::RENDER_TARGET: {
                    info.layout = vk::ImageLayout::eColorAttachmentOptimal;
                    info.accessFlags = vk::AccessFlagBits::eColorAttachmentWrite;
                    info.stageFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
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

        template <bool PRESENT> void submit();
        void acquireNextImage();
        void startRender();

        void startTransfer();
        void endTransfer();

        void reuploadPipelines();
};

}

#endif // PGEINTERNAL_GRAPHICS_VK_H_INCLUDED
