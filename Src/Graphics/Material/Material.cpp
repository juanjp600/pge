#include <PGE/Graphics/Material.h>

using namespace PGE;

#include <PGE/Graphics/Shader.h>
#include <PGE/Graphics/Texture.h>

Material::Material(Shader* sh, const std::vector<Texture*>& t, bool opaq) {
    shader = sh; textures = t; opaque = opaq;
}

Material::Material(Shader* sh,Texture* t,bool opaq) {
    shader = sh; textures.clear(); textures.push_back(t); opaque = opaq;
}

Material::Material(Shader* sh,bool opaq) {
    shader = sh; textures.clear(); opaque = opaq;
}

Shader* Material::getShader() const {
    return shader;
}

Texture* Material::getTexture(int index) const {
    return textures[index];
}

int Material::getTextureCount() const {
    return (int)textures.size();
}

bool Material::isOpaque() const {
    return opaque;
}
