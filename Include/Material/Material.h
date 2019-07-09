#ifndef PGE_MATERIAL_H_INCLUDED
#define PGE_MATERIAL_H_INCLUDED

#include <Misc/String.h>
#include <Shader/Shader.h>
#include <Texture/Texture.h>

namespace PGE {

class Material {
    public:
        Material(Shader* sh,const std::vector<Texture*> &t,bool opaq = true);
        Material(Shader* sh,Texture* t,bool opaq=true);
        Material(Shader* sh,bool opaq=true);

        Shader* getShader() const;
        Texture* getTexture(int index) const;
        int getTextureCount() const;
        bool isOpaque() const;
    protected:
        Material(){};
        Shader* shader; std::vector<Texture*> textures;
        bool opaque;
};

}

#endif
