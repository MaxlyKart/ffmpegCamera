#include "mRecorder.h"


int mRecorder::initFFmpeg(const char * filePath) {
    // AVIOContext --> AVFormatContext --> AVOutputFormat --> AVStream --> AVCodecContext --> AVCodec
    int ret = avformat_alloc_output_context2(&outputFormatCtx, NULL, NULL, filePath);
    if (ret != 0) {
        printf("Alloc output context error! ret:%d", ret);
        return ret;
    }
    outputFormat = outputFormatCtx->oformat;
    // 分配code_id
    outputFormat->video_codec = AV_CODEC_ID_H264;
    // 找编码器
    AVCodec *codec = avcodec_find_encoder(outputFormat->video_codec);

    // 根据编码器分配上下文
    codecCtx = avcodec_alloc_context3(codec);
    // 设置上下文
    codecCtx->codec_id = outputFormat->video_codec;
    codecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

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

    // 把codecCtx的参数赋值给Stream的codecpar
    ret = avcodec_parameters_from_context(outputStream->codecpar, codecCtx);
    if(ret != 0){
        printf("Fill params error! ret:%d", ret);
        return ret;
    }

    // 打开AVIOContext
    ret = avio_open(&outputFormatCtx->pb, filePath, AVIO_FLAG_WRITE);
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

mRecorder::mRecorder(const char* filePath) {
    initFFmpeg(filePath);
    isRecording = false;
}

int mRecorder::recordByFrame(AVFrame *pFrameYUV) {
    if (!isRecording) {
        startTime = std::chrono::steady_clock::now();
    }
    auto nowTime = std::chrono::steady_clock::now();
    pFrameYUV->pts = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - startTime).count();

    int ret = avcodec_send_frame(codecCtx, pFrameYUV);
    if(ret == 0){
        AVPacket packet;
        av_init_packet(&packet);
        ret = avcodec_receive_packet(codecCtx, &packet);
        if(ret == 0){
            av_packet_rescale_ts(&packet, codecCtx->time_base, outputStream->time_base);
            av_write_frame(outputFormatCtx, &packet);
        }
        av_packet_unref(&packet);
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
        outputFormat = NULL;
    }
}