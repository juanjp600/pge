#ifndef PGE_MATERIALVK_H_INCLUDED
#define PGE_MATERIALVK_H_INCLUDED

#include <PGE/Graphics/Material.h>

#include "../../ResourceManagement/VK.h"
#include "../../ResourceManagement/ResourceManagerVK.h"

namespace PGE {

class MaterialVK : public Material {
	public:
		MaterialVK(Graphics& gfx, Shader& sh, const ReferenceVector<Texture>& texs, Opaque o);

		const vk::DescriptorSet& getDescriptorSet() const;

	private:
		VKDescriptorPool::View dPool;
		vk::DescriptorSet dSet;

		ResourceManagerVK resourceManager;
};

}

#endif // PGE_MATERIALVK_H_INCLUDED