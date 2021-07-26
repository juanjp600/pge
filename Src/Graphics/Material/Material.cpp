#include <PGE/Graphics/Material.h>

using namespace PGE;

Material::Material(Shader& sh, const std::vector<std::reference_wrapper<Texture>>& t, bool opaq)
    : shader(sh), textures(t), opaque(opaq) { }

Material::Material(Shader& sh, Texture& t,bool opaq)
    : shader(sh), opaque(opaq) {
    textures.push_back(t);
}

Material::Material(Shader& sh,bool opaq)
    : shader(sh), opaque(opaq) { }

Shader& Material::getShader() const {
    return shader;
}

Texture& Material::getTexture(int index) const {
    return textures[index];
}

int Material::getTextureCount() const {
    return (int)textures.size();
}

bool Material::isOpaque() const {
    return opaque;
}
