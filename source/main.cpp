#include <stdio.h>
#include <iostream>
#ifdef __cplusplus
extern "C" {
#endif
#ifdef _WIN32
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libavfilter/avfilter.h"
    #include "libavdevice/avdevice.h"
    #include "libavutil/dict.h"
    #include "libavutil/imgutils.h"
    #include "libswscale/swscale.h"
    #include "SDL.h"
#else
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavfilter/avfilter.h>
    #include <libavdevice/avdevice.h>
    #include <SDL/SDL.h>
#endif
#ifdef __cplusplus
};
#endif

#define SDL_REFRESHEVENT SDL_USEREVENT + 1
#define SDL_BREAKEVENT SDL_USEREVENT + 2

#define _IN
#define _OUT
#define _IN_OUT

//Show Dshow Device
void show_dshow_device(){
	AVFormatContext *pFormatCtx = avformat_alloc_context();
	AVDictionary* options = NULL;
	av_dict_set(&options,"list_devices","true",0);
	AVInputFormat *iformat = av_find_input_format("dshow");
	printf("========Device Info=============\n");
	avformat_open_input(&pFormatCtx,"video=dummy",iformat,&options);
	printf("================================\n");
}

//Show VFW Device
void show_vfw_device(){
	AVFormatContext *pFormatCtx = avformat_alloc_context();
	AVInputFormat *iformat = av_find_input_format("vfwcap");
	printf("========VFW Device Info======\n");
	avformat_open_input(&pFormatCtx,"list",iformat,NULL);
	printf("=============================\n");
}

/**
 * @brief 初始化SDL
 * 
 * @param pCodecCtx 
 * @return int 
 */
int initSDL(AVCodecContext* pCodecCtx, SDL_Window **window, SDL_Renderer **render, SDL_Texture **tex) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        std::cout << "cant init SDL! error: " << SDL_GetError() << std::endl;
        return -1;
    }

    int ret = SDL_CreateWindowAndRenderer(pCodecCtx->width, pCodecCtx->height, 0, window, render);
    if (ret != 0) {
        std::cout << "create window error! error: " << SDL_GetError() << std::endl;
        return -1;
    }
    *tex = SDL_CreateTexture(*render, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);
    if (!*tex) {
        std::cout << "create texture error! error: " << SDL_GetError() << std::endl;
        return -1;
    }
}

/**
 * @brief 初始化设备、编解码器
 * 
 * @return int 
 */
int initFFmpeg() {
    // 注册所有的编解码器
	av_register_all();
    //Register Device
	avdevice_register_all();
	avformat_network_init();
}

int thread_exit = 0;

int refreshTread(void *q) {
    thread_exit = 0;
    SDL_Event event;
    while(!thread_exit) {
        event.type = SDL_REFRESHEVENT;
        SDL_PushEvent(&event);
        SDL_Delay(25);
    }
    // 跳出循环，thread_exit = 1
    thread_exit = 0;
    event.type = SDL_BREAKEVENT;
    SDL_PushEvent(&event);

    return 0;
}

/**
 * @brief 打开摄像头，输出参数
 * 
 * @param pFormatCtx 储存视频流
 * @param pCodecCtx 视频帧编解码上下文
 * @param pCodec 编解码器
 * @param videoIdx format结构中合法视频流下标
 * @return int 
 */
