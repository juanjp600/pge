#ifndef PGEINTERNAL_AUDIOINTERNAL_H_INCLUDED
#define PGEINTERNAL_AUDIOINTERNAL_H_INCLUDED

#include <vector>

#ifdef LINUX
#include <AL/al.h>
#include <AL/alc.h>
#elif defined __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#elif defined WINDOWS
#include <al.h>
#include <alc.h>
#endif

#include <Audio/Audio.h>
#include <Sound/Sound.h>
#include "../Sound/SoundInternal.h"

namespace PGE {

class AudioInternal : public Audio {
    public:
        AudioInternal(ThreadManager* threadMgr);
        ~AudioInternal();

        void registerSound(SoundInternal* snd);
        void unregisterSound(SoundInternal* snd);
        bool registerSoundChannel(SoundInternal::ChannelInternal* chn,ALuint& alSource);
        void unregisterSoundChannel(SoundInternal::ChannelInternal* chn);
        bool updateStreamThread();
    private:
        static const int CHANNEL_COUNT = 16;

        ThreadManager* threadManager;

        class AudioThreadRequest : public ThreadManager::NewThreadRequest {
            public:
                AudioThreadRequest(AudioInternal* a);
                void execute();
                void abort();
            private:
                AudioInternal* audio;
                static const int SLEEP_MS = 100;
                std::atomic<bool> abortRequested;
        };
        AudioThreadRequest* audioThreadRequest;
        std::mutex audioThreadMutex;
        void initStreamThread();

        ALCdevice* alcDevice;
        ALCcontext* alcContext;

        std::vector<SoundInternal*> loadedSounds;
        Sound::Channel* playingChannels[CHANNEL_COUNT];
        ALuint alSources[CHANNEL_COUNT];
};

}

#endif // PGEINTERNAL_AUDIOINTERNAL_H_INCLUDED
