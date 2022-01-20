#ifndef MAXLY_RECORDER
#define MAXLY_RECORDER

#include "typeAInclude.h"
#include "chrono"

class mRecorder {
    private:
    AVFormatContext *outputFormatCtx;
    AVStream *outputStream;
    AVCodecContext *codecCtx;
    AVFrame *outputFrame;

    std::chrono::steady_clock::time_point startTime;
    bool isRecording;
    int width;
    int height;
    const char* filename;
    int nextPts;

    public:
    mRecorder(const char* filename);
    int recordByFrame(SwsContext *convertCtx, AVFrame *pFrame);
    int init(int width, int height);
    ~mRecorder();
};

#endif