#include <Audio/Audio.h>
#include <Sound/Sound.h>
#include "../Audio/AudioInternal.h"
#include "SoundInternal.h"

using namespace PGE;

Sound* Sound::load(Audio* a,const FileName& fn,bool forcePan,bool strm) {
    return new SoundInternal(a,fn,forcePan,strm);
}

SoundInternal::SoundInternal(Audio* a,const FileName& fn,bool forcePan,bool strm) {
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
        f = fopen(fn.cstr(),"rb");
        vorbis_info *vorbisInfo;

        ov_open(f,&oggFile,"",0);
        vorbisInfo = ov_info(&oggFile,-1);
        if (vorbisInfo->channels == 1) {
            stereo = false;
        } else {
            stereo = true;
        }
        frequency = vorbisInfo->rate;
        forcePanning = false;
        if (forcePan && stereo) {
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
            readBytes = ov_read(&oggFile,buf->charBuf,bufferSamples*2,endian,2,1,&bitStream);
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
        ov_clear(&oggFile);

        alBufferData(alBuffer,stereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,allPcm.data(),allPcm.size(),frequency);
    } else {
        FILE* f = nullptr;
        f = fopen(fn.cstr(),"rb");
        vorbis_info *vorbisInfo;

        ov_open(f,&oggFile,"",0);
        vorbisInfo = ov_info(&oggFile,-1);
        if (vorbisInfo->channels == 1) {
            stereo = false;
        } else {
            stereo = true;
        }
        frequency = vorbisInfo->rate;
        bool mergeChannels = false;
        forcePanning = false;
        if (forcePan && stereo) {
            //OpenAL does not perform automatic panning or attenuation with stereo tracks
            stereo = false;
            forcePanning = true;
        }
    }

    ((AudioInternal*)audio)->registerSound(this);
}

SoundInternal::~SoundInternal() {
    ((AudioInternal*)audio)->unregisterSound(this);
    for (int i=channels.size()-1;i>=0;i--) {
        delete channels[i];
    }
    if (alBuffer != 0) {
        alDeleteBuffers(1,&alBuffer);

        if (alGetError()!=AL_NO_ERROR) {
            //do nothing i guess?
        }
    }
    if (stream) {
        ov_clear(&oggFile);
    }
}

bool SoundInternal::isStream() const {
    return stream;
}

bool SoundInternal::isStereo() const {
    return stereo;
}

int SoundInternal::getFrequency() const {
    return frequency;
}

void SoundInternal::fillStreamBuffer(int seekPos,uint8_t* buf,int maxSize,int& outSamples,bool& outEof) {
    if (!stream) {
        outSamples = 0;
        outEof = true;
        return;
    }
    const int bufferSamples = 32768;
    if (maxSize > bufferSamples) { maxSize = bufferSamples; }
    union PCMBuffer {
        int16_t shortBuf[bufferSamples];
        char charBuf[bufferSamples*2];
    };
    PCMBuffer* tempBuf = new PCMBuffer();
    int endian = 0;
    int bitStream;
    int readBytes = 0;
    if (forcePanning) {
        seekPos *= 2;
    }
    seekPos /= 2;
    ov_pcm_seek(&oggFile,seekPos);
    int accumSamples = 0;
    while (accumSamples<maxSize*2) {
        readBytes = ov_read(&oggFile,tempBuf->charBuf,maxSize*2,endian,2,1,&bitStream);
        int readSamples = readBytes/2;
        if (maxSize-accumSamples < readSamples) { break; }
        outEof = readBytes==0;
        if (forcePanning) {
            readSamples/=2;
            for (int i=0;i<readSamples*2;i++) {
                int32_t sample0 = tempBuf->shortBuf[i*2];
                int32_t sample1 = tempBuf->shortBuf[i*2+1];
                int32_t mergedSample = (sample0+sample1)/2;
                tempBuf->shortBuf[i] = mergedSample;
            }
        }
        memcpy(buf+accumSamples*2,tempBuf->charBuf,readBytes);
        accumSamples += readSamples;
        if (outEof) { break; }
    }
    outSamples = accumSamples;
    if (forcePanning) { outSamples /= 2; }
    delete tempBuf;
}

ALuint SoundInternal::getALBuffer() const {
    return alBuffer;
}

Sound::Channel* SoundInternal::play(bool loop) {
    ChannelInternal* newChannel = new ChannelInternal(audio,this,loop);
    for (int i=channels.size()-1;i>=0;i--) {
        if (!channels[i]->isPlaying()) {
            delete channels[i];
        }
    }
    channels.push_back(newChannel);
    return newChannel;
}

