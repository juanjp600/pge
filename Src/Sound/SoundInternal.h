#ifndef PGEINTERNAL_SOUNDINTERNAL_H_INCLUDED
#define PGEINTERNAL_SOUNDINTERNAL_H_INCLUDED

#include <Sound/Sound.h>

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
class SoundInternal : public Sound {
    public:
        SoundInternal(Audio* a,const FileName& fn,bool forcePanning=false,bool strm=false);
        ~SoundInternal();

        bool isStream() const;
        bool isStereo() const;
        int getFrequency() const;
        void fillStreamBuffer(int seekPos,uint8_t* buf,int maxSize,int& outSamples,bool& outEof);
        ALuint getALBuffer() const;

        class ChannelInternal : public Channel {
            public:
                ChannelInternal(Audio* a,SoundInternal* snd,bool lp);
                ~ChannelInternal();

                virtual bool isPlaying() const;
                virtual bool isStream() const;
                virtual bool isStreamReady() const;
                virtual void updateStream();
            private:
                Audio* audio;
                SoundInternal* sound;
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

        virtual Channel* play(bool loop=false);
        void removeChannel(ChannelInternal* channel);
    private:
        std::vector<ChannelInternal*> channels;
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
