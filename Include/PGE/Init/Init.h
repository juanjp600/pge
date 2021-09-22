#ifndef PGE_INIT_H_INCLUDED
#define PGE_INIT_H_INCLUDED

#include <vector>

#include <PGE/String/String.h>

namespace PGE {

namespace Init {
    void init();
    void quit();

    int main(const std::vector<String>& args);
}

}

#endif // PGE_INIT_H_INCLUDED
