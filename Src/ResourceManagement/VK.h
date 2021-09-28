#ifndef PGEINTERNAL_RESOURCEMANAGEMENT_VK
#define PGEINTERNAL_RESOURCEMANAGEMENT_VK

#include <PGE/ResourceManagement/ResourceManager.h>

#include <unordered_set>

#include <vulkan/vulkan.hpp>
#include <SDL_vulkan.h>

#include <PGE/Graphics/Mesh.h>
#include <PGE/Graphics/Texture.h>

namespace PGE {

template <class T>
class VKResource : public Resource<T> {
    public:
        vk::Device device;

    public:
        VKResource(vk::Device dev) {
            device = dev;
        }
};

template <class T>
class VKDestroyResource : public VKResource<T> {
    public:
        using VKResource<T>::VKResource;

        ~VKDestroyResource() {
            this->device.destroy(this->resource);
        }
};

template <class T>
class VKFreeResource : public VKResource<T> {
    public:
        using VKResource<T>::VKResource;

        ~VKFreeResource() {
            this->device.free(this->resource);
        }
};

class VKInstance : public Resource<vk::Instance> {
    public:
        VKInstance(SDL_Window* window, const String& name, const std::vector<const char*>& layers) {
            // Extensions (currently only SDL required ones).
            unsigned int extensionCount = 0;
            SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);
            std::vector<const char*> extensions = std::vector<const char*>(extensionCount);
            SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions.data());

            vk::ApplicationInfo appInfo;
            appInfo.pApplicationName = name.cstr();
            appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
            appInfo.pEngineName = "pulsegun engine";
            appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.apiVersion = VK_API_VERSION_1_1;

            vk::InstanceCreateInfo info;
            info.pApplicationInfo = &appInfo;
            info.setPEnabledLayerNames(layers);
            info.setPEnabledExtensionNames(extensions);

#ifdef DEBUG
            std::array enables = {
                vk::ValidationFeatureEnableEXT::eBestPractices,
                vk::ValidationFeatureEnableEXT::eSynchronizationValidation,
            };
            vk::ValidationFeaturesEXT features; features.setEnabledValidationFeatures(enables);
            info.pNext = &features;
#endif

            resource = vk::createInstance(info);
        }

        ~VKInstance() {
            resource.destroy();
        }
};

class VKDevice : public Resource<vk::Device> {
    private:
        static constexpr std::array priority = { 1.f };

    public:
        VKDevice(vk::PhysicalDevice physDev, const std::unordered_set<uint32_t>& queueIndices,
            const std::vector<const char*>& layers, const std::vector<const char*>& deviceExtensions) {
            vk::PhysicalDeviceFeatures features;
            features.samplerAnisotropy = VK_TRUE;

            std::vector<vk::DeviceQueueCreateInfo> infos; infos.reserve(3);
            for (uint32_t index : queueIndices) {
                infos.emplace_back((vk::DeviceQueueCreateFlags)0, index, priority);
            }

            vk::DeviceCreateInfo info;
            info.pEnabledFeatures = &features;
            info.setQueueCreateInfos(infos);
            info.setPEnabledLayerNames(layers);
            info.setPEnabledExtensionNames(deviceExtensions);

            resource = physDev.createDevice(info);
        }

        ~VKDevice() {
            resource.destroy();
        }
};

class VKSurface : public Resource<vk::SurfaceKHR> {
    private:
        vk::Instance instance;

    public:
        VKSurface(vk::Instance inst, SDL_Window* window) {
            instance = inst;

            // TODO: Change SDL to be more epic.
            PGE_ASSERT(SDL_Vulkan_CreateSurface(window, (VkInstance)instance, (VkSurfaceKHR*)&resource) == SDL_TRUE, "Failed to create Vulkan surface (SDLERROR: " + String(SDL_GetError()) + ")");
        }

        ~VKSurface() {
            instance.destroySurfaceKHR(resource);
        }
};

class VKSemaphore : public VKDestroyResource<vk::Semaphore> {
    public:
        VKSemaphore(vk::Device dev) : VKDestroyResource(dev) {
            resource = dev.createSemaphore({ });
        }
};

class VKFence : public VKDestroyResource<vk::Fence> {
    public:
        VKFence(vk::Device dev, bool signaled) : VKDestroyResource(dev) {
            vk::FenceCreateInfo info;
            if (signaled) { info.flags = vk::FenceCreateFlagBits::eSignaled; }

            resource = dev.createFence(info);
        }
};

class VKCommandPool : public VKDestroyResource<vk::CommandPool> {
    public:
        VKCommandPool(vk::Device dev, int queueIndex) : VKDestroyResource(dev) {
            vk::CommandPoolCreateInfo info;
            info.flags = vk::CommandPoolCreateFlagBits::eTransient;
            info.queueFamilyIndex = queueIndex;

            resource = dev.createCommandPool(info);
        }
};

