#ifndef PGEINTERNAL_RESOURCEMANAGEMENT_VK
#define PGEINTERNAL_RESOURCEMANAGEMENT_VK

#include <PGE/ResourceManagement/ResourceManager.h>

#include <set>

#include <vulkan/vulkan.hpp>
#include <SDL_vulkan.h>

#include <PGE/Graphics/Mesh.h>

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

            vk::ApplicationInfo vkAppInfo = vk::ApplicationInfo(name.cstr(), VK_MAKE_VERSION(0, 0, 0), "pulsegun engine", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_1);
            resource = vk::createInstance(vk::InstanceCreateInfo({}, &vkAppInfo, layers, extensions));
        }

        ~VKInstance() {
            resource.destroy();
        }
};

class VKDevice : public Resource<vk::Device> {
    public:
        VKDevice(vk::PhysicalDevice physDev, const std::set<uint32_t>& queueIndices, const std::vector<const char*>& layers, const std::vector<const char*>& deviceExtensions) {
            // Creating the logical device.
            constexpr float queuePriority = 1.f;
            std::vector<vk::DeviceQueueCreateInfo> infos;
            infos.reserve(3);
            for (uint32_t index : queueIndices) {
                infos.push_back(vk::DeviceQueueCreateInfo({}, index, 1, &queuePriority));
            }
            resource = physDev.createDevice(vk::DeviceCreateInfo({}, infos, layers, deviceExtensions));
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

            // Creating the window's surface via SDL.
            // TODO: Change SDL to be more epic.
            bool success = SDL_Vulkan_CreateSurface(window, (VkInstance)instance, (VkSurfaceKHR*)&resource);
            PGE_ASSERT(success, "Failed to create Vulkan surface (SDLERROR: " + String(SDL_GetError()) + ")");
        }

        ~VKSurface() {
            instance.destroySurfaceKHR(resource);
        }
};

class VKSemaphore : public VKDestroyResource<vk::Semaphore> {
    public:
        VKSemaphore(vk::Device dev) : VKDestroyResource(dev) {
            resource = dev.createSemaphore(vk::SemaphoreCreateInfo({}));
        }
};

class VKFence : public VKDestroyResource<vk::Fence> {
    public:
        VKFence(vk::Device dev, bool signaled = false) : VKDestroyResource(dev) {
            resource = dev.createFence(vk::FenceCreateInfo(signaled ? vk::FenceCreateFlagBits::eSignaled : (vk::FenceCreateFlags)0));
        }
};

class VKCommandPool : public VKDestroyResource<vk::CommandPool> {
    public:
        VKCommandPool(vk::Device dev, int queueIndex) : VKDestroyResource(dev) {
            resource = dev.createCommandPool(vk::CommandPoolCreateInfo({}, queueIndex));
        }
};

class VKImageView : public VKDestroyResource<vk::ImageView> {
    public:
        VKImageView(vk::Device dev, vk::Image swapchainImage, vk::SurfaceFormatKHR swapchainFormat) : VKDestroyResource(dev) {
            vk::ImageViewCreateInfo ivci = vk::ImageViewCreateInfo({}, swapchainImage, vk::ImageViewType::e2D, swapchainFormat.format);
            ivci.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
            resource = dev.createImageView(ivci);
        }
};

class VKRenderPass : public VKDestroyResource<vk::RenderPass> {
    public:
        VKRenderPass(vk::Device dev, vk::SurfaceFormatKHR swapchainFormat) : VKDestroyResource(dev) {
            vk::AttachmentDescription colorAttachment = vk::AttachmentDescription({}, swapchainFormat.format, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);
            vk::AttachmentReference colorAttachmentRef = vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal);
            vk::SubpassDescription subpass = vk::SubpassDescription({}, vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &colorAttachmentRef);
            vk::SubpassDependency dependency = vk::SubpassDependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput, {}, vk::AccessFlagBits::eColorAttachmentWrite);
            resource = dev.createRenderPass(vk::RenderPassCreateInfo({}, 1, &colorAttachment, 1, &subpass, 1, &dependency));
        }
};

