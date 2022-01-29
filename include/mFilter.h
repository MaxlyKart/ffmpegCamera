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
    mFilter(AVCodecContext *pCodecCtx, char *drawStr);
    int getFilteredFrame(AVFrame *inFrame);
    ~mFilter();
};

#endif