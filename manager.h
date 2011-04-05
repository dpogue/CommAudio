#ifndef _manager_
#define _manager_

#include <QMutex>
#include <QObject>
#include <QString>
#include <QThread>
#include <QtDebug>
#include <QtConcurrentRun>
#include <QQueue>
#include <QDir>
#include <vorbis/vorbisfile.h>
#include <errno.h>
#include <sys/types.h>
#include "openal_helper.h"

#define QUEUESIZE 8
#define BUFFERSIZE (1024*16)

enum fileType{ 
OGG,
WAV
};


class AudioManager : public QObject {
    Q_OBJECT;

private:
    /**
     * The static singleton instance of the AudioManager.
     *
     * It should only be retrieved using the instance() method, which
     * will initialize it the first time that it is retrieved.
     */
    static AudioManager* instance_;

    static QMutex mutex_;
    static int playCount_;	

    static bool pause_;
    static bool stop_;	
	static bool capturePause_;
	static bool captureStop_;
    static QString nextplay_; 
    static bool multicast_;

    /**
     * The volume/gain of the background music.
     */
    static float musicGain_;


	static QQueue<QByteArray> streamQueue;
	static QQueue<QByteArray> netQueue;

    /**
     * Whether the AudioManager has been initialized.
     *
     * In practice, this should only be used when checking whether OpenAL is
     * initialized. Code that actually deals with OpenAL should call the
     * checkError() method instead of using this value!
     */
    bool inited_;

private:
    explicit AudioManager();
    ~AudioManager();
    
    /**
     * Does a check for stop flags
     * and errors
     * @author Terence Stenvold
     * @return bool if True and 
     */
    bool checkCondition();

	/**
     * Does a check for openal errors and destroys the openal 
     * context if any are found
     * 
     * @author Terence Stenvold
     * @return bool if True and error occured
     */
	bool checkError();

    /**
     * Streams and plays an audio File.
     *
     * This is meant to be called in it's own thread.
     *
     * @author Terence Stenvold
     * @param filename the path to file.
     */
    void streamFile(QString filename);

    /**
     * Streams and plays an audio from the queue.
     *
     * This is meant to be called in it's own thread.
     *
     * @author Terence Stenvold
     */
	void streamStream();
    char getBitmask(ALenum format, ALuint freq);
    void getSpecs(char bitmask, ALenum *format, ALuint *freq);
	void cleanUp(ALuint *source, ALuint *buffer);
	void clearProcessedBuffers
		(ALuint *source, int &buffersAvailable, ALint *playing, ALint* play);
	void openOgg(FILE *file, OggVorbis_File *oggFile, ALenum *format);
	void openWav(FILE **file, ALenum *format, ALuint *frequency);

public:

    /**
     * Return the instance of the AudioManager.
     * This creates a new instance the first time that it is called and 
     * always returns that instance when called further in the program's 
     * execution.
     *
     * @author Darryl Pogue
     * @return A pointer to the AudioManager instance.
     */
    static AudioManager* instance() {
        mutex_.lock();
        if (instance_ == NULL) {
            instance_ = new AudioManager();
        }
        mutex_.unlock();

        return instance_;
    }

	static void togglePause() {
		mutex_.lock();
		pause_ = !pause_;
		mutex_.unlock();
	}

	static void toggleStop() {
		mutex_.lock();
		stop_ = !stop_;
		mutex_.unlock();
	}

	static bool getPause() {
        bool temp;		
		mutex_.lock();
		temp = pause_;
		mutex_.unlock();
		return temp;
	}

	static bool getStop() {
        bool temp;		
		mutex_.lock();
		temp = stop_;
		mutex_.unlock();
		return temp;
	}

	static bool getCapturePause() {
        bool temp;		
		mutex_.lock();
		temp = capturePause_;
		mutex_.unlock();
		return temp;
	}

	static bool getCaptureStop() {
        bool temp;		
		mutex_.lock();
		temp = captureStop_;
		mutex_.unlock();
		return temp;
	}

	static int getPlayCount() {
        int temp;		
		mutex_.lock();
		temp = playCount_;
		mutex_.unlock();
		return temp;
	}

	static void toggleCapturePause() {
		mutex_.lock();
		capturePause_ = !capturePause_;
		mutex_.unlock();
	}

	static void toggleCaptureStop() {
		mutex_.lock();
		captureStop_ = !captureStop_;
		mutex_.unlock();
	}

    static void setMulticast(bool mcast) {
        mutex_.lock();
        multicast_ = mcast;
        mutex_.unlock();
    }

	static void setGain(float vol) {
		mutex_.lock();
		musicGain_ = vol;
		mutex_.unlock();
	}

	static void addToQueue(QByteArray buffer) {
		mutex_.lock();
		streamQueue.enqueue(buffer);
		mutex_.unlock();
	}

	static void addToNetworkQueue(char bitmask, char *buffer, int size) {
		QByteArray temp(buffer,size);
        mutex_.lock();
        temp.prepend(bitmask);
        temp = qCompress(temp,9);
        netQueue.enqueue(temp); 
		mutex_.unlock();
	}	

    static int getQueueSize() {
		int temp;
		mutex_.lock();
		temp = streamQueue.count();
		mutex_.unlock();
		return temp;
	}

	static QByteArray getNextInQueue() {
		QByteArray temp = NULL;
		mutex_.lock();
        if(streamQueue.count() > 0) {		
			temp = streamQueue.dequeue();
		}
		mutex_.unlock();
		return qUncompress(temp);
	}

	static QByteArray* getNextNetworkQueue() {
		QByteArray* temp = NULL;
		mutex_.lock();
        if(netQueue.count() > 0) {		
			temp = new QByteArray(netQueue.dequeue());
		}
		mutex_.unlock();
		return temp;
	}

    static void setNextPlaying(QString next) {
        mutex_.lock();
        nextplay_ = next;
        mutex_.unlock();
    }

	void startCapture();
	void captureMic();
    /**
     * Destroy the OpenAL context and try to clean up any resources.
     * This must be called manually when the main application exits to ensure
     * that OpenAL is properly deinitialized and that all open sound files
     * are closed.
     *
     * @author Darryl Pogue
     */
    void shutdown();

    /**
     * Creates the OpenAL context. This does not need to be called except 
     * this must be called manually when you want to continue using
     * sounds but have called shutdown() already
     *
     * @author Terence Stenvold
     */
    void startup();
    
    /**
     * Play the Ogg Vorbis or wav sound file
     *
     * @author Terence Stenvold
     * @param filename QString of ogg file.
     */    
    void playMusic(QString filename);

    /**
     * Play the queue stream
     *
     * @author Terence Stenvold
     */    
    void playStream();	

signals:
	void finished();
	
};

#endif