class VKFramebuffer : public VKDestroyResource<vk::Framebuffer> {
    public:
        VKFramebuffer(vk::Device dev, vk::RenderPass renderPass, vk::ImageView swapchainImageView, vk::Extent2D swapchainExtent) : VKDestroyResource(dev) {
            resource = dev.createFramebuffer(vk::FramebufferCreateInfo({}, renderPass, 1, &swapchainImageView, swapchainExtent.width, swapchainExtent.height, 1));
        }
};

class VKSwapchain : public VKDestroyResource<vk::SwapchainKHR> {
    public:
        // TODO: Why doesn't a reference work here?
        VKSwapchain(vk::Device dev, vk::PhysicalDevice physDev, vk::SurfaceKHR surface, vk::Extent2D* swapchainExtent, int& width, int& height, vk::SurfaceFormatKHR swapchainFormat, uint32_t gi, uint32_t pi, uint32_t ti, bool vsync) : VKDestroyResource(dev) {
            // Setting the size of the swap chain images.
            vk::SurfaceCapabilitiesKHR sc = physDev.getSurfaceCapabilitiesKHR(surface);
            // 0xFFFFFFFF indicates to just rely on the size of the window clamped by the given maxs and mins.
            if (sc.currentExtent.width != 0xFFFFFFFF) {
                swapchainExtent->setWidth(sc.currentExtent.width);
                swapchainExtent->setHeight(sc.currentExtent.height);
            } else {
                // TODO: Move MathUtil to PGE and use it here.
                swapchainExtent->setWidth((uint32_t)width < sc.minImageExtent.width ? sc.minImageExtent.width : (uint32_t)width > sc.maxImageExtent.width ? sc.maxImageExtent.width : (uint32_t)width);
                swapchainExtent->setHeight((uint32_t)height < sc.minImageExtent.height ? sc.minImageExtent.height : (uint32_t)height > sc.maxImageExtent.height ? sc.maxImageExtent.height : (uint32_t)height);
            }

            // Setting the amount of images in the swap chain.
            uint32_t imageCount = sc.minImageCount + 1;
            if (sc.maxImageCount > 0 && imageCount > sc.maxImageCount) {
                imageCount = sc.maxImageCount;
            }

            vk::SwapchainCreateInfoKHR sci = vk::SwapchainCreateInfoKHR({}, surface, imageCount, swapchainFormat.format, swapchainFormat.colorSpace, *swapchainExtent, 1, vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, 0, nullptr, sc.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque, vsync ? vk::PresentModeKHR::eFifoRelaxed : vk::PresentModeKHR::eMailbox, VK_TRUE, nullptr);
            // TODO: Remove.
            if (gi != pi || pi != ti) {
                sci.setImageSharingMode(vk::SharingMode::eConcurrent);
                std::vector<uint32_t> queueIndices = std::vector<uint32_t>{ gi, pi, ti };
                sci.setQueueFamilyIndices(queueIndices);
            }
            resource = dev.createSwapchainKHR(sci);
        }
};

class VKBuffer : public VKDestroyResource<vk::Buffer> {
    public:
        VKBuffer(vk::Device dev, vk::DeviceSize size, vk::BufferUsageFlags bufferUsage) : VKDestroyResource(dev) {
            vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo({}, size, bufferUsage, vk::SharingMode::eExclusive);
            resource = device.createBuffer(bufferInfo);
        }
};

