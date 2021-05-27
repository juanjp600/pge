#ifndef PGE_MATERIAL_H_INCLUDED
#define PGE_MATERIAL_H_INCLUDED

#include <vector>

namespace PGE {

class Shader;
class Texture;

class Material {
    public:
        Material(Shader* sh, const std::vector<Texture*>& t, bool opaq = true);
        Material(Shader* sh, Texture* t, bool opaq = true);
        Material(Shader* sh, bool opaq = true);

        Shader* getShader() const;
        Texture* getTexture(int index) const;
        int getTextureCount() const;
        bool isOpaque() const;

    protected:
        Material() = default;
        Shader* shader; std::vector<Texture*> textures;
        bool opaque;
};

}

#endif // PGE_MATERIAL_H_INCLUDED