class VKImageView : public VKDestroyResource<vk::ImageView> {
    public:
        VKImageView(vk::Device dev, vk::Image swapchainImage, vk::Format fmt)
            : VKDestroyResource(dev) {
            vk::ImageSubresourceRange subRange;
            subRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            subRange.layerCount = 1;
            subRange.levelCount = 1;

            vk::ImageViewCreateInfo info;
            info.subresourceRange = subRange;
            info.image = swapchainImage;
            info.viewType = vk::ImageViewType::e2D;
            info.format = fmt;

            resource = dev.createImageView(info);
        }
};

class VKRenderPass : public VKDestroyResource<vk::RenderPass> {
    public:
        VKRenderPass(vk::Device dev, vk::SurfaceFormatKHR swapchainFormat)
            : VKDestroyResource(dev) {
            vk::AttachmentDescription color;
            color.format = swapchainFormat.format;
            color.samples = vk::SampleCountFlagBits::e1;
            color.loadOp = vk::AttachmentLoadOp::eDontCare;
            color.storeOp = vk::AttachmentStoreOp::eStore;
            color.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
            color.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            color.initialLayout = vk::ImageLayout::eUndefined;
            color.finalLayout = vk::ImageLayout::ePresentSrcKHR;

            vk::AttachmentReference colorRef;
            colorRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

            vk::SubpassDescription subpass;
            subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
            subpass.setColorAttachments(colorRef);

            vk::SubpassDependency dependency;
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

            vk::RenderPassCreateInfo info;
            info.setAttachments(color);
            info.setSubpasses(subpass);
            info.setDependencies(dependency);

            resource = dev.createRenderPass(vk::RenderPassCreateInfo({}, 1, &color, 1, &subpass, 1, &dependency));
        }
};

class VKFramebuffer : public VKDestroyResource<vk::Framebuffer> {
    public:
        VKFramebuffer(vk::Device dev, vk::RenderPass renderPass, vk::ImageView swapchainImageView, const vk::Extent2D& swapchainExtent)
            : VKDestroyResource(dev) {
            vk::FramebufferCreateInfo info;
            info.renderPass = renderPass;
            info.setAttachments(swapchainImageView);
            info.width = swapchainExtent.width;
            info.height = swapchainExtent.height;
            info.layers = 1;

            resource = dev.createFramebuffer(info);
        }
};

class VKSwapchain : public VKDestroyResource<vk::SwapchainKHR> {
    public:
        VKSwapchain(vk::Device dev, vk::PhysicalDevice physDev, vk::SurfaceKHR surface,
            vk::Extent2D& swapchainExtent, int& width, int& height, vk::SurfaceFormatKHR swapchainFormat,
            uint32_t gi, uint32_t pi, uint32_t ti, bool vsync, vk::SwapchainKHR oldChain)
            : VKDestroyResource(dev) {
            // Setting the size of the swap chain images.
            vk::SurfaceCapabilitiesKHR sc = physDev.getSurfaceCapabilitiesKHR(surface);
            // 0xFFFFFFFF indicates to just rely on the size of the window clamped by the given maxs and mins.
            if (sc.currentExtent.width != 0xFFFFFFFF) {
                swapchainExtent = sc.currentExtent;
            } else {
                swapchainExtent.setWidth(std::clamp((uint32_t)width, sc.minImageExtent.width, sc.maxImageExtent.width));
                swapchainExtent.setHeight(std::clamp((uint32_t)height, sc.minImageExtent.height, sc.maxImageExtent.height));
            }

            // Setting the amount of images in the swap chain.
            uint32_t imageCount = sc.minImageCount + 1;
            if (sc.maxImageCount > 0 && imageCount > sc.maxImageCount) {
                imageCount = sc.maxImageCount;
            }

            vk::SwapchainCreateInfoKHR info;
            info.surface = surface;
            info.minImageCount = imageCount;
            info.imageFormat = swapchainFormat.format;
            info.imageColorSpace = swapchainFormat.colorSpace;
            info.imageExtent = swapchainExtent;
            info.imageArrayLayers = 1;
            info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
            info.imageSharingMode = vk::SharingMode::eExclusive;
            info.preTransform = sc.currentTransform;
            info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
            info.presentMode = vsync ? vk::PresentModeKHR::eFifoRelaxed : vk::PresentModeKHR::eMailbox;
            info.clipped = VK_TRUE;
            info.oldSwapchain = oldChain;

            // TODO: Remove.
            std::array indices{ gi, pi, ti };
            if (gi != pi || pi != ti) {
                info.imageSharingMode = vk::SharingMode::eConcurrent;
                info.setQueueFamilyIndices(indices);
            }

            resource = dev.createSwapchainKHR(info);
        }
};

