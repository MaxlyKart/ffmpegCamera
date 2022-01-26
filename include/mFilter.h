#ifndef MAXLY_FILTER
#define MAXLY_FILTER

#include "typeAInclude.h"
#include <string>

class mFilter {
    private:
    AVFilterGraph *filterGraph;
    AVFilterContext *filterInCtx;
    AVFilterContext *filterOutCtx;
    AVFilterInOut *inStru;
    AVFilterInOut *outStru;

    public:
    mFilter(AVCodecContext *pCodecCtx);
    int getFilteredFrame(AVFrame *inFrame, char *drawStr);
    ~mFilter();
};

#endif