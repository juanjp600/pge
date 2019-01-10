#ifndef PGE_MATERIAL_H_INCLUDED
#define PGE_MATERIAL_H_INCLUDED

#include <Misc/String.h>
#include <Shader/Shader.h>
#include <Texture/Texture.h>

namespace PGE {

struct Material {
    public:
        Material(Shader* sh,Texture* t);

        Shader* getShader() const;
        Texture* getTexture() const;
    protected:
        Material(){};
        Shader* shader; Texture* texture;
};

}

#endif
