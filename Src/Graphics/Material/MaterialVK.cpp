#include "MaterialVK.h"

#include "../GraphicsVK.h"

using namespace PGE;

MaterialVK::MaterialVK(Graphics& gfx, Shader& sh, const ReferenceVector<Texture>& texs, Opaque o)
	: Material(gfx, sh, texs, o), resourceManager(gfx), graphics((GraphicsVK&)gfx) {
    if (texs.empty()) { return; }

    vk::Device device = graphics.getDevice();

    dPool = resourceManager.addNewResource<VKDescriptorPool>(device, texs.size());

    vk::DescriptorSetAllocateInfo allocInfo;
    allocInfo.descriptorPool = dPool;
    std::vector<vk::DescriptorSetLayout> layouts(1, graphics.getDescriptorSetLayout(texs.size()));
    allocInfo.setSetLayouts(layouts);
    dSet = device.allocateDescriptorSets(allocInfo).front();
    
    std::vector<vk::DescriptorImageInfo> infos(texs.size());
    for (int i = 0; i < infos.size(); i++) {
        TextureVK& tex = (TextureVK&)texs[i].get();

        infos[i].imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        infos[i].imageView = tex.getImageView();
        infos[i].sampler = graphics.getSampler(tex.isRenderTarget());
    }

    vk::WriteDescriptorSet set;
    set.dstSet = dSet;
    set.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    set.setImageInfo(infos);

    device.updateDescriptorSets(set, { });
}

MaterialVK::~MaterialVK() {
    if (getTextureCount() > 0) {
        graphics.dropDescriptorSetLayout(getTextureCount());
    }
}

const vk::DescriptorSet& MaterialVK::getDescriptorSet() const {
    return dSet;
}
