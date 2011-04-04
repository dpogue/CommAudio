#include "manager.h"
#include "openal_helper.h"


AudioManager* AudioManager::instance_ = NULL;
bool AudioManager::pause_ = false;
bool AudioManager::stop_ = false;
bool AudioManager::capturePause_ = true;
bool AudioManager::captureStop_ = false;
int AudioManager::playCount_ = 0;
QString AudioManager::nextplay_; 
bool AudioManager::multicast_ = false;
float AudioManager::musicGain_ = 0.5;
QMutex AudioManager::mutex_;
QQueue<QByteArray> AudioManager::streamQueue;
QQueue<QByteArray> AudioManager::netQueue;

AudioManager::AudioManager()
{
    AudioManager::startup();
}

AudioManager::~AudioManager()
{
    AudioManager::shutdown();
}

void AudioManager::shutdown()
{
    if (inited_) {
        inited_ = false;
		stop_ = true;
		captureStop_ = true;
        alExit();
    }
}

void AudioManager::startup()
{
    alInit();
    inited_ = true;
}

void AudioManager::playMusic(QString filename)
{
    
    toggleStop();
    if(getPause() == true) { 
		togglePause();
	}
    setNextPlaying(filename);
    QFuture<void> future =
        QtConcurrent::run(this, &AudioManager::streamFile, filename);
}

void AudioManager::playStream()
{
    QFuture<void> future =
        QtConcurrent::run(this, &AudioManager::streamStream);
}

void AudioManager::startCapture()
{
    QFuture<void> future =
        QtConcurrent::run(this, &AudioManager::captureMic);
}

bool AudioManager::checkCondition()
{
	if(getStop() == true || captureStop_ == true) {
		return true;
	}

	return checkError();
}

bool AudioManager::checkError()
{
    ALuint error = alGetError();
    const ALchar* err = alGetString(error);

    if (inited_ == false) {
        return true;
    } else if (error != AL_NO_ERROR) {
        qCritical("AudioManager::checkError(): %d:%s", error, err);
        alExit();
		inited_=false;
        return true;
    }

    return false;
}

void AudioManager::getSpecs(char bitmask, ALenum *format, ALuint *freq) {

    if(bitmask & 0x10) {
        *freq = 22050;
    } else if (bitmask & 0x20) {
        *freq = 44100;
    } else if (bitmask & 0x40) {
        *freq = 48000;
    }
    
    bitmask &= 0xF;

    if (bitmask == 0x5) {
        *format = AL_FORMAT_MONO8;
    } else if (bitmask == 0x6) {
        *format = AL_FORMAT_MONO16;
    } else if (bitmask == 0x9) {
        *format = AL_FORMAT_STEREO8;
    } else if (bitmask == 0xA) {
        *format = AL_FORMAT_STEREO16;
    }

}

char AudioManager::getBitmask(ALenum format, ALuint freq) {
    
    char bitmask = 0;
    
    switch(freq)
    {
    case 22050:
        bitmask |= 0x10;
        break;
    case 44100:
        bitmask |= 0x20;
        break;
    case 48000:
        bitmask |= 0x40;
        break;
    }

    if(format == AL_FORMAT_MONO8) {
        bitmask |= 0x5;
    } else if(format == AL_FORMAT_MONO16) {
        bitmask |= 0x6;
    } else if(format == AL_FORMAT_STEREO8) {
        bitmask |= 0x9;
    } else if(format == AL_FORMAT_STEREO16) {
        bitmask |= 0xA;
    }

    return bitmask;
}

void AudioManager::streamStream()
{
    char array[BUFFERSIZE];
	QByteArray temp;
    int bitStream;
    int buffersAvailable = QUEUESIZE;
    long size = 0;
    long result = 0;	
    ALuint buffer[QUEUESIZE];
    ALuint source;
    ALint queued = 0;
    ALint queue = 0;
    ALint play = AL_TRUE;
    ALint playing = AL_TRUE;
    char bitmask = 0, oldbmask = 0;
    ALenum format = AL_FORMAT_MONO8;
    ALuint freq = 22050;

    /* Created the source and Buffers */
    alGenBuffers(QUEUESIZE, buffer);
    alGenSources(1, &source);

    do {

		alSourcef(source, AL_GAIN, musicGain_);
		clearProcessedBuffers(&source, buffersAvailable, &playing, &play);

        if (buffersAvailable > 0) {
            size = 0;
            /* Read file to we reached a BUFFERSIZE chunk */
            while (size < BUFFERSIZE && !checkError()) {
               
				while(getQueueSize() < 1 && !checkError()) {
					alSleep(0.1f);
				}
				
				if(checkError()) {
					break;
				}
				
				temp = getNextInQueue();
				if(temp.data() != NULL) {
					memcpy(array,temp.mid(1),BUFFERSIZE);
                    bitmask = temp.at(0);
                }
				result = sizeof(array);
				

                if (result == 0) {
                    break;
                }

                size += result;

                if (result < 0) {
                    qCritical() << "AudioManager::streamFile(): Read Failed ";
                    break;
                }
            }

            if(bitmask != oldbmask){
                do {
                    clearProcessedBuffers(&source, buffersAvailable, &playing, &play);
                    alSleep(0.1f);
                    alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);
                } while(queued > 0);
            }

            oldbmask = bitmask;
            getSpecs(bitmask,&format,&freq);
            qDebug("specs %d %d %d",bitmask,freq, format);
            alBufferData(buffer[queue], format, array, size, freq);
            alSourceQueueBuffers(source, 1, &buffer[queue]);
            queue = (++queue == QUEUESIZE ? 0 : queue);
            buffersAvailable--;

            /**Check the amount of buffers queued to see if
            * we should be playing the track right now.
            * If play is false it means it's playing already
            */
            alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);

            if (queued > 2 && play) {
				alSourcePlay(source);
                play = AL_FALSE;
            }
        } else {
            alSleep(0.1f);			
        }

		/* result == 0 when file is completely read */
    } while (result > 0 && !checkError());

    cleanUp(&source, buffer);		

}

