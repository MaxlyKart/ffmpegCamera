#include "mPlayer.h"

int mPlayer::initFFmpegAndSDL() {
    // 注册所有的编解码器
	av_register_all();
    //Register Device
	avdevice_register_all();
	avformat_network_init();

    //———————init SDL———————
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        std::cout << "cant init SDL! error: " << SDL_GetError() << std::endl;
    }

    int ret = SDL_CreateWindowAndRenderer(pCodecCtx->width, pCodecCtx->height, 0, &window, &render);
    if (ret != 0) {
        std::cout << "create window error! error: " << SDL_GetError() << std::endl;
    }
    tex = SDL_CreateTexture(render, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);
    if (!tex) {
        std::cout << "create texture error! error: " << SDL_GetError() << std::endl;
    }
}

mPlayer::mPlayer(SOURCE_TYPE srcType, const char *srcName) {
    initFFmpegAndSDL();

    AVInputFormat * inputFormat = NULL;
    AVDictionary *option = NULL;
    if (srcType == CAM) {
        inputFormat = av_find_input_format(srcName);
        srcName = "0";
        
        // 分辨率
        av_dict_set(&option, "video_size", "500x300", 0);
        // 设置帧率
        av_dict_set(&option, "framerate", "25", 0);
    }

    // 打开输入，将图像放进pFormatCtx的stream
    int ret = avformat_open_input(&pFormatCtx, srcName, inputFormat, &option);
        if (ret != 0) {
            printf("source name error, file name:%s", srcName);
            goto endOfFunc;
    }
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = pFormatCtx->streams[i];
            pCodecCtx = pFormatCtx->streams[i]->codec;
        }
    }
    // 寻找解码器
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec==NULL)
	{
		printf("Codec not found.\n");
		goto endOfFunc;
	}
    // 打开解码器
	if(avcodec_open2(pCodecCtx, pCodec, NULL)<0)
	{
		printf("Could not open codec.\n");
		goto endOfFunc;
	}

    // 用于错误转跳
    endOfFunc:
}

mPlayer::~mPlayer() {
    SDL_Quit();

    // av_free();
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
}

int mPlayer::showDShowDevice() {
    AVFormatContext *pFormatCtx = avformat_alloc_context();
	AVDictionary* options = NULL;
	av_dict_set(&options,"list_devices","true",0);
	AVInputFormat *iformat = av_find_input_format(D_SHOW_DEV);
	printf("========Device Info=============\n");
	avformat_open_input(&pFormatCtx,"video=dummy",iformat,&options);
	printf("================================\n");
}

int mPlayer::showVfwCapDevice() {
    AVFormatContext *pFormatCtx = avformat_alloc_context(); 
    AVInputFormat *iformat = av_find_input_format(VFW_CAP_DEV);
	printf("========VFW Device Info======\n");
	avformat_open_input(&pFormatCtx,"list",iformat,NULL);
	printf("=============================\n");
}

int mPlayer::refreshSDLThread(void *p) {
    threadExit = false;
    SDL_Event event;
    while(!threadExit) {
        event.type = SDL_REFRESHEVENT;
        SDL_PushEvent(&event);
        // 25帧
        SDL_Delay(40);
    }
    event.type = SDL_BREAKEVENT;
    SDL_PushEvent(&event);
    return 0;
}

int mPlayer::SDLDisplay() {
    if (!sdlThread) {
        sdlThread = SDL_CreateThread(refreshSDLThread, NULL, NULL);
    }

    AVPixelFormat pixFmt = ConvertDeprecatedFormat(pCodecCtx->pix_fmt);
    SwsContext* convertCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, 
    pixFmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
    
    while(true) {
        SDL_Event event;
        SDL_WaitEvent(&event);
        const Uint8 *keyState = SDL_GetKeyboardState(NULL);
        // 绘制
        if (event.type == SDL_REFRESHEVENT) {
            AVPacket *readPkt = av_packet_alloc();
            // 从流里读一个包
            int ret = av_read_frame(pFormatCtx, readPkt);
            if (ret != 0) {
                printf("Read frame error! ret:%d", ret);
                break;
            }
            // 把包发给ffmpeg解码
            AVFrame *pFrame = av_frame_alloc(), *pFrameYUV = av_frame_alloc();
            ret = avcodec_send_packet(pCodecCtx, readPkt);
            if (ret != 0) {
                printf("Send pkt error! ret:%d", ret);
                break;
            }
            // 接收解码后的frame
            ret = avcodec_receive_frame(pCodecCtx, pFrame);
            if (ret != 0) {
                printf("Receive frame error! ret:%d", ret);
                break;
            }
            // 转成YUV
            ret = sws_scale(convertCtx, (const unsigned char* const*)pFrame->data, pFrame->linesize,
            0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
            if (ret != 0) {
                printf("Scale frame error! ret:%d", ret);
                break;
            }
            
            // 把yuv图像更新到贴图上
            SDL_UpdateTexture(tex, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]);
            // 清理上一帧图像
            SDL_RenderClear(render);
            // 贴图更新到render上
            SDL_RenderCopy(render, tex, NULL, NULL);
            // 把缓冲区的render画上去
            SDL_RenderPresent(render);
        } else if (keyState[SDLK_ESCAPE]) {
            threadExit = true;
        } else if (event.type == SDL_BREAKEVENT) {
            break;
        }
    }
}