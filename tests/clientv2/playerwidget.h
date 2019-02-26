#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H
#include <QWidget>
#include <signal.h>
#include <unistd.h>
#include "Util/logger.h"
#include <iostream>
#include <vector>
#include <thread>
#include "Poller/EventPoller.h"
#include "Rtsp/UDPServer.h"
#include "Player/MediaPlayer.h"
#include "Util/onceToken.h"
#include "../H264Decoder.h"
#include "../YuvDisplayer.h"
#include "Network/sockutil.h"

class playerwidget:public QWidget
{
public:
    playerwidget(QWidget* parent=0);
    ~playerwidget();
    void play();
    void pause();
    void run(string);
    float getDuration();
    void runloop();
    void start();
private:


    string play_add=
//            "rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov";
            //"rtsp://admin:gdutcs719@10.21.71.146:554/Streaming/Channels/101";
           // "rtmp://127.0.0.1/live/0";
    //"rtmp://127.0.0.1/record/live/0_1548212451.flv";
    "rtsp://127.0.0.1/record/live/0/2019-01-23/11-00-53.mp4";
    bool running=false;
    MediaPlayer::Ptr player;
    bool bpause=true;
};


#endif // PLAYERWIDGET_H
/*
#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H
#include <QWidget>
#include <signal.h>
#include <unistd.h>
#include "Util/logger.h"
#include <iostream>
#include <vector>
#include <thread>
#include "Poller/EventPoller.h"
#include "Rtsp/UDPServer.h"
#include "Player/MediaPlayer.h"
#include "Util/onceToken.h"
#include "../H264Decoder.h"
#include "../YuvDisplayer.h"
#include "Network/sockutil.h"

using namespace std;
using namespace toolkit;
using namespace mediakit;

class PlayerWidget:public QWidget

{
public:
    typedef std::shared_ptr<PlayerWidget> Ptr;

    PlayerWidget(QWidget* parent=0);
    ~PlayerWidget();
    void play();
    void pause();
    void run(string);
    void seekTo(float);
    float getDuration();

private:
    std::shared_ptr<YuvDisplayer> displayer;
    shared_ptr<SDLDisplayer> sdlPlayer;
    std::vector<std::thread> threads;
    string play_add=
//            "rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov";
            //"rtsp://admin:gdutcs719@10.21.71.146:554/Streaming/Channels/101";
           // "rtmp://127.0.0.1/live/0";
    //"rtmp://127.0.0.1/record/live/0_1548212451.flv";
    "rtsp://127.0.0.1/record/live/0/2019-01-23/11-00-53.mp4";
    bool running=false;
    PlayerPro::Ptr player;
    bool bpause=true;
};

#endif // PLAYERWIDGET_H
*/
