#ifndef PGE_MATERIAL_H_INCLUDED
#define PGE_MATERIAL_H_INCLUDED

#include <vector>

namespace PGE {

class Shader;
class Texture;

/// Can be applied to a mesh and simply consists of a #PGE::Shader and optional #PGE::Texture (s).
/// If a material is not opaque z-buffering will never be applied to the rendered mesh.
class Material {
    public:
        Material(Shader& sh, const std::vector<std::reference_wrapper<Texture>>& t, bool opaq = true);
        Material(Shader& sh, Texture& t, bool opaq = true);
        Material(Shader& sh, bool opaq = true);

        Shader& getShader() const;
        Texture& getTexture(int index) const;
        int getTextureCount() const;
        bool isOpaque() const;

    private:
        Shader& shader; std::vector<std::reference_wrapper<Texture>> textures;
        const bool opaque;
};

}

#endif // PGE_MATERIAL_H_INCLUDED
