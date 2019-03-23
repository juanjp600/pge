#ifndef PGE_SOUND_H_INCLUDED
#define PGE_SOUND_H_INCLUDED

#include <atomic>

#include <Misc/String.h>
#include <al.h>

namespace PGE {

class Audio;
class Sound {
    public:
        Sound(Audio* a,const String& fn,bool forcePanning=false,bool strm=false);
        ~Sound();

        bool isStream() const;
        int fillStreamBuffer(int seekPos,uint8_t* buf,int maxSize);
        ALuint getALBuffer() const;

        class Channel {
            public:
                Channel(Audio* a,Sound* snd,bool loop);
                ~Channel();

                bool isPlaying() const;
                bool isStream() const;
                void updateStream();
            private:
                Audio* audio;
                Sound* sound;
                ALuint alSource;

                std::atomic<bool> playing;
        };

        Channel* play(bool loop=false);
        void removeChannel(Channel* channel);
    private:
        std::vector<Channel*> channels;
        int frequency;
        Audio* audio;
        bool stereo;
        bool stream;
        ALuint alBuffer;
};

}

#endif