class VKBuffer : public VKDestroyResource<vk::Buffer> {
    public:
        VKBuffer(vk::Device dev, vk::DeviceSize size, vk::BufferUsageFlags bufferUsage) : VKDestroyResource(dev) {
            vk::BufferCreateInfo info;
            info.size = size;
            info.usage = bufferUsage;
            info.sharingMode = vk::SharingMode::eExclusive;

            resource = device.createBuffer(info);
        }
};

class VKMemory : public VKFreeResource<vk::DeviceMemory> {
    public:
        template <typename T>
        VKMemory(vk::Device dev, vk::PhysicalDevice physDev, T data, vk::MemoryPropertyFlags memPropFlags) : VKFreeResource(dev) {
            vk::MemoryRequirements memReq;
            if constexpr (std::is_same<T, vk::Image>::value) {
                memReq = device.getImageMemoryRequirements(data);
            } else if constexpr (std::is_same<T, vk::Buffer>::value) {
                memReq = device.getBufferMemoryRequirements(data);
            } else {
                static_assert(false);
            } 

            // Finding a fitting memory type.
            int memIndex = -1;
            vk::PhysicalDeviceMemoryProperties memProps = physDev.getMemoryProperties();
            for (int i = 0; i < (int)memProps.memoryTypeCount; i++) {
                if ((memReq.memoryTypeBits & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & memPropFlags) == memPropFlags) {
                    memIndex = i;
                    break;
                }
            }
            PGE_ASSERT(memIndex != -1, "No suitable memory type found");


            vk::MemoryAllocateInfo info;
            info.allocationSize = memReq.size;
            info.memoryTypeIndex = memIndex;

            resource = device.allocateMemory(info);

            if constexpr (std::is_same<T, vk::Image>::value) {
                device.bindImageMemory(data, resource, 0);
            } else if constexpr (std::is_same<T, vk::Buffer>::value) {
                device.bindBufferMemory(data, resource, 0);
            } else {
                static_assert(false);
            }
        }
};

class VKPipelineInfo : PolymorphicHeap {
    public:
        // The extra info needs to remain in memory.
        vk::Viewport viewport;
        vk::PipelineViewportStateCreateInfo viewportInfo;

        vk::PipelineColorBlendAttachmentState colorBlendAttachmentState;
        vk::PipelineColorBlendStateCreateInfo colorBlendInfo;

        vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
        vk::PipelineMultisampleStateCreateInfo multisamplerInfo;

        VKPipelineInfo() = default;

        void init(vk::Extent2D extent, const vk::Rect2D& scissor) {
            // Creating a viewport.
            viewport = vk::Viewport(0.f, (float)extent.height, (float)extent.width, -(float)extent.height, 0.f, 1.f);
            
            viewportInfo.setViewports(viewport);
            viewportInfo.setScissors(scissor);

            rasterizationInfo.polygonMode = vk::PolygonMode::eFill;
            rasterizationInfo.cullMode = vk::CullModeFlagBits::eFrontAndBack;
            rasterizationInfo.frontFace = vk::FrontFace::eCounterClockwise;
            rasterizationInfo.lineWidth = 1.f;
            
            multisamplerInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;

            // Color blending. (No idea what's going on here, just stole it from D3D11BlendState.)
            colorBlendAttachmentState.blendEnable = true;
            colorBlendAttachmentState.colorBlendOp = vk::BlendOp::eAdd;
            colorBlendAttachmentState.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
            colorBlendAttachmentState.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
            colorBlendAttachmentState.alphaBlendOp = vk::BlendOp::eAdd;
            colorBlendAttachmentState.srcAlphaBlendFactor = vk::BlendFactor::eOne;
            colorBlendAttachmentState.dstAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
            colorBlendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG;

            colorBlendInfo.setAttachments(colorBlendAttachmentState);
        }
};

class VKPipeline : public VKDestroyResource<vk::Pipeline> {
    private:
        static constexpr vk::PipelineInputAssemblyStateCreateInfo inputAssemblyLines{ { }, vk::PrimitiveTopology::eLineList };
        static constexpr vk::PipelineInputAssemblyStateCreateInfo inputAssemblyTris{ { }, vk::PrimitiveTopology::eTriangleList };

