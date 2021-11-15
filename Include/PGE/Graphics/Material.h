#ifndef PGE_MATERIAL_H_INCLUDED
#define PGE_MATERIAL_H_INCLUDED

#include <PGE/Types/Types.h>
#include <PGE/ResourceManagement/PolymorphicHeap.h>

namespace PGE {

class Graphics; class Shader; class Texture;

enum class Opaque {
	YES,
	NO,
};

class Material : private PolymorphicHeap {
	public:
		static Material* create(Graphics& gfx, Shader& sh, Opaque o);
		static Material* create(Graphics& gfx, Shader& sh, Texture& tex, Opaque o);
		static Material* create(Graphics& gfx, Shader& sh, const ReferenceVector<Texture>& texs, Opaque o);

		Material(Graphics& gfx, Shader& sh, const ReferenceVector<Texture>& texs, Opaque o);

		bool isOpaque() const;

		Shader& getShader() const;

		int getTextureCount() const;
		const Texture& getTexture(int index) const;

	private:
		Shader* shader;
		ReferenceVector<Texture> textures;
		Opaque opaque;
};

}

#endif // PGE_MATERIAL_H_INCLUDED
