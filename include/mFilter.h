#ifndef MAXLY_FILTER
#define MAXLY_FILTER

#include "typeAInclude.h"

class mFilter {
    private:
    AVFilterGraph *filterGraph;
    AVFilterContext *filterInCtx;
    AVFilterContext *filterOutCtx;
    AVFilterInOut *inStru;
    AVFilterInOut *outStru;

    public:
    mFilter(AVCodecContext *pCodecCtx);
    AVFrame* getFilteredFrame(AVFrame *inFrame, const char *drawStr);
    ~mFilter();
};

#endif