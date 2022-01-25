#ifndef MAXLY_RECORDER
#define MAXLY_RECORDER

#include "typeAInclude.h"

class mRecorder {
    private:
    AVFormatContext *outputFormatCtx;
    AVStream *outputStream;
    AVCodecContext *codecCtx;
    AVFrame *outputFrame;

    Uint64 recordStartTime;
    bool isRecording;
    int width;
    int height;
    const char* filename;
    int nextPts;

    public:
    mRecorder(const char* filename);
    int recordByFrame(SwsContext *convertCtx, AVFrame *pFrame);
    int init(int width, int height);
    Uint64 getRecordTime();
    ~mRecorder();
};

#endif