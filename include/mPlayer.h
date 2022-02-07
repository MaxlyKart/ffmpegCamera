#ifndef MAXLY_PLAYER
#define MAXLY_PLAYER

#include "typeAInclude.h"
#include "mRecorder.h"
#include "mFilter.h"
#include <time.h>
#include "processStatus.h"

enum SOURCE_TYPE {
    CAM = 0,
    FD
};

/**
 * @brief SDL发送事件线程函数
 * 
 * @param q 
 * @return int 状态码
 */
static int refreshSDLThread(void *data);

class mPlayer{
    private:
    mRecorder *videoRecorder;
    bool threadExit;
    int fps;
    bool showFPS;
    int maxRecordTime;

    // ffmpeg
    AVFormatContext	*pFormatCtx;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVStream *videoStream;
    // filter
    mFilter *subTitleFilter;

    // SDL
    SDL_Window *window;
    SDL_Renderer *render;
    SDL_Texture *tex;
    // bool threadExit = true;
    SDL_Thread *sdlThread;

    /**
     * @brief 进行ffmpeg初始化
     * 
     * @return int 
     */
    int initFFmpeg();

    /**
     * @brief 初始化SDL
     * 
     * @return int 
     */
    int initSDL();

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

    /**
     * @brief Set the Recorder object
     * 
     * @param recorder 
     * @return int 
     */
    int setRecorder(mRecorder* recorder);
    /**
     * @brief Get the Recorder object
     * 
     * @return mRecorder* 
     */
    mRecorder* getRecorder();
    /**
     * @brief 将player持有的recorder对象清除
     * 不会delete该对象
     * 
     * @return int 
     */
    int cleanRecorder();

    // /**
    //  * @brief SDL发送事件线程函数
    //  * 
    //  * @param q 
    //  * @return int 状态码
    //  */
    // int refreshSDLThread(void *q);

    /**
     * @brief 是否显示帧率
     * 
     * @param isShow 
     * @return int 
     */
    int setShowFPS(bool isShow);

    /**
     * @brief 设置录制最长时间，-1为无限制
     * 
     * @param recordTime 
     * @return int 
     */
    int setRecordTime(int recordTime);
};

#endif