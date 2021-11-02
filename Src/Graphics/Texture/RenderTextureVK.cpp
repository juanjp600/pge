#include "../GraphicsVK.h"

using namespace PGE;

RenderTextureVK::RenderTextureVK(Graphics& gfx, int w, int h, Format fmt)
    : TextureVK(gfx, w, h, fmt), graphics((GraphicsVK&)gfx) {
    vk::Device device = graphics.getDevice();
    vk::PhysicalDevice physicalDevice = graphics.getPhysicalDevice();

    format = TextureVK::getFormat(fmt);

    image = resourceManager.addNewResource<VKImage>(device, w, h, format, 1, VKImage::Usage::RENDER_TARGET);
    imageMem = resourceManager.addNewResource<VKMemory>(device, physicalDevice, image.get(), vk::MemoryPropertyFlagBits::eDeviceLocal);
    graphics.transformImage<GraphicsVK::ImageLayout::UNDEFINED, GraphicsVK::ImageLayout::SHADER_READ>(image, 1);
    imageView = resourceManager.addNewResource<VKImageView>(device, image, format, 1);

    renderInfo.pass = graphics.requestRenderPass(format);
    depth = resourceManager.addNewResource<RawWrapper<TextureVK>>(gfx, w, h);
    renderInfo.buffer = resourceManager.addNewResource<VKFramebuffer>(device, renderInfo.pass, imageView.get(), depth->getImageView(), vk::Extent2D(w, h));

    scissor = vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(w, h));
}

RenderTextureVK::~RenderTextureVK() {
    graphics.returnRenderPass(format);
    for (u64 r : associatedMultiRTResources) {
        graphics.destroyMultiRTResources(r);
    }
}

void RenderTextureVK::associateMultiRTResources(u64 id) {
    associatedMultiRTResources.push_back(id);
}

const RenderInfo& RenderTextureVK::getRenderInfo() const {
    return renderInfo;
}

vk::Format RenderTextureVK::getFormat() const {
    return format;
}

const vk::Rect2D& RenderTextureVK::getScissor() const {
    return scissor;
}

vk::ImageView RenderTextureVK::getDepthView() const {
    return depth->getImageView();
}
