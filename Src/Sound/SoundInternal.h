#ifndef PGEINTERNAL_SOUNDINTERNAL_H_INCLUDED
#define PGEINTERNAL_SOUNDINTERNAL_H_INCLUDED

#ifdef LINUX
#include <AL/al.h>
#elif defined __APPLE__
#include <OpenAL/al.h>
#elif defined WINDOWS
#include <al.h>
#endif

#include <vorbis/vorbisfile.h>
#include <ogg/ogg.h>

#include <Sound/Sound.h>

namespace PGE {

class Audio;

class SoundInternal : public Sound {
    public:
        SoundInternal(Audio* a, const FilePath& fn, bool forcePanning = false, bool strm = false);
        ~SoundInternal();

        bool isStream() const override;
        bool isStereo() const override;
        int getFrequency() const override;
        void fillStreamBuffer(int seekPos, uint8_t* buf, int maxSize, int& outSamples, bool& outEof);
        ALuint getALBuffer() const;

        class ChannelInternal : public Channel {
            public:
                ChannelInternal(Audio* a, SoundInternal* snd, bool lp);
                ~ChannelInternal();

                bool isPlaying() const override;
                bool isStream() const override;
                bool isStreamReady() const override;
                void updateStream() override;

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

        Channel* play(bool loop = false) override;
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

#endif // PGEINTERNAL_SOUNDINTERNAL_H_INCLUDED
