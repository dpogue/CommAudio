#include "manager.h"
#include "openal_helper.h"


AudioManager* AudioManager::instance_ = NULL;
bool AudioManager::pause_ = false;
bool AudioManager::stop_ = false;
int AudioManager::musicGain_ = 0.9;
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

	while(pause_ == true && inited_ == true) {
		alSleep(0.1f);			
	}

	if(stop_ == true) {
		return true;				
	}

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

void AudioManager::streamFile(QString filename)
{
    FILE* file;
    char array[BUFFERSIZE];
    int bitStream;
    int buffersAvailable = QUEUESIZE;
    long size = 0;
    long result = 0;
    ALuint buffer[QUEUESIZE];
    ALuint tempBuffer;
    ALuint source;
    ALint processed;
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

    if ((file = fopen(filename.toAscii().constData(), "rb")) == NULL) {
        qCritical() << "AudioManager::streamFile(): Cannot open " << filename << " for reading...";
		return;
    }

	if(filename.contains(".ogg")) {	
		openOgg(file, &oggFile, &format);
		fType = OGG;
	} else {
		openWav(&file,&format,&freq);
		fType = WAV;
	}


    do {
        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

        while (processed) {
            alSourceUnqueueBuffers(source, 1, &tempBuffer);
            processed--;

            /* ensures buffers don't get larger than QUEUESIZE */
            if (buffersAvailable < QUEUESIZE - 1) {
                buffersAvailable++;
            }

            /* Restarts playing if buffer emptied too quickly */
            alGetSourcei(source, AL_SOURCE_STATE, &playing);

            if (playing == AL_STOPPED) {
                play = AL_TRUE;
            }
        }
         
        if (buffersAvailable > 0) {
            size = 0;
            /* Read file to we reached a BUFFERSIZE chunk */
            while (size < BUFFERSIZE) {

                if(fType == OGG) {
					result = ov_read(&oggFile, array + size,
						BUFFERSIZE - size, 0, 2, 1, &bitStream);
				} else {
					
					result = fread(array + size, 1, BUFFERSIZE - size, file);
				}

                if (result == 0) {
                    break;
                }

                size += result;

                if (result < 0) {
                    qCritical() << "AudioManager::streamFile(): Ogg Read Failed ";
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

		alSourcef(source, AL_GAIN, 0.9);

		/* result == 0 when file is completely read */
    } while (result > 0 && !checkCondition() && !stop_);

    if(fType == OGG) {	
		ov_clear(&oggFile);
	}

    /** Wait until sound stops playing before
     *  clearing the buffers and source
     */
    do {
        alGetSourcei(source, AL_SOURCE_STATE, &playing);
        alSleep(0.1f);
    } while (playing != AL_STOPPED && !checkCondition());

    alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

    while (processed) {
        alSourceUnqueueBuffers(source, 1, &tempBuffer);
        processed--;
    }

    mutex_.lock();
    alDeleteSources(1, &source);
    alDeleteBuffers(QUEUESIZE, buffer);
    mutex_.unlock();

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
		  return;
	  }

	  fread(buf, 1, 2, *file);
	  if(buf[1] != 0 || buf[0] != 1) {
		  qCritical("Not PCM");
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
		  return;
	  }

}