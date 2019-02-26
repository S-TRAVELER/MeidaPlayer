#include "playerwidget.h"

playerwidget::playerwidget(QWidget* parent):player(new MediaPlayer())
{
    setStyleSheet("background-color:black;");
   // run("");



    player->setOnShutdown([](const SockException &ex) {
        ErrorL << "OnShutdown:" << ex.what();
    });
    (*player)[RtspPlayer::kRtpType] = RtspPlayer::RTP_TCP;
}


void playerwidget::start(){

}

void playerwidget::play()
{
    {
        weak_ptr<MediaPlayer> weakPlayer = player;
        auto winId=this->winId();
        player->setOnPlayResult([weakPlayer,winId](const SockException &ex) {
            InfoL << "OnPlayResult:" << ex.what();
            auto strongPlayer = weakPlayer.lock();
            if (ex || !strongPlayer) {
                return;
            }

            auto viedoTrack = strongPlayer->getTrack(TrackVideo);
            if (!viedoTrack || viedoTrack->getCodecId() != CodecH264) {
                WarnL << "没有视频或者视频不是264编码!";
                return;
            }
            SDLDisplayerHelper::Instance().doTask([viedoTrack,winId]() {
                std::shared_ptr<H264Decoder> decoder(new H264Decoder);
                std::shared_ptr<YuvDisplayer> displayer(new YuvDisplayer((void*)winId));

                viedoTrack->addDelegate(std::make_shared<FrameWriterInterfaceHelper>([decoder, displayer](const Frame::Ptr &frame) {
                    SDLDisplayerHelper::Instance().doTask([decoder, displayer, frame]() {

                        AVFrame *pFrame = nullptr;
                        bool flag = decoder->inputVideo((unsigned char *) frame->data(), frame->size(),
                                                        frame->stamp(), &pFrame);
                        if (flag) {

                            displayer->displayYUV(pFrame);
                        }
                        return true;
                    });
                }));
                return true;
            });
        });
    }
    InfoL<<"Trying to connect "<<play_add;
    player->play(play_add.c_str());
    cout<<"hrer"<<endl;
}

void playerwidget::runloop(){
    SDLDisplayerHelper::Instance().runLoop();
}
void playerwidget::run(string address){
    if(!running){
        play_add=address;

        play();
        running=true;
    }
    else{
        player->teardown();
        play_add=address;
          play();
    }
}

playerwidget::~playerwidget(){

   // SDLDisplayerHelper::Instance().shutdown();

}
float playerwidget::getDuration(){
    /*AVFormatContext	*pFormatCtx;
    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();

    if(avformat_open_input(&pFormatCtx,play_add.c_str(),NULL,NULL)!=0){
        printf("Couldn't open input stream.\n");
        return -1;
    }
    if(avformat_find_stream_info(pFormatCtx,NULL)<0){
        printf("Couldn't find stream information.\n");
        return -1;
    }
    DebugL<<pFormatCtx->duration;

    return pFormatCtx->duration;*/
    return 0;
}
