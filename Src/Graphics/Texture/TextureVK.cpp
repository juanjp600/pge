#include "../GraphicsVK.h"

#include "../../ResourceManagement/VK.h"

using namespace PGE;

static void assertVKLinearFilteringSupport(const vk::FormatProperties& properties,
    const std::source_location& location = std::source_location::current()) {
    asrt((properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear) != (vk::FormatFeatureFlagBits)0,
        "Format doesn't support linear filtering", location);
}

vk::Format TextureVK::getFormat(Texture::Format fmt) {
    switch (fmt) {
        using enum Texture::Format;
        case RGBA64: { return vk::Format::eR16G16B16A16Unorm; }
        case RGBA32: { return vk::Format::eR8G8B8A8Unorm; }
        case R32F: { return vk::Format::eR32Sfloat; }
        case R8: { return vk::Format::eR8Unorm; }
        default: { throw Exception("Invalid format"); }
    }
}

vk::Format TextureVK::getFormat(Texture::CompressedFormat fmt) {
    switch (fmt) {
        using enum Texture::CompressedFormat;
        case BC1: { return vk::Format::eBc1RgbUnormBlock; }
        case BC2: { return vk::Format::eBc2UnormBlock; }
        case BC3: { return vk::Format::eBc3UnormBlock; }
        case BC4: { return vk::Format::eBc4UnormBlock; }
        case BC5: { return vk::Format::eBc5UnormBlock; }
        case BC6: { return vk::Format::eBc6HSfloatBlock; }
        case BC7: { return vk::Format::eBc7UnormBlock; }
        default: { throw Exception("Invalid format"); }
    }
}

vk::Format TextureVK::getFormat(const Texture::AnyFormat& fmt) {
    if (std::holds_alternative<Texture::Format>(fmt)) {
        return getFormat(std::get<Texture::Format>(fmt));
    } else {
        return getFormat(std::get<Texture::CompressedFormat>(fmt));
    }
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
    VKMemoryBuffer& staging(graphics.getTempStagingBuffer(size));
    memcpy(staging.getData(), buffer, size);
    staging.flush(VK_WHOLE_SIZE);

    vk::Format vkFmt = getFormat(fmt);
    assertVKLinearFilteringSupport(physicalDevice.getFormatProperties(vkFmt));
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
    assertVKLinearFilteringSupport(physicalDevice.getFormatProperties(vkFmt));

    image = resourceManager.addNewResource<VKImage>(device, mipmaps[0].width, mipmaps[0].height, vkFmt, (int)mipmaps.size(), VKImage::Usage::IMAGE);
    imageMem = resourceManager.addNewResource<VKMemory>(device, physicalDevice, image.get(), vk::MemoryPropertyFlagBits::eDeviceLocal);
    graphics.transformImage<GraphicsVK::ImageLayout::UNDEFINED, GraphicsVK::ImageLayout::TRANSFER_DST>(image, (int)mipmaps.size());

    VKMemoryBuffer& staging = graphics.getTempStagingBuffer(mipmaps[0].size);
    
    for (const Mipmap& m : mipmaps) {
        memcpy(staging.getData(), m.buffer, m.size);
        staging.flush(m.size);
        graphics.transferToImage(staging.getBuffer(), image, m.width, m.height, i);
    }

    graphics.transformImage<GraphicsVK::ImageLayout::TRANSFER_DST, GraphicsVK::ImageLayout::SHADER_READ>(image, (int)mipmaps.size());

    imageView = resourceManager.addNewResource<VKImageView>(device, image, vkFmt, (int)mipmaps.size());
}

TextureVK::TextureVK(Graphics& gfx, int w, int h) : Texture(w, h, false, Texture::Format::R32F), resourceManager(gfx) {
    GraphicsVK& graphics = (GraphicsVK&)gfx;
    vk::Device device = graphics.getDevice();
    vk::PhysicalDevice physicalDevice = graphics.getPhysicalDevice();

    image = resourceManager.addNewResource<VKImage>(device, w, h, UtilVK::DEPTH_FORMAT, 1, VKImage::Usage::DEPTH);
    imageMem = resourceManager.addNewResource<VKMemory>(device, physicalDevice, image.get(), vk::MemoryPropertyFlagBits::eDeviceLocal);
    imageView = resourceManager.addNewResource<VKImageView>(device, image, UtilVK::DEPTH_FORMAT, 1, vk::ImageAspectFlagBits::eDepth);
}

const vk::ImageView TextureVK::getImageView() const {
    return imageView;
}

void* TextureVK::getNative() const {
    return (void*)&image;
}