void SoundInternal::removeChannel(SoundInternal::ChannelInternal* chn) {
    for (int i=channels.size()-1;i>=0;i--) {
        if (channels[i]==chn) {
            channels.erase(channels.begin()+i);
            ((AudioInternal*)audio)->unregisterSoundChannel(chn);
            break;
        }
    }
}

SoundInternal::ChannelInternal::ChannelInternal(Audio* a,SoundInternal* snd,bool lp) {
    audio = a; sound = snd;
    loop = lp;
    playing = false;
    streamReady = false;
    ((AudioInternal*)audio)->registerSoundChannel(this,alSource);
    if (alSource != 0) {
        alSourcei(alSource, AL_SOURCE_RELATIVE, AL_TRUE);
        alSourcef(alSource, AL_REFERENCE_DISTANCE, 100.f);
        alSourcef(alSource, AL_MAX_DISTANCE, 200.f);
        alSource3f(alSource,AL_POSITION,0.f,0.f,0.f);
        alSourcef(alSource,AL_GAIN,1.f);
        if (!isStream()) {
            alSourcei(alSource,AL_LOOPING,loop);
            alSourcei(alSource,AL_BUFFER,sound->getALBuffer());
            alSourceRewind(alSource);
            alSourcePlay(alSource);
        } else {
            alSourcei(alSource,AL_LOOPING,false);
            streamByteBuf = new uint8_t[streamSampleCount*2];
            streamAlBuffers = new ALuint[4];
            alGenBuffers(4,streamAlBuffers);
            streamMutex = new std::mutex();
            streamSeekPos = -1;
            streamReachedEof = false;
        }
        playing = true;
    }
    streamReady = true;
}

SoundInternal::ChannelInternal::~ChannelInternal() {
    playing = false;
    alSourceStop(alSource);
    if (isStream()) {
        streamMutex->lock();

        delete[] streamByteBuf;

        ALuint unqueuedBuffers[4];
        alSourceUnqueueBuffers(alSource,4,unqueuedBuffers);
        alDeleteBuffers(4,streamAlBuffers);
        delete[] streamAlBuffers;

        streamMutex->unlock();

        delete streamMutex;
    }

    sound->removeChannel(this);
}

bool SoundInternal::ChannelInternal::isPlaying() const {
    bool retVal = playing;
    if (!isStream() && alSource!=0) {
        ALint alSourceState = 0; alGetSourcei(alSource,AL_SOURCE_STATE,&alSourceState);
        retVal &= alSourceState==AL_PLAYING;
    }
    return retVal;
}

bool SoundInternal::ChannelInternal::isStream() const {
    return sound->isStream();
}

bool SoundInternal::ChannelInternal::isStreamReady() const {
    return streamReady;
}

void SoundInternal::ChannelInternal::updateStream() {
    if (!playing) { return; }
    std::lock_guard<std::mutex> lockGuard(*streamMutex);

    ALint alSourceState = 0; alGetSourcei(alSource,AL_SOURCE_STATE,&alSourceState);
    if (streamReachedEof) {
        if (alSourceState != AL_PLAYING) {
            playing  = false;
            return;
        }
    }

    ALint buffersToUnqueue;
    ALuint unqueuedBuffers[4];

    if (streamSeekPos>=0) {
        alGetSourcei(alSource,AL_BUFFERS_PROCESSED,&buffersToUnqueue);

        if (buffersToUnqueue<=0) { return; }

        alSourceUnqueueBuffers(alSource,buffersToUnqueue,unqueuedBuffers);
    } else {
        buffersToUnqueue = 4;
        memcpy(unqueuedBuffers,streamAlBuffers,sizeof(ALuint)*4);
        streamSeekPos = 0;
    }

    for (int i=0;i<buffersToUnqueue;i++) {
        if (streamReachedEof) { break; }
        int outSamples = 0; bool outEof;
        sound->fillStreamBuffer(streamSeekPos,streamByteBuf,streamSampleCount,outSamples,outEof);
        streamSeekPos += outSamples;
        if (outEof) {
            if (loop) {
                streamSeekPos = 0;
            } else {
                streamReachedEof = true;
            }
        }
        if (outSamples <= 0) {
            memset(streamByteBuf,0,streamSampleCount*2);
            outSamples = streamSampleCount;
        }
        alBufferData(unqueuedBuffers[i],sound->isStereo() ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16,streamByteBuf,outSamples*2,sound->getFrequency());
        alSourceQueueBuffers(alSource,1,&(unqueuedBuffers[i]));
        if (alSourceState != AL_PLAYING) {
            alSourcePlay(alSource);
        }
    }
}
