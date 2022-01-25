#ifndef MAXLY_TYPEANDINCLUDE
#define MAXLY_TYPEANDINCLUDE

#define _IN
#define _OUT
#define _IN_OUT

#define SDL_REFRESHEVENT SDL_USEREVENT + 1
#define SDL_BREAKEVENT SDL_USEREVENT + 2

#define D_SHOW_DEV "dshow"
#define VFW_CAP_DEV "vfwcap"

#include <stdio.h>
#include <iostream>
#ifdef __cplusplus
extern "C" {
#endif
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libavfilter/avfilter.h"
    #include "libavfilter/buffersink.h"
    #include "libavfilter/buffersrc.h"
    #include "libavdevice/avdevice.h"
    #include "libavutil/dict.h"
    #include "libavutil/imgutils.h"
    #include "libswscale/swscale.h"
    #include "SDL.h"
#ifdef __cplusplus
};

struct CAMERA_TYPE {
    const char *dshow = "dshow";
    const char *vfwCap = "vfwcap";
};

/**
 * @brief 把废弃格式转换成当前使用格式
 * 
 * @param format 
 * @return AVPixelFormat 
 */
AVPixelFormat ConvertDeprecatedFormat(enum AVPixelFormat format);

#endif

#endif