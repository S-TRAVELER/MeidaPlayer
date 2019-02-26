#ifndef SRC_DEVICE_PlayerPro_H_
#define SRC_DEVICE_PlayerPro_H_

#include <memory>
#include "Common/Device.h"
#include "Player/MediaPlayer.h"
#include "Util/TimeTicker.h"
#include "../YuvDisplayer.h"
#include "../H264Decoder.h"

using namespace std;
using namespace toolkit;


class PlayerPro :public MediaPlayer,
                   public std::enable_shared_from_this<PlayerPro> ,
                   public MediaSourceEvent{
public:
    typedef std::shared_ptr<PlayerPro> Ptr;

    //如果iRetryCount<0,则一直重试播放；否则重试iRetryCount次数
    //默认一直重试
    PlayerPro(shared_ptr<YuvDisplayer> displayer,int iRetryCount):weakyuv(displayer),_iRetryCount(iRetryCount){
    }
    virtual ~PlayerPro();

    void play(const char* strUrl) override;
    bool close() override;
    void shutdown();
private:
    void rePlay(const string &strUrl,int iFailedCnt);
    void onPlaySuccess();

private:
    int _iRetryCount=3;
    bool out=false;
    MultiMediaSourceMuxer::Ptr _mediaMuxer;
    weak_ptr<YuvDisplayer> weakyuv;
    std::shared_ptr<H264Decoder> decoder{new H264Decoder};
};

#endif /* SRC_DEVICE_PlayerPro_H_ */
