#include "mRecorder.h"

int mRecorder::init(int width, int height) {
    this->width = width;
    this->height = height;

    // AVIOContext --> AVFormatContext --> AVOutputFormat --> AVStream --> AVCodecContext --> AVCodec
    // 这里会利用filePath的后缀来解析formatCtx的封装格式
    int ret = avformat_alloc_output_context2(&outputFormatCtx, NULL, NULL, filename);
    if (ret != 0) {
        printf("Alloc output context error! ret:%d", ret);
        // 错误的话指定格式
        avformat_alloc_output_context2(&outputFormatCtx, NULL, "mpeg", filename);
        return ret;
    }
    AVOutputFormat *outputFormat = outputFormatCtx->oformat;
    // 分配code_id
    outputFormat->video_codec = AV_CODEC_ID_H264;
    // 找编码器
    AVCodec *codec = avcodec_find_encoder(outputFormat->video_codec);

    // 根据编码器分配上下文
    codecCtx = avcodec_alloc_context3(codec);
    // 设置上下文
    codecCtx->codec_id = outputFormat->video_codec;
    codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    codecCtx->time_base = {1,25};
    codecCtx->bit_rate = 400000;
    codecCtx->gop_size = 12;
    codecCtx->width = width;
    codecCtx->height = height;

    if (outputFormat->flags & AVFMT_GLOBALHEADER) {
        codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
    // 利用codec初始化codecCtx
    ret = avcodec_open2(codecCtx, codec, NULL);
    if (ret != 0) {
        printf("Open codec error! ret:%d", ret);
        return ret;
    }

    // 初始化AVStream
    outputStream = avformat_new_stream(outputFormatCtx, codec);
    if(outputStream == NULL){
        printf("New stream error! ret:%d", ret);
        return ret;
    }

    outputStream->id = outputFormatCtx->nb_streams - 1;
    // 把codecCtx的参数赋值给Stream的codecpar
    ret = avcodec_parameters_from_context(outputStream->codecpar, codecCtx);
    if(ret != 0){
        printf("Fill params error! ret:%d", ret);
        return ret;
    }

    // 打开AVIOContext
    ret = avio_open(&outputFormatCtx->pb, filename, AVIO_FLAG_WRITE);
    if(ret != 0){
        printf("Open IO context error! ret:%d", ret);
        return ret;
    }

    // 写入封装格式头
    ret = avformat_write_header(outputFormatCtx, NULL);
    if(ret != 0){
        return ret;
    }
}

mRecorder::mRecorder(const char* filename) {
    isRecording = false;
    this->filename = filename;
    recordStartTime = -1;
}

int mRecorder::recordByFrame(AVFrame *pFrame) {
    // 第一次创建
    if (!isRecording) {
        recordStartTime = SDL_GetTicks64();
        isRecording = true;
    }

    if (av_frame_make_writable(pFrame) < 0)
        exit(1);
    int ret = avcodec_send_frame(codecCtx, pFrame);
    AVPacket packet;
    av_init_packet(&packet);
    if(ret == 0){
        ret = avcodec_receive_packet(codecCtx, &packet);
        if(ret == 0){
            av_packet_rescale_ts(&packet, codecCtx->time_base, outputStream->time_base);
            av_write_frame(outputFormatCtx, &packet);
        }
        // av_packet_unref(&packet);
    }
}

mRecorder::~mRecorder() {
    if(outputFormatCtx != NULL){
        av_write_trailer(outputFormatCtx);
    }

    if(codecCtx != NULL){
        avcodec_close(codecCtx);
        avcodec_free_context(&codecCtx);
    }

    if(outputFormatCtx != NULL){
        avio_close(outputFormatCtx->pb);

        avformat_free_context(outputFormatCtx);
    }
}

Uint64 mRecorder::getRecordTime() {
    return recordStartTime ? SDL_GetTicks64() - recordStartTime : 0;
}

