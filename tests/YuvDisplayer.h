/*
 * MIT License
 *
 * Copyright (c) 2017 xiongziliang <771730766@qq.com>
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
#ifndef YUVDISPLAYER_H_
#define YUVDISPLAYER_H_
#include <stdexcept>
#include "Util/onceToken.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
}
#endif

#if defined(_WIN32)
#pragma comment(lib,"SDL2.lib")
#endif //defined(_WIN32)
#include <iostream>
#include <memory>

using namespace toolkit;
using namespace mediakit;

#define REFRESH_EVENT   (SDL_USEREVENT + 1)


class SDLDisplayerHelper
{
public:
    static SDLDisplayerHelper &Instance(){
        static SDLDisplayerHelper *instance(new SDLDisplayerHelper);
        return *instance;
    }
    static void Destory(){
        delete &Instance();
    }
    template<typename FUN>
    void doTask(FUN &&f){
        {
            lock_guard<mutex> lck(_mtxTask);
            _taskList.emplace_back(f);
        }
        SDL_Event event;
        event.type = REFRESH_EVENT;
        SDL_PushEvent(&event);
    }

	void runLoop(){
		bool flag = true;
		std::function<bool ()> task;
		SDL_Event event;
		while(flag){
			SDL_WaitEvent(&event);
			if (event.type == REFRESH_EVENT)
			{
				{
					lock_guard<mutex> lck(_mtxTask);
					if(_taskList.empty()){
						//not reachable
						continue;
					}
					task = _taskList.front();
					_taskList.pop_front();
				}
				flag = task();
			}

		}
	}

	void shutdown(){
		doTask([](){return false;});
	}
private:
    SDLDisplayerHelper(){
    };
    ~SDLDisplayerHelper(){
		shutdown();
    };
private:
    std::deque<std::function<bool ()> > _taskList;
    std::mutex _mtxTask;

};


class YuvDisplayer {
public:
	YuvDisplayer(void *hwnd = nullptr,const char *title = "untitled"){

        static onceToken token([]() {
            if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) == -1) {
                string err = "初始化SDL失败:";
                err+= SDL_GetError();
                ErrorL << err;
                throw std::runtime_error(err);
            }
            if(TTF_Init()!=0){
                string err = "初始化SDL-TTF失败:";
                err+= SDL_GetError();
                ErrorL << err;
                throw std::runtime_error(err);
            }
            SDL_LogSetAllPriority(SDL_LOG_PRIORITY_CRITICAL);
            SDL_LogSetOutputFunction([](void *userdata, int category, SDL_LogPriority priority, const char *message){
                DebugL << category << " " <<  priority << message;
            },nullptr);
            InfoL << "SDL_Init";
        }, []() {
            SDLDisplayerHelper::Destory();
            TTF_Quit();
            SDL_Quit();
        });

		_title = title;
		_hwnd = hwnd;
	}
	virtual ~YuvDisplayer(){
            InfoL<<"SDL_free";
		if (_texture) {
			SDL_DestroyTexture(_texture);
			_texture = nullptr;
		}
		if (_render) {
			SDL_DestroyRenderer(_render);
			_render = nullptr;
		}
		if (_win) {
			SDL_DestroyWindow(_win);
			_win = nullptr;
		}
	}
	bool displayYUV(AVFrame *pFrame){
		if (!_win) {

			if (_hwnd) {
                _win = SDL_CreateWindowFrom(_hwnd);
            }else {
				_win = SDL_CreateWindow(_title.data(),
										SDL_WINDOWPOS_UNDEFINED,
										SDL_WINDOWPOS_UNDEFINED,
                                        pFrame->width,
                                        pFrame->height,
										SDL_WINDOW_OPENGL);
			}
		}
		if (_win && ! _render){
#if 0
			SDL_RENDERER_SOFTWARE = 0x00000001,         /**< The renderer is a software fallback */
                    SDL_RENDERER_ACCELERATED = 0x00000002,      /**< The renderer uses hardware
                                                                     acceleration */
                    SDL_RENDERER_PRESENTVSYNC = 0x00000004,     /**< Present is synchronized
                                                                     with the refresh rate */
                    SDL_RENDERER_TARGETTEXTURE = 0x00000008     /**< The renderer supports
                                                                     rendering to texture */
