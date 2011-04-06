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
#define BUFFERSIZE (1024*8)

enum fileType{ 
OGG,
WAV,
AIFF
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

    /** 
     * mutex to protect stuff
     */
    static QMutex mutex_;

    /**
     * Number of playing songs should never be > 1
     */ 
    static int playCount_;	

    /**
     * Bools to stop and pause different playback and records
     */
    static bool pause_;
    static bool stop_;	
	static bool capturePause_;
	static bool captureStop_;

    /**
     * ensures the next track played is the right one
     */ 
    static QString nextplay_; 

    /**
     * Lets the class know if it's multicast or not
     */
    static bool multicast_;

    /**
     * The volume/gain of the background music.
     */
    static float musicGain_;

    /**
    * The stream Queue and netqueue for buffering audio data
    */
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
    
    /**
     * set the bitmask from the format and freq
     *
     * @author Terence Stenvold
     * @param format is like stereo16 or mono8 etc
     * @param freq is the frequency to play at
     */ 
    char getBitmask(ALenum format, ALuint freq);

    /**
     * get the bitmask for the format and freq
     *
     * @author Terence Stenvold
     * @param bitmask is the bitmask of the format freq
     * @param *format is like stereo16 or mono8 etc
     * @param *freq is the frequency to play at
     */ 
    void getSpecs(char bitmask, ALenum *format, ALuint *freq);

    /**
     * cleans up the buffers and source
     *
     * @author Terence Stenvold
     * @param *source is the source
     * @param *buffer is the array of buffers
     */ 
	void cleanUp(ALuint *source, ALuint *buffer);

    /**
     * clear buffers to be filled again
     *
     * @author Terence Stenvold
     * @param *play is a bool
     * @param *playing is a bool
     * @param *source is the source
     * @param buffersAvailable is the number of buffers empty
     */ 
	void clearProcessedBuffers
		(ALuint *source, int &buffersAvailable, ALint *playing, ALint* play);

    /**
     * opens the oggfile and gets the format and freq
     *
     * @author Terence Stenvold
     * @param file is the file pointer
     * @param oggFile is the oggfile pointer
     * @param format is the format to be set
     */ 
	void openOgg(FILE *file, OggVorbis_File *oggFile, ALenum *format);
    
    /**
     * opens the wavfile and gets the format and freq
     *
     * @author Terence Stenvold
     * @param file is the file pointer
     * @param format is the format to be set
     * @param freq is the frequency to be set
     */
    void openWav(FILE **file, ALenum *format, ALuint *freq);
    
    /**
     * opens the aiff file and gets the format and freq
     *
     * @author Terence Stenvold
     * @param file is the file pointer
     * @param format is the format to be set
     * @param freq is the frequency to be set
     */
    void openAiff(FILE **file, ALenum *format, ALuint *freq);

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

    /**
     * toggle the music pause
     *
     * @author Terence Stenvold
     */
	static void togglePause() {
		mutex_.lock();
		pause_ = !pause_;
		mutex_.unlock();
	}

    /**
     * toggle the music stop
     *
     * @author Terence Stenvold
     */
	static void toggleStop() {
		mutex_.lock();
		stop_ = !stop_;
		mutex_.unlock();
	}

    /**
     * get the music pause state
     *
     * @author Terence Stenvold
     * @return bool whether paused or not
     */
	static bool getPause() {
        bool temp;		
		mutex_.lock();
		temp = pause_;
		mutex_.unlock();
		return temp;
	}

    /**
     * get the music stop state
     *
     * @author Terence Stenvold
     * @return the current stop state
     */
    static bool getStop() {
        bool temp;		
		mutex_.lock();
		temp = stop_;
		mutex_.unlock();
		return temp;
	}

    /**
     * gets the capture pause
     *
     * @author Terence Stenvold
     * @returns the capture pause state
     */
    static bool getCapturePause() {
        bool temp;		
		mutex_.lock();
		temp = capturePause_;
		mutex_.unlock();
		return temp;
	}

    /**
     * toggle the capture stop
     *
     * @author Terence Stenvold
     * @returns the capture stop state
     */
	static bool getCaptureStop() {
        bool temp;		
		mutex_.lock();
		temp = captureStop_;
		mutex_.unlock();
		return temp;
	}

    /**
     * get the number of songs playing
     *
     * @author Terence Stenvold
     * @returns the number of songs playing
     */
	static int getPlayCount() {
        int temp;		
		mutex_.lock();
		temp = playCount_;
		mutex_.unlock();
		return temp;
	}

    /**
     * toggle the capture pause
     *
     * @author Terence Stenvold
     */
	static void toggleCapturePause() {
		mutex_.lock();
		capturePause_ = !capturePause_;
		mutex_.unlock();
	}

    /**
     * toggle the music stop
     *
     * @author Terence Stenvold
     */
	static void toggleCaptureStop() {
		mutex_.lock();
		captureStop_ = !captureStop_;
		mutex_.unlock();
	}
    
    /**
     * set the multicast bool
     *
     * @author Terence Stenvold
     * @param mcast is a bool to set 
     */
    static void setMulticast(bool mcast) {
        mutex_.lock();
        multicast_ = mcast;
        mutex_.unlock();
    }
    
    /**
     * change the volume
     *
     * @author Terence Stenvold
     * @param vol is the new volume
     */
	static void setGain(float vol) {
		mutex_.lock();
		musicGain_ = vol;
		mutex_.unlock();
	}
    
    /**
     * add a buffer to the queue
     *
     * @author Terence Stenvold
     * @param buffer is the buffer
     */
	static void addToQueue(QByteArray buffer) {
		mutex_.lock();
		streamQueue.enqueue(buffer);
		mutex_.unlock();
	}

    /**
     * add a buffer to the queue
     *
     * @author Terence Stenvold
     * @param bitmask contains the format
     * @param buffer is the buffer
     * @param size of the buffer
     */
	static void addToNetworkQueue(char bitmask, char *buffer, int size) {
		QByteArray temp(buffer,size);
        mutex_.lock();
        temp.prepend(bitmask);
        temp = qCompress(temp,9);
        netQueue.enqueue(temp); 
		mutex_.unlock();
	}	

    /**
     * get the queue size
     *
     * @author Terence Stenvold
     * @return the size of the queue
     */
    static int getQueueSize() {
		int temp;
		mutex_.lock();
		temp = streamQueue.count();
		mutex_.unlock();
		return temp;
	}

    /**
     * get the next buffer in the queue
     *
     * @author Terence Stenvold
     * @return the next buffer
     */
	static QByteArray getNextInQueue() {
		QByteArray temp = NULL;
		mutex_.lock();
        if(streamQueue.count() > 0) {		
			temp = streamQueue.dequeue();
		}
		mutex_.unlock();
		return qUncompress(temp);
	}

    /**
     * get the next buffer in the queue
     *
     * @author Terence Stenvold
     * @return the next buffer
     */
	static QByteArray* getNextNetworkQueue() {
		QByteArray* temp = NULL;
		mutex_.lock();
        if(netQueue.count() > 0) {		
			temp = new QByteArray(netQueue.dequeue());
		}
		mutex_.unlock();
		return temp;
	}

    /**
     * set the next track filename
     *
     * @author Terence Stenvold
     * @param next is the filename of the next song
     */
    static void setNextPlaying(QString next) {
        mutex_.lock();
        nextplay_ = next;
        mutex_.unlock();
    }
    
    /**
     * start capturing from the mic
     *
     * @author Terence Stenvold
     */
	void startCapture();

    /**
     * capture the mic data
     *
     * @author Terence Stenvold
     */
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