void AudioManager::streamFile(QString filename)
{
    FILE* file;
    char array[BUFFERSIZE];
    int bitStream;
    int buffersAvailable = QUEUESIZE;
    long size = 0;
    long result = 0;
    ALuint buffer[QUEUESIZE];
    ALuint source;
    ALint queued = 0;
    ALint queue = 0;
    ALint play = AL_TRUE;
    ALint playing = AL_TRUE;
    ALenum format = AL_FORMAT_MONO8;
    ALuint freq = 22050;
    OggVorbis_File oggFile;
    vorbis_info* vorbisInfo;
	fileType fType;
    char bitmask;

    while(getPlayCount() > 0) {
		alSleep(0.1f);
        if(checkError() || nextplay_.compare(filename) != 0) {
			return;
		}
	}	

    /* Created the source and Buffers */
    alGenBuffers(QUEUESIZE, buffer);
    alGenSources(1, &source);

    mutex_.lock();	
	playCount_++;
    mutex_.unlock();

	if(getStop() == true) {
		toggleStop();
	}	

	if ((file = fopen(filename.toAscii().constData(), "rb")) == NULL) {
        qCritical() << "AudioManager::streamFile(): Cannot open " << filename << " for reading...";
		return;
    }

	if(filename.contains(".ogg") || filename.contains(".oga")) {	
		openOgg(file, &oggFile, &format);
        freq = 44100;		
		fType = OGG;
	} else if (filename.contains(".wav")) {
		openWav(&file,&format,&freq);
		fType = WAV;
	}

    bitmask = getBitmask(format,freq);

    do {

		alSourcef(source, AL_GAIN, musicGain_);
		bool paused = false;

		if(getPause()== true) {
			alSourcePause(source);
			paused = true;
		} else {
			clearProcessedBuffers(&source, buffersAvailable, &playing, &play);
		}

		while(getPause() == true && getStop() == false) {
			alSleep(0.1f);
		}

		if(getStop() == true) {
			break;
		} else if(paused == true) {
			alSourcePlay(source);
			play = AL_FALSE;
		}

        if (buffersAvailable > 0) {
            size = 0;
            /* Read file to we reached a BUFFERSIZE chunk */
            while (size < BUFFERSIZE && !checkCondition()) {
                if(fType == OGG) {
					result = ov_read (&oggFile, array + size, 
						BUFFERSIZE - size, 0, 2, 1, &bitStream);
				} else if (fType == WAV) {
					result = fread(array + size, 1, BUFFERSIZE - size, file);
				} 

                if (result == 0) {
                    break;
                }

                size += result;

                if (result < 0) {
                    qCritical() << "AudioManager::streamFile(): Read Failed ";
                    break;
                }
            }
            
            if(multicast_ == true) {
                addToNetworkQueue(bitmask, array, size);
            }
           
            alBufferData(buffer[queue], format, array, size, freq);
            alSourceQueueBuffers(source, 1, &buffer[queue]);
            queue = (++queue == QUEUESIZE ? 0 : queue);
            buffersAvailable--;
            /**Check the amount of buffers queued to see if
            * we should be playing the track right now.
            * If play is false it means it's playing already
            */
            alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);

            if (queued > 2 && play) {
				alSourcePlay(source);
                play = AL_FALSE;				
            }
        } else {
            alSleep(0.1f);			
        }

		/* result == 0 when file is completely read */
    } while (result > 0 && !checkCondition());

    if(fType == OGG) {	
		ov_clear(&oggFile);
	}

    cleanUp(&source, buffer);
	mutex_.lock();
	playCount_--;
    mutex_.unlock();
}