#endif

			_render = SDL_CreateRenderer(_win, -1, SDL_RENDERER_ACCELERATED);
		}
                if(_texture&&(pFrame->width!=_width||pFrame->height!=_height)){
                    SDL_DestroyTexture(_texture);
                    _texture = nullptr;
                }

                if (_render && !_texture) {
			_texture = SDL_CreateTexture(_render, SDL_PIXELFORMAT_IYUV,
										 SDL_TEXTUREACCESS_STREAMING,
										 pFrame->width,
										 pFrame->height);
                        _width=pFrame->width;
                        _height=pFrame->height;
		}

		if (_texture) {
			SDL_UpdateYUVTexture(_texture, nullptr,
								 pFrame->data[0], pFrame->linesize[0],
								 pFrame->data[1], pFrame->linesize[1],
								 pFrame->data[2], pFrame->linesize[2]);

			//SDL_UpdateTexture(_texture, nullptr, pFrame->data[0], pFrame->linesize[0]);
			SDL_RenderClear(_render);
			SDL_RenderCopy(_render, _texture, nullptr, nullptr);
			SDL_RenderPresent(_render);
			return true;
		}
		return false;
	}
        bool prompt(string message){
            if (!_win) {

                    if (_hwnd) {
            _win = SDL_CreateWindowFrom(_hwnd);
        }else {
                            _win = SDL_CreateWindow(_title.data(),
                                                                            SDL_WINDOWPOS_UNDEFINED,
                                                                            SDL_WINDOWPOS_UNDEFINED,
                                    640,
                                    480,
                                                                            SDL_WINDOW_OPENGL);
                    }
            }
            if (_win && ! _render){
#if 0
                    SDL_RENDERER_SOFTWARE = 0x00000001,         /**< The renderer is a software fallback */
                SDL_RENDERER_ACCELERATED = 0x00000002,      /**< The renderer uses hardware
                                                                 acceleration */
                SDL_RENDERER_PRESENTVSYNC = 0x00000004,     /**< Present is synchronized
                                                                 with the refresh rate */
                SDL_RENDERER_TARGETTEXTURE = 0x00000008     /**< The renderer supports
                                                                 rendering to texture */
#endif

                    _render = SDL_CreateRenderer(_win, -1, SDL_RENDERER_ACCELERATED);
            }

            if (_render) {
                    _width=640;
                    _height=480;
                    SDL_Color color={255,255,255,255};
                    SDL_Texture *image=renderText(message, "/usr/share/fonts/truetype/ttf-khmeros-core/KhmerOSsys.ttf", color, 16, _render);
                    int iW, iH;
                    SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
                    int x = _width / 2 - iW / 2;
                    int y = _height / 2 - iH / 2;

                    SDL_RenderClear(_render);
                    renderTexture(image,_render,x,y);
                    SDL_RenderPresent(_render);
                    if(image!=nullptr){
                        SDL_DestroyTexture(image);
                    }
                    return true;
            }
            return false;
        }
protected:
        void renderTexture(SDL_Texture *tex,SDL_Renderer *ren,SDL_Rect dst, SDL_Rect* clip=nullptr){
            SDL_RenderCopy(ren,tex,clip,&dst);
        }

        void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y,
                           SDL_Rect *clip=nullptr){
            SDL_Rect dst;
            dst.x=x;
            dst.y=y;
            if(clip!=nullptr){
                dst.w=clip->w;
                dst.h=clip->h;
            }
            else{
                SDL_QueryTexture(tex,NULL,NULL,&dst.w,&dst.h);
            }

            renderTexture(tex,ren,dst,clip);
        }

        void logSDLError(std::ostream &os,const std::string &msg){
            os<<msg<<" error: "<<SDL_GetError()<<std::endl;
        }

        SDL_Texture* renderText(const std::string &message, const std::string &fontFile, SDL_Color color,
                                int fontSize, SDL_Renderer *renderer){
            TTF_Font *font=TTF_OpenFont(fontFile.c_str(),fontSize);
            if(font==nullptr){
                logSDLError(std::cout,"TTF_OpenFont");
                return nullptr;
            }
            SDL_Surface *surf=TTF_RenderText_Blended(font,message.c_str(),color);
            if(surf==nullptr){
                TTF_CloseFont(font);
                logSDLError(std::cout,"TTF_RenderText");
                return nullptr;
            }
            SDL_Texture *texture=SDL_CreateTextureFromSurface(renderer,surf);
            if(texture==nullptr){
                logSDLError(std::cout,"CreateTexture");
            }
            SDL_FreeSurface(surf);
            TTF_CloseFont(font);
            return texture;
        }

private:
	string _title;
	SDL_Window *_win = nullptr;
	SDL_Renderer *_render = nullptr;
	SDL_Texture *_texture = nullptr;
	void *_hwnd = nullptr;
        int _width,_height;
};



class SDLDisplayer
{
public:
    typedef std::shared_ptr<SDLDisplayer> Ptr;
    SDLDisplayer(){
    }
    ~SDLDisplayer(){
        shutdown();
    }

    template<typename FUN>
    void doTask(FUN &&f){
        {
            lock_guard<mutex> lck(_mtxTask);
            _taskList.emplace_back(f);
        }
        SDL_Event event;
        event.type = REFRESH_EVENT;
        SDL_PushEvent(&event);
    }

    void runLoop(){
        std::function<bool ()> task;
        SDL_Event event;
        while(flag){
            SDL_WaitEvent(&event);
            if (event.type == REFRESH_EVENT)
            {
                {
                    lock_guard<mutex> lck(_mtxTask);
                    if(_taskList.empty()){
                        //not reachable

                        continue;
                    }
                    task = _taskList.front();
                    _taskList.pop_front();

                }
                if(false == task()){
                    flag=false;
                }

            }

        }
        _taskList.clear();
    }

    void shutdown(){
        flag=false;
    }
    void restart(){
        flag=true;
    }
    void clearTask(){
        _taskList.clear();
    }

private:
    std::deque<std::function<bool ()> > _taskList;
    std::mutex _mtxTask;
    bool flag=true;

};

#endif /* YUVDISPLAYER_H_ */
