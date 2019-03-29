#ifndef PGE_SOUND_H_INCLUDED
#define PGE_SOUND_H_INCLUDED

#include <atomic>
#include <mutex>
#include <vector>

#include <Misc/String.h>

#ifdef LINUX
#include <AL/al.h>
#elif defined __APPLE__
#include <OpenAL/al.h>
#elif defined WINDOWS
#include <al.h>
#endif
#include <vorbis/vorbisfile.h>
#include <ogg/ogg.h>

namespace PGE {

class Audio;
class Sound {
    public:
        Sound(Audio* a,const String& fn,bool forcePanning=false,bool strm=false);
        ~Sound();

        bool isStream() const;
        bool isStereo() const;
        int getFrequency() const;
        void fillStreamBuffer(int seekPos,uint8_t* buf,int maxSize,int& outSamples,bool& outEof);
        ALuint getALBuffer() const;

        class Channel {
            public:
                Channel(Audio* a,Sound* snd,bool lp);
                ~Channel();

                bool isPlaying() const;
                bool isStream() const;
                bool isStreamReady() const;
                void updateStream();
            private:
                Audio* audio;
                Sound* sound;
                ALuint alSource;
                bool loop;
                std::atomic<bool> playing;

                ALuint* streamAlBuffers;
                std::mutex* streamMutex;
                const int streamSampleCount = 8192;
                uint8_t* streamByteBuf;
                int streamSeekPos;
                bool streamReachedEof;
                std::atomic<bool> streamReady;
        };

        Channel* play(bool loop=false);
        void removeChannel(Channel* channel);
    private:
        std::vector<Channel*> channels;
        int frequency;
        Audio* audio;
        bool forcePanning;
        bool stereo;
        bool stream;
        OggVorbis_File oggFile;
        ALuint alBuffer;
};

}

#endif
