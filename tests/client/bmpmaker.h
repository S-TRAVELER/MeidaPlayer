#ifndef BMPMAKER_H_
#define BMPMAKER_H_
#include <stdexcept>
#include "Util/logger.h"
#include "Util/onceToken.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "SDL2/SDL.h"
#include "libswscale/swscale.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#ifdef __cplusplus
}
#endif
const int MAXSIZE=1024*1024*10;

#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avformat.lib")

typedef struct  //BMP Header
{
    uint32_t imageSize;
    uint32_t blank;
    uint32_t startPosition;
}BmpHead;

typedef struct  //BMP Information Header
{
    uint32_t  Length;
    uint32_t  width;
    uint32_t  height;
    unsigned short  colorPlane;
    unsigned short  bitColor;
    uint32_t  zipFormat;
    uint32_t  realSize;
    uint32_t  xPels;
    uint32_t  yPels;
    uint32_t  colorUse;
    uint32_t  colorImportant;
}InfoHead;

class BMPMaker{
public:
    BMPMaker(int fWidth,int fHeight);
    ~BMPMaker();
    int AVFrameToRgb(AVFrame*);
protected:
     std::string getStoragePath();

private:
    std::shared_ptr<uint8_t[]> buf;
    int height;
    int width;
    FILE *fp_bmp=nullptr;
    InfoHead bmpInfoHeader={0};
    BmpHead bmpHeader={0};
    char bfType[2]={'B','M'};
    int header_size=sizeof(bfType)+sizeof(BmpHead)+sizeof(InfoHead);
    long lastTime=0;
    int times=0;
};

#endif //BMPMAKER_H_
