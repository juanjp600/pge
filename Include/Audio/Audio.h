#ifndef PGE_AUDIO_H_INCLUDED
#define PGE_AUDIO_H_INCLUDED

#include <Sound/Sound.h>
#include <Threading/ThreadManager.h>
#include <vector>
#include <atomic>
#include <al.h>
#include <alc.h>

namespace PGE {

class Audio {
    public:
        Audio(ThreadManager* threadMgr);
        ~Audio();

        enum class ERROR_STATE {
            NONE = 0,
            DEVICE_INIT_FAILED,
            CONTEXT_INIT_FAILED,
            CONTEXT_MAKECURRENT_FAILED,
            AL_MISC_CALL_FAILED,
            SOURCE_ALLOC_FAILED
        };
        ERROR_STATE getErrorState() const;

        void registerSound(Sound* snd);
        void unregisterSound(Sound* snd);
        bool registerSoundChannel(Sound::Channel* chn,ALuint& alSource);
        void unregisterSoundChannel(Sound::Channel* chn);
        bool updateStreamThread();
    private:
        ERROR_STATE errorState = ERROR_STATE::NONE;

        static const int CHANNEL_COUNT = 16;

        ThreadManager* threadManager;

        class AudioThreadRequest : public ThreadManager::NewThreadRequest {
            public:
                AudioThreadRequest(Audio* a);
                void execute();
                void abort();
            private:
                Audio* audio;
                static const int SLEEP_MS = 100;
                std::atomic<bool> abortRequested;
        };
        AudioThreadRequest* audioThreadRequest;
        std::mutex audioThreadMutex;
        void initStreamThread();
        
        ALCdevice* alcDevice;
        ALCcontext* alcContext;

        std::vector<Sound*> loadedSounds;
        Sound::Channel* playingChannels[CHANNEL_COUNT];
        ALuint alSources[CHANNEL_COUNT];
};

}

#endif
