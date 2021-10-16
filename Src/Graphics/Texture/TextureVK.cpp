#include "../GraphicsVK.h"

#include "../../ResourceManagement/VK.h"

using namespace PGE;

vk::Format TextureVK::getFormat(Texture::Format fmt) {
    switch (fmt) {
        case Texture::Format::RGBA64: { return vk::Format::eR16G16B16A16Unorm; }
        case Texture::Format::RGBA32: { return vk::Format::eR8G8B8A8Unorm; }
        case Texture::Format::R32F: { return vk::Format::eR32Sfloat; }
        case Texture::Format::R8: { return vk::Format::eR8Unorm; }
        default: { throw PGE_CREATE_EX("Invalid format"); }
    }
}

vk::Format TextureVK::getFormat(Texture::CompressedFormat fmt) {
    switch (fmt) {
        case Texture::CompressedFormat::BC1: { return vk::Format::eBc1RgbUnormBlock; }
        case Texture::CompressedFormat::BC2: { return vk::Format::eBc2UnormBlock; }
        case Texture::CompressedFormat::BC3: { return vk::Format::eBc3UnormBlock; }
        case Texture::CompressedFormat::BC4: { return vk::Format::eBc4UnormBlock; }
        case Texture::CompressedFormat::BC5: { return vk::Format::eBc5UnormBlock; }
        case Texture::CompressedFormat::BC6: { return vk::Format::eBc6HSfloatBlock; }
        case Texture::CompressedFormat::BC7: { return vk::Format::eBc7UnormBlock; }
        default: { throw PGE_CREATE_EX("Invalid format"); }
    }
}

vk::Format TextureVK::getFormat(const Texture::AnyFormat& fmt) {
    if (std::holds_alternative<Texture::Format>(fmt)) {
        return getFormat(std::get<Texture::Format>(fmt));
    } else {
        return getFormat(std::get<Texture::CompressedFormat>(fmt));
    }
}

TextureVK::TextureVK(Graphics& gfx, int w, int h, Format fmt)
    : Texture(w, h, true, fmt), resourceManager(gfx) {
    graphics = (GraphicsVK*)&gfx;
    vk::Device device = graphics->getDevice();
    vk::PhysicalDevice physicalDevice = graphics->getPhysicalDevice();

    format = getFormat(fmt);

    image = resourceManager.addNewResource<VKImage>(device, w, h, format, 1, VKImage::Usage::RENDER_TARGET);
    imageMem = resourceManager.addNewResource<VKMemory>(device, physicalDevice, image.get(), vk::MemoryPropertyFlagBits::eDeviceLocal);
    graphics->transformImage<GraphicsVK::ImageLayout::UNDEFINED, GraphicsVK::ImageLayout::RENDER_TARGET>(image, 1);
    imageView = resourceManager.addNewResource<VKImageView>(device, image, format, 1);

    renderPass = graphics->requestRenderPass(format);
    depth = resourceManager.addNewResource<RawWrapper<TextureVK>>(gfx, w, h);
    framebuffer = resourceManager.addNewResource<VKFramebuffer>(device, renderPass, imageView.get(), depth->getImageView(), vk::Extent2D(w, h));
}

TextureVK::TextureVK(Graphics& gfx, int w, int h, const byte* buffer, Format fmt, bool mipmaps)
    : Texture(w, h, false, fmt), resourceManager(gfx) {
    GraphicsVK& graphics = (GraphicsVK&)gfx;
    vk::Device device = graphics.getDevice();
    vk::PhysicalDevice physicalDevice = graphics.getPhysicalDevice();

    int miplevels = 1;
    if (mipmaps) {
        int maxDim = std::max(w, h);
        int pot = 2;
        while (pot <= maxDim) {
            pot *= 2;
            miplevels++;
        }
    }

    int size = w * h * getBytesPerPixel(fmt);
    VKMemoryBuffer staging(device, physicalDevice, size, VKMemoryBuffer::Type::STAGING);
    memcpy(staging.getData(), buffer, size);
    staging.flush(VK_WHOLE_SIZE);

    vk::Format vkFmt = getFormat(fmt);
    PGE_ASSERT(physicalDevice.getFormatProperties(vkFmt).optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear,
        "Format doesn't support linear filtering");
    image = resourceManager.addNewResource<VKImage>(device, w, h, vkFmt, miplevels, mipmaps ? VKImage::Usage::IMAGE_GEN_MIPS : VKImage::Usage::IMAGE);
    imageMem = resourceManager.addNewResource<VKMemory>(device, physicalDevice, image.get(), vk::MemoryPropertyFlagBits::eDeviceLocal);
    graphics.transformImage<GraphicsVK::ImageLayout::UNDEFINED, GraphicsVK::ImageLayout::TRANSFER_DST>(image, miplevels);
    graphics.transferToImage(staging.getBuffer(), image, w, h);
    if (mipmaps) {
        graphics.generateMipmaps(image, w, h, miplevels);
    } else {
        graphics.transformImage<GraphicsVK::ImageLayout::TRANSFER_DST, GraphicsVK::ImageLayout::SHADER_READ>(image, miplevels);
    }

    imageView = resourceManager.addNewResource<VKImageView>(device, image, vkFmt, miplevels);
}

