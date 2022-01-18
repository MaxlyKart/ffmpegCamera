#ifndef MAXLY_RECORDER
#define MAXLY_RECORDER

#include "typeAInclude.h"
#include "chrono"

class mRecorder {
    private:
    AVFormatContext *outputFormatCtx;
    AVOutputFormat *outputFormat;
    AVStream *outputStream;
    AVCodecContext *codecCtx;

    std::chrono::steady_clock::time_point startTime;
    bool isRecording;

    int initFFmpeg(const char *filePath);
    public:
    mRecorder(const char* filePath);
    int recordByFrame(AVFrame *pFrameYUV);
    ~mRecorder();
};

#endif