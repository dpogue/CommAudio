#include "manager.h"
#include "openal_helper.h"


AudioManager* AudioManager::instance_ = NULL;
bool AudioManager::pause_ = false;
bool AudioManager::stop_ = false;
float AudioManager::musicGain_ = 0.9;
QMutex AudioManager::mutex_;

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
    stop_ = true;
	pause_ = false;
    alSleep(0.25f); //HACK!!
    QFuture<void> future =
        QtConcurrent::run(this, &AudioManager::streamFile, filename);
    return;
}

bool AudioManager::checkCondition()
{
    ALuint error = alGetError();
    const ALchar* err = alGetString(error);

    if (inited_ == false || stop_ == true) {
        return true;
    } else if (error != AL_NO_ERROR) {
        qCritical("AudioManager::checkError(): %d:%s", error, err);
        alExit();
		inited_=false;
        return true;
    }

    return false;
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
    ALenum format = AL_FORMAT_STEREO16;
    ALuint freq = 44100;
    OggVorbis_File oggFile;
    vorbis_info* vorbisInfo;
	fileType fType;

    /* Created the source and Buffers */
    alGenBuffers(QUEUESIZE, buffer);
    alGenSources(1, &source);
    /*set the Gain for Music or Sfx*/
    
	stop_=false;

	if(filename.isEmpty()){
		fType = NETWORK;
	} else if ((file = fopen(filename.toAscii().constData(), "rb")) == NULL) {
        qCritical() << "AudioManager::streamFile(): Cannot open " << filename << " for reading...";
		return;
    }

	if(filename.contains(".ogg") || filename.contains(".oga")) {	
		openOgg(file, &oggFile, &format);
		fType = OGG;
	} else if (filename.contains(".wav")) {
		openWav(&file,&format,&freq);
		fType = WAV;
	}


    do {

		alSourcef(source, AL_GAIN, musicGain_);
		bool paused = false;

		if(pause_== true) {
			alSourcePause(source);
			paused = true;
		} else {
			clearProcessedBuffers(&source, buffersAvailable, &playing, &play);
		}

		while(pause_ == true && inited_ == true) {
			alSleep(0.1f);
		}

		if(paused == true) {
			alSourcePlay(source);
			play = AL_FALSE;
		}

        if (buffersAvailable > 0) {
            size = 0;
            /* Read file to we reached a BUFFERSIZE chunk */
            while (size < BUFFERSIZE) {

                if(fType == OGG) {
					result = ov_read (&oggFile, array + size, 
						BUFFERSIZE - size, 0, 2, 1, &bitStream);
				} else if (fType = WAV) {
					result = fread(array + size, 1, BUFFERSIZE - size, file);
				} else if (fType = NETWORK) {
					if(streamQueue.count() == 0)
						result = 0; break;
					mutex_.lock();
					strcpy(array,streamQueue.dequeue());
					result = sizeof(array);
					mutex_.unlock();
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
            alSleep(0.05f);			
        }

		/* result == 0 when file is completely read */
    } while (result > 0 && !checkCondition());

    if(fType == OGG) {	
		ov_clear(&oggFile);
	}

    cleanUp(&source, &playing, buffer);

}

void AudioManager::cleanUp(ALuint *source, ALint *playing, ALuint *buffer) 
{
	ALint processed;
    ALuint tempBuffer;	

	do {
        alGetSourcei(*source, AL_SOURCE_STATE, playing);
        alSleep(0.1f);
    } while (*playing != AL_STOPPED && !checkCondition());

    alGetSourcei(*source, AL_BUFFERS_PROCESSED, &processed);

    while (processed) {
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
	qDebug("Before opening file");
    if (ov_open(file, oggFile, NULL, 0) != 0) {
        qCritical() << "AudioManager::openOgg(): Error opening file for decoding...";
        return;
    }
    qDebug("after opening file");
    vorbisInfo = ov_info(oggFile, -1);
	qDebug("file info");
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