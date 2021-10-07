#include "../GraphicsVK.h"

#include "../../ResourceManagement/VK.h"

using namespace PGE;

static vk::Format getFormat(Texture::Format fmt) {
    switch (fmt) {
        case Texture::Format::RGBA64: { return vk::Format::eR16G16B16A16Unorm; }
        case Texture::Format::RGBA32: { return vk::Format::eR8G8B8A8Unorm; }
        case Texture::Format::R32F: { return vk::Format::eR32Sfloat; }
        case Texture::Format::R8: { return vk::Format::eR8Unorm; }
        default: { throw PGE_CREATE_EX("Invalid format"); }
    }
}

TextureVK::TextureVK(Graphics& gfx, int w, int h, Format fmt)
    : Texture(w, h, true, fmt), resourceManager(gfx) {

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
    VKBuffer stagingBuffer(device, size, vk::BufferUsageFlagBits::eTransferSrc);
    VKMemory stagingMemory(device, physicalDevice, stagingBuffer.get(), vk::MemoryPropertyFlagBits::eHostVisible);
    void* mappedMem = device.mapMemory(stagingMemory, 0, VK_WHOLE_SIZE);
    memcpy(mappedMem, buffer, size);
    device.flushMappedMemoryRanges(vk::MappedMemoryRange(stagingMemory, 0, VK_WHOLE_SIZE));
    device.unmapMemory(stagingMemory);

    vk::Format vkFmt = getFormat(fmt);
    image = resourceManager.addNewResource<VKImage>(device, w, h, vkFmt, miplevels, mipmaps);
    imageMem = resourceManager.addNewResource<VKMemory>(device, physicalDevice, image.get(), vk::MemoryPropertyFlagBits::eDeviceLocal);
    graphics.transformImage<GraphicsVK::ImageLayout::UNDEFINED, GraphicsVK::ImageLayout::TRANSFER_DST>(image, vkFmt, miplevels);
    graphics.transferToImage(stagingBuffer, image, w, h);
    if (mipmaps) {
        graphics.generateMipmaps(image, w, h, miplevels);
    } else {
        graphics.transformImage<GraphicsVK::ImageLayout::TRANSFER_DST, GraphicsVK::ImageLayout::SHADER_READ>(image, vkFmt, miplevels);
    }

    imageView = resourceManager.addNewResource<VKImageView>(device, image, vkFmt, miplevels);
}

TextureVK::TextureVK(Graphics& gfx, const std::vector<Mipmap>& mipmaps, CompressedFormat fmt)
    : Texture(mipmaps[0].width, mipmaps[0].height, false, fmt), resourceManager(gfx) {

}

const vk::ImageView& TextureVK::getImageView() const {
    return imageView;
}

void* TextureVK::getNative() const {
    return nullptr;
}