TextureVK::TextureVK(Graphics& gfx, const std::vector<Mipmap>& mipmaps, CompressedFormat fmt)
    : Texture(mipmaps[0].width, mipmaps[0].height, false, fmt), resourceManager(gfx) {
    GraphicsVK& graphics = (GraphicsVK&)gfx;
    vk::Device device = graphics.getDevice();
    vk::PhysicalDevice physicalDevice = graphics.getPhysicalDevice();

    vk::Format vkFmt = getFormat(fmt);
    PGE_ASSERT(physicalDevice.getFormatProperties(vkFmt).optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear,
        "Format doesn't support linear filtering");

    image = resourceManager.addNewResource<VKImage>(device, mipmaps[0].width, mipmaps[0].height, vkFmt, (int)mipmaps.size(), VKImage::Usage::IMAGE);
    imageMem = resourceManager.addNewResource<VKMemory>(device, physicalDevice, image.get(), vk::MemoryPropertyFlagBits::eDeviceLocal);
    graphics.transformImage<GraphicsVK::ImageLayout::UNDEFINED, GraphicsVK::ImageLayout::TRANSFER_DST>(image, (int)mipmaps.size());

    VKBuffer stagingBuffer(device, mipmaps[0].size, vk::BufferUsageFlagBits::eTransferSrc);
    VKMemory stagingMemory(device, physicalDevice, stagingBuffer.get(), vk::MemoryPropertyFlagBits::eHostVisible);
    void* mappedMem = device.mapMemory(stagingMemory, 0, VK_WHOLE_SIZE);
    
    for (int i = 0; i < mipmaps.size(); i++) {
        memcpy(mappedMem, mipmaps[i].buffer, mipmaps[i].size);
        device.flushMappedMemoryRanges(vk::MappedMemoryRange(stagingMemory, 0, Math::roundUp((vk::DeviceSize)mipmaps[i].size, graphics.getAtomSize())));
        graphics.transferToImage(stagingBuffer, image, mipmaps[i].width, mipmaps[i].height, i);
    }

    device.unmapMemory(stagingMemory);

    graphics.transformImage<GraphicsVK::ImageLayout::TRANSFER_DST, GraphicsVK::ImageLayout::SHADER_READ>(image, (int)mipmaps.size());

    imageView = resourceManager.addNewResource<VKImageView>(device, image, vkFmt, (int)mipmaps.size());
}

TextureVK::TextureVK(Graphics& gfx, int w, int h) : Texture(w, h, false, Texture::Format::R32F), resourceManager(gfx) {
    GraphicsVK& graphics = (GraphicsVK&)gfx;
    vk::Device device = graphics.getDevice();
    vk::PhysicalDevice physicalDevice = graphics.getPhysicalDevice();

    constexpr vk::Format DEPTH_FORMAT = vk::Format::eD32Sfloat;
    PGE_ASSERT(physicalDevice.getFormatProperties(DEPTH_FORMAT).optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment,
        "Depth stencil not supported!"); // TODO: Move or sth idfk.
    image = resourceManager.addNewResource<VKImage>(device, w, h, DEPTH_FORMAT, 1, VKImage::Usage::DEPTH);
    imageMem = resourceManager.addNewResource<VKMemory>(device, physicalDevice, image.get(), vk::MemoryPropertyFlagBits::eDeviceLocal);
    imageView = resourceManager.addNewResource<VKImageView>(device, image, DEPTH_FORMAT, 1, vk::ImageAspectFlagBits::eDepth);
}

TextureVK::~TextureVK() {
    if (isRenderTarget()) {
        graphics->returnRenderPass(format);
    }
}

const vk::ImageView TextureVK::getImageView() const {
    return imageView;
}

const vk::RenderPass TextureVK::getRenderPass() const {
    return renderPass;
}

const vk::Framebuffer TextureVK::getFramebuffer() const {
    return framebuffer;
}

vk::Format TextureVK::getFormat() const {
    return format;
}

void* TextureVK::getNative() const {
    return (void*)&image;
}
