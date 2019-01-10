#ifndef PGE_SHADER_H_INCLUDED
#define PGE_SHADER_H_INCLUDED

#include <Graphics/Graphics.h>
#include <Misc/String.h>

namespace PGE {

class Shader {
    public:
        virtual ~Shader(){};
    protected:
        Shader(){};
        String filepath;
};

}

#endif
