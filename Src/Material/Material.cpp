#include <Material/Material.h>

using namespace PGE;

Material::Material(Shader* sh,Texture* t) {
    shader = sh; texture = t;
}

Shader* Material::getShader() const {
    return shader;
}

Texture* Material::getTexture() const {
    return texture;
}
