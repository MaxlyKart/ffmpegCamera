#ifndef MAXLY_PLAYER
#define MAXLY_PLAYER

#include "typeAInclude.h"

enum SOURCE_TYPE {
    CAM = 0,
    FD
};

class mPlayer{
    private:
    // ffmpeg
    AVFormatContext	*pFormatCtx;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVStream *videoStream;

    // SDL
    SDL_Window *window;
    SDL_Renderer *render;
    SDL_Texture *tex;
    bool threadExit = true;
    SDL_Thread *sdlThread;

    /**
     * @brief 进行ffmpeg和SDL的初始化
     * 
     * @return int 
     */
    int initFFmpegAndSDL();
    /**
     * @brief SDL发送事件线程函数
     * 
     * @param q 
     * @return int 状态码
     */
    int refreshSDLThread(void *q);

    public:
    /**
     * @brief Construct a new m Player object
     * 根据type和name填装pFormatCtx、pCodecCtx、pCodec
     * 
     * @param srcType 摄像头或文件
     * @param srcName 
     */
    mPlayer(SOURCE_TYPE srcType, const char* srcName);
    /**
     * @brief Destroy the m Player object
     * 资源回收
     */
    ~mPlayer();
    /**
     * @brief 显示当前DShow设备清单
     * 
     * @return int 
     */
    static int showDShowDevice();
    /**
     * @brief 显示当前VFW设备清单
     * 
     * @return int 状态码
     */
    static int showVfwCapDevice();
    
    /**
     * @brief 显示函数
     * 
     * @return int 状态码
     */
    int SDLDisplay();
};

#endif