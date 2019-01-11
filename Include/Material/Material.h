#ifndef PGE_MATERIAL_H_INCLUDED
#define PGE_MATERIAL_H_INCLUDED

#include <Misc/String.h>
#include <Shader/Shader.h>
#include <Texture/Texture.h>

namespace PGE {

struct Material {
    public:
        Material(Shader* sh,const std::vector<Texture*> &t);

        Shader* getShader() const;
        Texture* getTexture(int index) const;
        int getTextureCount() const;
    protected:
        Material(){};
        Shader* shader; std::vector<Texture*> textures;
};

}

#endif
