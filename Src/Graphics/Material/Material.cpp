#include <PGE/Graphics/Material.h>

#include <PGE/Graphics/Graphics.h>
#include <PGE/Graphics/Shader.h>

using namespace PGE;

Material::Material(Graphics&, Shader& sh, const ReferenceVector<Texture>& texs, Opaque o) {
    asrt(sh.getTextureCount() == texs.size(), "Mismatched texture count");

    shader = &sh;
    textures = texs;
    opaque = o;
}

bool Material::isOpaque() const {
    return opaque == Opaque::YES;
}

Shader& Material::getShader() const {
    return *shader;
}

int Material::getTextureCount() const {
    return (int)textures.size();
}

const Texture& Material::getTexture(int index) const {
    asrt(index >= 0 && index < getTextureCount(), "Texture index out of bounds");
    return textures.at(index);
}
