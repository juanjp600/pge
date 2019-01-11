#include <Material/Material.h>

using namespace PGE;

Material::Material(Shader* sh,const std::vector<Texture*> &t) {
    shader = sh; textures = t;
}

Shader* Material::getShader() const {
    return shader;
}

Texture* Material::getTexture(int index) const {
    return textures[index];
}

int Material::getTextureCount() const {
    return textures.size();
}
