#include <Audio/Audio.h>
#include <Sound/Sound.h>

#include <al.h>
#include <vorbis/vorbisfile.h>
#include <ogg/ogg.h>

using namespace PGE;

Sound::Sound(Audio* a,const String& fn,bool forcePanning,bool strm) {
    audio = a;
    stream = strm;
    alBuffer = 0;
    stereo = false;

    if (audio->getErrorState()!=Audio::ERROR_STATE::NONE) { return; }

    if (!stream) {
        alGenBuffers(1,&alBuffer);
        if (alGetError()!=AL_NO_ERROR) {
            alBuffer = 0;
            return;
        }
        
        FILE* f = nullptr;
        fopen_s(&f,fn.cstr(),"rb");
        vorbis_info *vorbisInfo;
        OggVorbis_File oggfile;

        ov_open(f,&oggfile,"",0);
        vorbisInfo = ov_info(&oggfile,-1);
        if (vorbisInfo->channels == 1) {
            stereo = false;
        } else {
            stereo = true;
        }
        frequency = vorbisInfo->rate;
        bool mergeChannels = false;
        if (forcePanning && stereo) {
            //OpenAL does not perform automatic panning or attenuation with stereo tracks
            stereo = false;
            forcePanning = true;
        }

        const int bufferSamples = 32768;
        union PCMBuffer {
            int16_t shortBuf[bufferSamples];
            char charBuf[bufferSamples*2];
        };
        PCMBuffer* buf = new PCMBuffer();
        std::vector<char> allPcm;
        int endian = 0;
        int bitStream;
        int readBytes = 0;
        do {
            readBytes = ov_read(&oggfile,buf->charBuf,bufferSamples*2,endian,2,1,&bitStream);
            if (forcePanning) {
                for (int i=0;i<bufferSamples/2;i++) {
                    int32_t sample0 = buf->shortBuf[i*2];
                    int32_t sample1 = buf->shortBuf[i*2+1];
                    int32_t mergedSample = (sample0+sample1)/2;
                    buf->shortBuf[i] = mergedSample;
                }
                allPcm.insert(allPcm.end(),buf->charBuf,buf->charBuf+(readBytes/2));
            } else {
                allPcm.insert(allPcm.end(),buf->charBuf,buf->charBuf+readBytes);
            }
        } while (readBytes>0);
        delete buf;
        ov_clear(&oggfile);

        alBufferData(alBuffer,stereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,allPcm.data(),allPcm.size(),frequency);
    }

    audio->registerSound(this);
}

Sound::~Sound() {
    audio->unregisterSound(this);
    for (int i=channels.size()-1;i>=0;i--) {
        delete channels[i];
    }
    if (alBuffer != 0) {
        alDeleteBuffers(1,&alBuffer);

        if (alGetError()!=AL_NO_ERROR) {
            //do nothing i guess?
        }
    }
}

bool Sound::isStream() const {
    return stream;
}

int Sound::fillStreamBuffer(int seekPos,uint8_t* buf,int maxSize) {
    //TODO: implement
    return 0;
}

ALuint Sound::getALBuffer() const {
    return alBuffer;
}

Sound::Channel* Sound::play(bool loop) {
    Channel* newChannel = new Channel(audio,this,loop);
    for (int i=channels.size()-1;i>=0;i--) {
        if (!channels[i]->isPlaying()) {
            delete channels[i];
        }
    }
    channels.push_back(newChannel);
    return newChannel;
}

void Sound::removeChannel(Sound::Channel* chn) {
    for (int i=channels.size()-1;i>=0;i--) {
        if (channels[i]==chn) {
            channels.erase(channels.begin()+i);
            audio->unregisterSoundChannel(chn);
            break;
        }
    }
}

Sound::Channel::Channel(Audio* a,Sound* snd,bool loop) {
    audio = a; sound = snd;
    audio->registerSoundChannel(this,alSource);
    if (alSource == 0) {
        playing = false;
    } else {
        playing = true;
        alSourcei(alSource,AL_BUFFER,sound->getALBuffer());
        alSourcei(alSource, AL_SOURCE_RELATIVE, AL_TRUE);
        alSourcef(alSource, AL_REFERENCE_DISTANCE, 100.f);
        alSourcef(alSource, AL_MAX_DISTANCE, 200.f);
        alSource3f(alSource,AL_POSITION,0.f,0.f,0.f);
        alSourcei(alSource,AL_LOOPING,loop);
        alSourcef(alSource,AL_GAIN,1.f);
        alSourceRewind(alSource);
        alSourcePlay(alSource);
    }
}

Sound::Channel::~Channel() {
    playing = false;
    alSourceStop(alSource);
    sound->removeChannel(this);
}

bool Sound::Channel::isPlaying() const {
    bool retVal = playing;
    if (!isStream() && alSource!=0) {
        ALint alSourceState = 0; alGetSourcei(alSource,AL_SOURCE_STATE,&alSourceState);
        retVal &= alSourceState==AL_PLAYING;
    }
    return retVal;
}

bool Sound::Channel::isStream() const {
    return sound->isStream();
}

void Sound::Channel::updateStream() {
    //TODO: implement
}