int openCamera(_OUT AVFormatContext **pFormatCtx, _OUT AVCodecContext **pCodecCtx,
 _OUT AVCodec **pCodec,_OUT int *videoIdx) {
    //Open File
	//char filepath[]="src01_480x272_22.h265";
	//avformat_open_input(&pFormatCtx,filepath,NULL,NULL)
    // open vfwcap
    AVInputFormat *ifmt = av_find_input_format("vfwcap");
    // 打开输入摄像头，将图像放进pFormatCtx的stream
	if(avformat_open_input(pFormatCtx,"0",ifmt,NULL)!=0){
		printf("Couldn't open input stream.\n");
		return -1;
	}

    if(avformat_find_stream_info(*pFormatCtx,NULL)<0)
	{
		printf("Couldn't find stream information.\n");
		return -1;
	}
	*videoIdx = -1;
	for(int i = 0; i < (*pFormatCtx)->nb_streams; i++) 
		if((*pFormatCtx)->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
		{
			*videoIdx=i;
			break;
		}
	if(*videoIdx == -1)
	{
		printf("Couldn't find a video stream.\n");
		return -1;
	}
    // 寻找解码器
	*pCodecCtx = (*pFormatCtx)->streams[*videoIdx]->codec;
	*pCodec = avcodec_find_decoder((*pCodecCtx)->codec_id);
	if(pCodec==NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}
    // 打开解码器
	if(avcodec_open2(*pCodecCtx, *pCodec,NULL)<0)
	{
		printf("Could not open codec.\n");
		return -1;
	}
}

/**
 * @brief 把废弃格式转换成当前使用格式
 * 
 * @param format 
 * @return AVPixelFormat 
 */
AVPixelFormat ConvertDeprecatedFormat(enum AVPixelFormat format)
{
    switch (format)
    {
    case AV_PIX_FMT_YUVJ420P:
        return AV_PIX_FMT_YUV420P;
        break;
    case AV_PIX_FMT_YUVJ422P:
        return AV_PIX_FMT_YUV422P;
        break;
    case AV_PIX_FMT_YUVJ444P:
        return AV_PIX_FMT_YUV444P;
        break;
    case AV_PIX_FMT_YUVJ440P:
        return AV_PIX_FMT_YUV440P;
        break;
    default:
        return format;
        break;
    }
}

/**
 * @brief 调用SDL展示
 * 
 * @param render 
 * @param tex 
 * @param pFormatCtx 
 * @param pCodecCtx 
 * @param videoIdx 
 * @return int 
 */
int displayWithSDL(_IN SDL_Renderer *render, _IN SDL_Texture *tex,
 _IN AVFormatContext *pFormatCtx, _IN AVCodecContext *pCodecCtx, _IN int& videoIdx) {
    // 分配后的pFrame里的data和buf都是空的
    AVFrame *pFrame = av_frame_alloc();
    AVFrame *pFrameYUV = av_frame_alloc();
    // 申请转换上下文，规定将pCodecCtx的格式转换为YUV420P
    // std::cout << "codecCtx pix_fmt " << pCodecCtx->pix_fmt << std::endl;
    // 输出13，是已废弃的AV_PIX_FMT_YUVJ422P
    AVPixelFormat pixFmt = ConvertDeprecatedFormat(pCodecCtx->pix_fmt);
    SwsContext* convertCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pixFmt,
     pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); 
    
    unsigned char *outBuffer=(unsigned char *)av_malloc(
        av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  pCodecCtx->width, pCodecCtx->height,1
        ));
	// 将outBuffer关联到FrameYUV上
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, outBuffer,
		AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);

    while(true) {
        SDL_Event event;
        SDL_WaitEvent(&event);
        if (event.type == SDL_REFRESHEVENT) {
            AVPacket *packet = av_packet_alloc();
            if (av_read_frame(pFormatCtx, packet) >= 0) {
                if (packet->stream_index == videoIdx) {
                    int gotPic;
                    int ret = avcodec_decode_video2(pCodecCtx, pFrame, &gotPic, packet);
                    if (ret < 0) {
                        std::cout << "Decode Error" << std::endl;
                        return -1;
                    }
                    if (gotPic) {
                        // 转换函数
                        sws_scale(convertCtx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
                        SDL_UpdateTexture(tex, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]);
                        SDL_RenderClear(render);
                        SDL_RenderCopy(render, tex, NULL, NULL);
                        SDL_RenderPresent(render);
                    }
                }
            } else {
                thread_exit = 1;
            }
            av_packet_free(&packet);
        } else if (event.type == SDL_BREAKEVENT) {
            break;
        } else if (event.type == SDL_QUIT) {
            thread_exit = 1;
        }
    }
    sws_freeContext(convertCtx);
}

int main (int argc, char** args) {
    initFFmpeg();

    show_dshow_device();
    show_vfw_device();
    
    // 分配封装上下文, 打开摄像头
	AVFormatContext	*pFormatCtx = avformat_alloc_context();
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    int videoIdx = -1;
    openCamera(&pFormatCtx, &pCodecCtx, &pCodec, &videoIdx);

    // ————————SDL——————————
    SDL_Window *window;
    SDL_Renderer *render;
    SDL_Texture *tex;
    initSDL(pCodecCtx, &window, &render, &tex);
    SDL_Thread* thread = SDL_CreateThread(refreshTread, NULL, NULL);
    
    // 绘图
    displayWithSDL(render, tex, pFormatCtx, pCodecCtx, videoIdx);

    SDL_Quit();
    // av_free();
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
    system("pause");
    return 0;
}