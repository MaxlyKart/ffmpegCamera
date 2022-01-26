#include "mFilter.h"

mFilter::mFilter(AVCodecContext *pCodecCtx) {
    // 创建graph和filter和inout结构
    filterGraph = avfilter_graph_alloc();
    const AVFilter *filterIn = avfilter_get_by_name("buffer");
    const AVFilter *filterOut = avfilter_get_by_name("buffersink");
    inStru = avfilter_inout_alloc();
    outStru = avfilter_inout_alloc();
    if (!filterGraph || !filterIn || !filterOut || !inStru || !outStru) {
        printf("error! alloc error");
    }

    // 创建filterContext
    // inCtx
    char args[512];
    sprintf_s(args, sizeof(args),
        "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
        pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P,
		pCodecCtx->time_base.num, pCodecCtx->time_base.den,
		pCodecCtx->sample_aspect_ratio.num, pCodecCtx->sample_aspect_ratio.den
    );
    int ret = 0;
    ret = avfilter_graph_create_filter(&filterInCtx, filterIn, "in", args, NULL, filterGraph);
    if (ret < 0) {
        printf("create filter context1 error, ret:%d\n", ret);
    }

    // outCtx
    ret = avfilter_graph_create_filter(&filterOutCtx, filterOut, "out", NULL, NULL, filterGraph);
    if (ret < 0) {
        printf("create filter context2 error, ret:%d\n", ret);
    }
    enum AVPixelFormat pixFmts[] = {AV_PIX_FMT_YUV420P, AV_PIX_FMT_YUV420P};
    ret = av_opt_set_int_list(filterOutCtx, "pix_fmts", pixFmts,
		AV_PIX_FMT_YUV420P, AV_OPT_SEARCH_CHILDREN);
	if (ret < 0) {
		printf("cannot set output pixel format, ret:%d\n", ret);
	}

    // 初始化inout结构
	outStru->name       = av_strdup("in");
	outStru->filter_ctx = filterInCtx;
	outStru->pad_idx    = 0;
	outStru->next       = NULL;

	inStru->name       = av_strdup("out");
	inStru->filter_ctx = filterOutCtx;
	inStru->pad_idx    = 0;
	inStru->next       = NULL;

    // 给输出图像分配空间，初始化
    // outFrame = av_frame_alloc();
    // frameBuffer = (unsigned char*)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1));
    // av_image_fill_arrays(outFrame->data, outFrame->linesize, frameBuffer,
    //  AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
}

mFilter::~mFilter() {
    if (inStru) {
        avfilter_inout_free(&inStru);
        inStru = NULL;
    }
    if (outStru) {
        avfilter_inout_free(&outStru);
        outStru = NULL;
    }
    if (filterGraph) {
        avfilter_graph_free(&filterGraph);
        filterGraph = NULL;
    }
}

int mFilter::getFilteredFrame(AVFrame *inFrame, char *drawStr) {
    int ret = 0;
    ret = avfilter_graph_parse_ptr(filterGraph, drawStr, &inStru, &outStru, NULL);
    if (ret < 0) {
        printf("graph parse error, ret:%d\n", ret);
        return ret;
    }
    ret = avfilter_graph_config(filterGraph, NULL);
    if (ret < 0) {
        printf("graph config error, ret:%d\n", ret);
        return ret;
    }

    ret = av_buffersrc_add_frame(filterInCtx, inFrame);
    if (ret < 0) {
        printf("add frame error, ret:%d\n", ret);
        return ret;
    }

    ret = av_buffersink_get_frame(filterOutCtx, inFrame);
    if (ret < 0) {
        printf("get frame error, ret:%d\n", ret);
        return ret;
    }
    return ret;
}