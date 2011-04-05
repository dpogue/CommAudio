#ifndef DEFINES_H
#define DEFINES_H
//For Network
#define WM_SOCKET WM_USER+1
#define TCP 0
#define UDP 1
#define CLIENT 2
#define SERVER 3
#define BUFSIZE (1024 * 4)
#define BACKLOG 5
#define NORMAL     0
#define FILESAVING 1

#define MULTICAST 0xE8152A01
#define STREAMINGBUFSIZE (1024*16)


// images
#define ICON_PLAY           "img/play.png"
#define ICON_PAUSE          "img/pause.png"
#define ICON_MUTE           "img/mute.png"
#define ICON_UNMUTE         "img/unmute.png"
#define ICON_CHAT           "img/chat.png"
#define ICON_CHATTING       "img/chatting.png"
#define ICON_SHUFFLE_ON     "img/shuffle_on.png"
#define ICON_SHUFFLE_OFF    "img/shuffle_off.png"
#define ICON_LOOP_ON        "img/loop_on.png"
#define ICON_LOOP_OFF       "img/loop_off.png"

// playing state
#define STOPPED     0
#define PLAYING     1
#define PAUSED      2

#endif