    public:
        // Sadly we can't make this any more straightforward, because we're in a header and including either shader or graphics would lead to circular inclusion.
        VKPipeline(vk::Device device, const std::array<vk::PipelineShaderStageCreateInfo, 2> shaderInfo, const vk::PipelineVertexInputStateCreateInfo* vertexInfo,
            vk::PipelineLayout layout, const VKPipelineInfo* pipelineInfo, vk::RenderPass renderPass, Mesh::PrimitiveType type)
            : VKDestroyResource(device) {
            const vk::PipelineInputAssemblyStateCreateInfo* inputInfo;
            switch (type) {
                case Mesh::PrimitiveType::LINE: {
                    inputInfo = &inputAssemblyLines;
                } break;
                default:
                case Mesh::PrimitiveType::TRIANGLE: {
                    inputInfo = &inputAssemblyTris;
                } break;
            }
            
            vk::GraphicsPipelineCreateInfo info;
            info.setStages(shaderInfo);
            info.pVertexInputState = vertexInfo;
            info.pInputAssemblyState = inputInfo;
            info.pViewportState = &pipelineInfo->viewportInfo;
            info.pRasterizationState = &pipelineInfo->rasterizationInfo;
            info.pMultisampleState = &pipelineInfo->multisamplerInfo;
            info.pColorBlendState = &pipelineInfo->colorBlendInfo;
            info.layout = layout;
            info.renderPass = renderPass;
            info.basePipelineIndex = -1;

            vk::ResultValue<vk::Pipeline> creation = device.createGraphicsPipeline(nullptr, info);
            PGE_ASSERT(creation.result == vk::Result::eSuccess, "Failed to create graphics pipeline (VKERROR: " + String::hexFromInt((u32)creation.result) + ")");
            resource = creation.value;
        }
};

class VKPipelineLayout : public VKDestroyResource<vk::PipelineLayout> {
    public:
        VKPipelineLayout(vk::Device dev, const std::vector<vk::PushConstantRange>& ranges, std::optional<vk::DescriptorSetLayout> layout = { }) : VKDestroyResource(dev) {
            vk::PipelineLayoutCreateInfo info;
            info.setPushConstantRanges(ranges);
            if (layout.has_value()) {
                info.setSetLayouts(layout.value());
            }

            resource = device.createPipelineLayout(info);
        }
};

class VKShader : public VKDestroyResource<vk::ShaderModule> {
    public:
        VKShader(vk::Device dev, const std::vector<byte>& shaderBinary) : VKDestroyResource(dev) {
            vk::ShaderModuleCreateInfo info;
            // WTF is wrong with vulkan?? Code as u32 ptr, but size in bytes, makes sense!
            info.codeSize = shaderBinary.size();
            info.pCode = (u32*)shaderBinary.data();

            resource = device.createShaderModule(info);
        }
};

class VKImage : public VKDestroyResource<vk::Image> {
    public:
        VKImage(vk::Device dev, int w, int h, vk::Format fmt) : VKDestroyResource(dev) {
            vk::ImageCreateInfo info;
            info.imageType = vk::ImageType::e2D;
            info.format = fmt;
            info.extent = vk::Extent3D(w, h, 1);
            info.mipLevels = 1;
            info.arrayLayers = 1;
            info.samples = vk::SampleCountFlagBits::e1;
            info.tiling = vk::ImageTiling::eOptimal;
            info.initialLayout = vk::ImageLayout::eUndefined;
            info.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
            info.sharingMode = vk::SharingMode::eExclusive;

            resource = device.createImage(info);
        }
};

class VKSampler : public VKDestroyResource<vk::Sampler> {
    public:
        VKSampler(vk::Device dev, bool rt) : VKDestroyResource(dev) {
            vk::SamplerCreateInfo info;
            info.minFilter = info.magFilter = rt ? vk::Filter::eNearest : vk::Filter::eLinear;
            info.anisotropyEnable = VK_TRUE;
            info.maxAnisotropy = rt ? 1.f : 4.f; // TODO: Clamp with max physical device.
            info.addressModeU = info.addressModeV = info.addressModeW = vk::SamplerAddressMode::eRepeat;
            info.mipmapMode = vk::SamplerMipmapMode::eLinear;

            resource = dev.createSampler(info);
        }
};

class VKDescriptorSetLayout : public VKDestroyResource<vk::DescriptorSetLayout> {
    public:
        VKDescriptorSetLayout(vk::Device dev, int count) : VKDestroyResource(dev) {
            vk::DescriptorSetLayoutBinding binding;
            binding.descriptorCount = count;
            binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
            binding.stageFlags = vk::ShaderStageFlagBits::eFragment;

            vk::DescriptorSetLayoutCreateInfo info;
            info.setBindings(binding);

            resource = dev.createDescriptorSetLayout(info);
        }
};

class VKDescriptorPool : public VKDestroyResource<vk::DescriptorPool> {
    public:
        VKDescriptorPool(vk::Device dev, int count) : VKDestroyResource(dev) {
            vk::DescriptorPoolSize size;
            size.descriptorCount = count;
            size.type = vk::DescriptorType::eCombinedImageSampler;

            vk::DescriptorPoolCreateInfo info;
            info.setPoolSizes(size);
            info.maxSets = count;

            resource = dev.createDescriptorPool(info);
        }
};

}

#endif // PGEINTERNAL_RESOURCEMANAGEMENT_VK