void AudioManager::captureMic()
{
    ALCdevice* captureDevice;
    ALint samplesAvailable;
    ALchar buffer[BUFFERSIZE+8];
    ALuint unqueueCount, queueCount;
    ALint format, frequency;
    char bitmask;

	format = AL_FORMAT_MONO8;
	frequency = 22050;
    bitmask = getBitmask(format,frequency);

    captureDevice = alcCaptureOpenDevice(NULL, frequency, format, frequency);

    if (captureDevice) {
        alcCaptureStart(captureDevice);

        while(!checkError() && getCaptureStop() != true) {
			alcGetIntegerv(captureDevice, ALC_CAPTURE_SAMPLES, 1, &samplesAvailable);
			
			while(getCapturePause() == true && getCaptureStop() == false) {
				alSleep(0.1f);
			}

			if(getCaptureStop() == true) {
				break;
			} else if (samplesAvailable > (BUFFERSIZE)) {
                alcCaptureSamples(captureDevice, buffer, BUFFERSIZE);
                addToNetworkQueue(bitmask, buffer, BUFFERSIZE);
			}
		}
	}

    alcCaptureCloseDevice(captureDevice);
}

void AudioManager::cleanUp(ALuint *source, ALuint *buffer) 
{
	ALint processed;
    ALuint tempBuffer;
    ALint playing;	

	do {
        alGetSourcei(*source, AL_SOURCE_STATE, &playing);
        alSleep(0.1f);
    } while (playing != AL_STOPPED && !checkCondition());

    alGetSourcei(*source, AL_BUFFERS_PROCESSED, &processed);
	
	if(getStop() != true) {	
		emit finished();
	}

    while (processed && !checkCondition()) {
        alSourceUnqueueBuffers(*source, 1, &tempBuffer);
        processed--;
	}

    alDeleteSources(1, source);
    alDeleteBuffers(QUEUESIZE, buffer);

}

void AudioManager::clearProcessedBuffers
	(ALuint *source, int &buffersAvailable, ALint *playing, ALint* play)
{
	ALint processed;
    ALuint tempBuffer;	

	alGetSourcei(*source, AL_BUFFERS_PROCESSED, &processed);

	while (processed) {
		alSourceUnqueueBuffers(*source, 1, &tempBuffer);
		processed--;

		/* ensures buffers don't get larger than QUEUESIZE */
		if (buffersAvailable < QUEUESIZE - 1) {
			buffersAvailable++;
		}

		/* Restarts playing if buffer emptied too quickly */
		alGetSourcei(*source, AL_SOURCE_STATE, playing);

		if (*playing == AL_STOPPED) {
			*play = AL_TRUE;
		}
	}
}

void AudioManager::openOgg(FILE *file, OggVorbis_File *oggFile, ALenum *format) 
{

	vorbis_info* vorbisInfo;
    if (ov_open(file, oggFile, NULL, 0) != 0) {
        qCritical() << "AudioManager::openOgg(): Error opening file for decoding...";
        return;
    }

    vorbisInfo = ov_info(oggFile, -1);

    if(vorbisInfo->channels == 1) {
        *format = AL_FORMAT_MONO16;
    }
    else {
        *format = AL_FORMAT_STEREO16;
    }

}

void AudioManager::openWav(FILE **file, ALenum *format, ALuint *frequency)
{
      char* buf = (char *)malloc(sizeof(BUFFERSIZE));
	  int channels, bits;
	  fread(buf,1,12,*file);
 
	  fread(buf,1,8,*file);
	  if(buf[0] != 'f' || buf[1] != 'm' || buf[2] != 't' || buf[3] != ' ') {
		  qCritical("No FMT header");
		  alExit();
		  return;
	  }

	  fread(buf, 1, 2, *file);
	  if(buf[1] != 0 || buf[0] != 1) {
		  qCritical("Not PCM");
		  alExit();
		  return;
	  }

	  fread(buf, 1, 2, *file);
	  channels  = buf[1]<<8;
	  channels |= buf[0];

	  fread(buf, 1, 4, *file);
	  *frequency = *((unsigned int *)buf);

	  fread(buf, 1, 6, *file);

	  fread(buf, 1, 2, *file);
	  bits  = buf[1]<<8;
	  bits |= buf[0];

	  if(bits == 8)
	  {
		  if(channels == 1)
			  *format = AL_FORMAT_MONO8;
		  else if(channels == 2)
			  *format = AL_FORMAT_STEREO8;
	  }
	  else if(bits == 16)
	  {
		  if(channels == 1)
			  *format = AL_FORMAT_MONO16;
		  else if(channels == 2)
			  *format = AL_FORMAT_STEREO16;
	  }

	  fread(buf, 1, 8, *file);
	  if(buf[0] != 'd' || buf[1] != 'a' || buf[2] != 't' || buf[3] != 'a') {
		  qCritical("Not 'data'");
		  alExit();
	  }

}
