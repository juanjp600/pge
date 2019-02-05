#include <Material/Material.h>

using namespace PGE;

Material::Material(Shader* sh,const std::vector<Texture*> &t) {
    shader = sh; textures = t;
}

Material::Material(Shader* sh,Texture* t) {
    shader = sh; textures.clear(); textures.push_back(t);
}

Material::Material(Shader* sh) {
    shader = sh; textures.clear();
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

bool Material::isOpaque() const {
    bool isOpaque = true;
    for (int i = 0; i < getTextureCount(); i++) {
        isOpaque &= getTexture(i)->isOpaque();
        if (!isOpaque) {
            break;
        }
    }
    return isOpaque;
}
