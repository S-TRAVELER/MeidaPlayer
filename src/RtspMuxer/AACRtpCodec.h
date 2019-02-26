﻿/*
 * MIT License
 *
 * Copyright (c) 2016 xiongziliang <771730766@qq.com>
 *
 * This file is part of ZLMediaKit(https://github.com/xiongziliang/ZLMediaKit).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ZLMEDIAKIT_AACRTPCODEC_H
#define ZLMEDIAKIT_AACRTPCODEC_H

#include "RtpCodec.h"
#include "Extension/AAC.h"
namespace mediakit{
/**
 * aac rtp转adts类
 */
class AACRtpDecoder : public RtpCodec , public ResourcePoolHelper<AACFrame> {
public:
    typedef std::shared_ptr<AACRtpDecoder> Ptr;

    AACRtpDecoder(const Track::Ptr &track);
    ~AACRtpDecoder() {}

    /**
     * 输入rtp并解码
     * @param rtp rtp数据包
     * @param key_pos 此参数内部强制转换为false,请忽略之
     */
    bool inputRtp(const RtpPacket::Ptr &rtp, bool key_pos = false) override;

    TrackType getTrackType() const override{
        return TrackAudio;
    }
    CodecId getCodecId() const override{
        return CodecAAC;
    }
protected:
    AACRtpDecoder();
private:
    void onGetAAC(const AACFrame::Ptr &frame);
    AACFrame::Ptr obtainFrame();
private:
    AACFrame::Ptr _adts;
    string _aac_cfg;
};


/**
 * aac adts转rtp类
 */
class AACRtpEncoder : public AACRtpDecoder , public RtpInfo {
public:
    typedef std::shared_ptr<AACRtpEncoder> Ptr;

    /**
     * @param ui32Ssrc ssrc
     * @param ui32MtuSize mtu 大小
     * @param ui32SampleRate 采样率
     * @param ui8PlayloadType pt类型
     * @param ui8Interleaved rtsp interleaved 值
     */
    AACRtpEncoder(uint32_t ui32Ssrc,
                  uint32_t ui32MtuSize,
                  uint32_t ui32SampleRate,
                  uint8_t ui8PlayloadType = 97,
                  uint8_t ui8Interleaved = TrackAudio * 2);
    ~AACRtpEncoder() {}

    /**
     * 输入aac 数据，必须带dats头
     * @param frame 带dats头的aac数据
     */
    void inputFrame(const Frame::Ptr &frame) override;
private:
    void makeAACRtp(const void *pData, unsigned int uiLen, bool bMark, uint32_t uiStamp);
private:
    unsigned char _aucSectionBuf[1600];
};

}//namespace mediakit

#endif //ZLMEDIAKIT_AACRTPCODEC_H
