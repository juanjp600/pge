#ifndef PGE_SOUND_H_INCLUDED
#define PGE_SOUND_H_INCLUDED

#include <Misc/String.h>

namespace PGE {

class Audio;
class Sound {
    public:
        static Sound* load(Audio* a,const String& fn,bool forcePan=false,bool strm=false);
        virtual ~Sound(){}

        virtual bool isStream() const =0;
        virtual bool isStereo() const =0;
        virtual int getFrequency() const =0;
        
        class Channel {
            public:
                virtual ~Channel(){}

                virtual bool isPlaying() const =0;
                virtual bool isStream() const =0;
                virtual bool isStreamReady() const =0;
                virtual void updateStream() =0;
        };

        virtual Channel* play(bool loop=false) =0;
};

}

#endif