class VKMemory : public VKFreeResource<vk::DeviceMemory> {
    public:
        VKMemory(vk::Device dev, vk::PhysicalDevice physDev, vk::Buffer buffer, vk::MemoryPropertyFlags memPropFlags) : VKFreeResource(dev) {
            vk::MemoryRequirements memReq = device.getBufferMemoryRequirements(buffer);

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


            vk::MemoryAllocateInfo memoryInfo = vk::MemoryAllocateInfo(memReq.size, memIndex);
            resource = device.allocateMemory(memoryInfo);

            device.bindBufferMemory(buffer, resource, 0);
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

        void init(vk::Extent2D extent, vk::Rect2D* scissor) {
            // Creating a viewport.
            viewport = vk::Viewport(0.f, (float)extent.height, (float)extent.width, -(float)extent.height, 0.f, 1.f);
            viewportInfo = vk::PipelineViewportStateCreateInfo({}, 1, &viewport, 1, scissor);

            rasterizationInfo = vk::PipelineRasterizationStateCreateInfo({}, false, false, vk::PolygonMode::eFill, vk::CullModeFlagBits::eFrontAndBack, vk::FrontFace::eCounterClockwise, false, 0.f, 0.f, 0.f, 1.f);
            multisamplerInfo = vk::PipelineMultisampleStateCreateInfo({}, vk::SampleCountFlagBits::e1, false, 0.f, nullptr, false, false);

            // Color blending.
            colorBlendAttachmentState = vk::PipelineColorBlendAttachmentState(false);
            colorBlendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG;
            colorBlendInfo = vk::PipelineColorBlendStateCreateInfo({}, false, vk::LogicOp::eClear, 1, &colorBlendAttachmentState);
        }
};

class VKPipeline : public VKDestroyResource<vk::Pipeline> {
    private:
        static constexpr vk::PipelineInputAssemblyStateCreateInfo inputAssemblyLines = vk::PipelineInputAssemblyStateCreateInfo({}, vk::PrimitiveTopology::eLineList, false);
        static constexpr vk::PipelineInputAssemblyStateCreateInfo inputAssemblyTris = vk::PipelineInputAssemblyStateCreateInfo({}, vk::PrimitiveTopology::eTriangleList, false);

    public:
        // Sadly we can't make this any more straightforward, because we're in a header and including either shader or graphics would lead to circular inclusion.
        VKPipeline(vk::Device device, const vk::PipelineShaderStageCreateInfo* shaderInfo, const vk::PipelineVertexInputStateCreateInfo* vertexInfo, vk::PipelineLayout layout, const VKPipelineInfo* info, vk::RenderPass renderPass, Mesh::PrimitiveType type) : VKDestroyResource(device) {
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
            vk::GraphicsPipelineCreateInfo pipelineInfo = vk::GraphicsPipelineCreateInfo({}, 2, shaderInfo, vertexInfo, inputInfo, nullptr, &info->viewportInfo, &info->rasterizationInfo, &info->multisamplerInfo, nullptr, &info->colorBlendInfo, nullptr, layout, renderPass, 0, {}, -1);
            vk::ResultValue<vk::Pipeline> creation = device.createGraphicsPipeline(nullptr, pipelineInfo);
            PGE_ASSERT(creation.result == vk::Result::eSuccess, "Failed to create graphics pipeline (VKERROR: " + String::hexFromInt((u32)creation.result) + ")");
            resource = creation.value;
        }
};

class VKPipelineLayout : public VKDestroyResource<vk::PipelineLayout> {
    public:
        VKPipelineLayout(vk::Device dev, const std::vector<vk::PushConstantRange>& ranges) : VKDestroyResource(dev) {
            vk::PipelineLayoutCreateInfo layoutInfo({}, 0, nullptr, (uint32_t)ranges.size(), ranges.data());
            resource = device.createPipelineLayout(layoutInfo);
        }
};

class VKShader : public VKDestroyResource<vk::ShaderModule> {
    public:
        VKShader(vk::Device dev, const std::vector<byte>& shaderBinary) : VKDestroyResource(dev) {
            vk::ShaderModuleCreateInfo shaderCreateInfo = vk::ShaderModuleCreateInfo({}, shaderBinary.size(), (uint32_t*)shaderBinary.data());
            resource = device.createShaderModule(shaderCreateInfo);
        }
};

}

#endif // PGEINTERNAL_RESOURCEMANAGEMENT_VK
