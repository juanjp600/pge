#ifndef PGE_AUDIO_H_INCLUDED
#define PGE_AUDIO_H_INCLUDED

#include <Threading/ThreadManager.h>

namespace PGE {

class Audio {
    public:
        static Audio* create(ThreadManager* threadManager);
        virtual ~Audio(){}

        enum class ERROR_STATE {
            NONE = 0,
            DEVICE_INIT_FAILED,
            CONTEXT_INIT_FAILED,
            CONTEXT_MAKECURRENT_FAILED,
            AL_MISC_CALL_FAILED,
            SOURCE_ALLOC_FAILED
        };
        ERROR_STATE getErrorState() const;

    protected:
        Audio(){}
        Audio(const Audio& other){}
        ERROR_STATE errorState = ERROR_STATE::NONE;
};

}

#endif // PGE_AUDIO_H_INCLUDED
