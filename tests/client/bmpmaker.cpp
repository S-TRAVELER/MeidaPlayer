#include "bmpmaker.h"

using namespace toolkit;

BMPMaker::BMPMaker(int fWidth,int fHeight):buf(new uint8_t[3*MAXSIZE]),height(fHeight),width(fWidth) {
    bmpHeader.startPosition=header_size;
    bmpInfoHeader.Length=sizeof(InfoHead);
    bmpInfoHeader.colorPlane=1;
    bmpInfoHeader.bitColor=24;
}

int BMPMaker::AVFrameToRgb(AVFrame* pframe){
    if((fp_bmp=fopen(getStoragePath().c_str(),"wb"))==NULL){
        WarnL<<"Error: Cannot open output BMP file.";
        return -1;
    }
    height=pframe->height;
    width=pframe->width;
    size_t bufSize=width*height;

    bmpHeader.imageSize=3*bufSize+header_size;
    bmpInfoHeader.width=width;
    bmpInfoHeader.height=-height;
    bmpInfoHeader.realSize=3*bufSize;

    fwrite(bfType,1,sizeof(bfType),fp_bmp);
    fwrite(&bmpHeader,1,sizeof(bmpHeader),fp_bmp);
    fwrite(&bmpInfoHeader,1,sizeof(bmpInfoHeader),fp_bmp);


    /*uint8_t *pY=frame->data[0];
    uint8_t *pU=frame->data[1];
    uint8_t *pV=frame->data[2];
    uint8_t * pbuf=buf.get();

    for(int i=0;i<bufSize;++i){
        *pU = *pU  - 128;
        *pV = *pV- 128;
        *pbuf++= *pY + *pV + ((*pV * 103) >> 8);     //R
        *pbuf++= *pY -((*pU* 88) >> 8) - ((*pV * 183) >> 8); ;    //G
        *pbuf++= *pY + *pU +((*pU * 198) >> 8) ;  //B
        ++pY;
        if(!i%4){
            ++pU;
            ++pV;
        }
    }*/
    AVFrame *bgr=NULL;
    bgr=av_frame_alloc();
    av_image_fill_arrays(bgr->data, bgr->linesize, buf.get(),
        AV_PIX_FMT_BGR24, width,height, 1);
    struct SwsContext* imgCtx = NULL;
    imgCtx=sws_getContext(width,height,AV_PIX_FMT_YUV420P, width,height,AV_PIX_FMT_BGR24,SWS_BILINEAR,0,0,0);
    if(bgr!=NULL&&imgCtx!=NULL){
        sws_scale(imgCtx, pframe->data, pframe->linesize, 0, pframe->height,
                bgr->data,bgr->linesize);
        fwrite(bgr->data[0],bufSize*3,1,fp_bmp);
    }

    fclose(fp_bmp);
    return 0;
}

BMPMaker::~BMPMaker(){

}

std::string BMPMaker::getStoragePath(){
    string path;
    path+="FrameStorage/";
    time_t nowtime;
    nowtime = time(NULL); //获取日历时间
    times=(nowtime==lastTime)?times+1:1;
    lastTime=nowtime;
    path+=to_string(nowtime);
    path+="_";
    path+=to_string(times);
    path+=".bmp";
    return path;
}
