#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <libswscale/swscale.h>
//gcc -c -fPIC -L/opt/ffmpeg/lib -I/opt/ffmpeg/include/ ffmpeg.c -lavcodec -lavformat -lavfilter -lavdevice -lswresample -lswscale -lavutil -o ffmpeg.o
void logging(const char *fmt, ...);
// decode packets into frames
uint8_t* decode_packet(AVCodecContext *pCodecContext, AVPacket *pPacket, AVFrame *pFrame);
int initav(const char* url, int thread_count);
void tiniav();
uint8_t* decode();
/////////////global start////////////
AVFormatContext *pFormatContext = 0;
AVCodecContext *pCodecContext = 0;
struct SwsContext* swsContext = 0;
AVFrame *pFrame = 0;
AVFrame* dstframe = 0; 
AVPacket *pPacket = 0;
int video_stream_index = -1;
int stride = 5;
int count = 0;
int img_width = 0;
int img_height = 0;
uint8_t* p_global_bgr_buffer = 0;
/////////////global end////////////

int initav(const char* url, int thread_count){
    logging("initializing all the containers, codecs and protocols.");
    pFormatContext = avformat_alloc_context();
    if (!pFormatContext) {
        logging("ERROR could not allocate memory for Format Context");
        return -1;
    }

    logging("opening the input file and loading format (container) header");
    if (avformat_open_input(&pFormatContext, url, NULL, NULL) != 0) {
        logging("ERROR could not open the file");
        return -1;
    }

    logging("format %s, duration %lld us, bit_rate %lld", pFormatContext->iformat->name, pFormatContext->duration, pFormatContext->bit_rate);

    logging("finding stream info from format");
    if (avformat_find_stream_info(pFormatContext,  NULL) < 0) {
        logging("ERROR could not get the stream info");
        return -1;
    }

    AVCodec *pCodec = NULL;

    AVCodecParameters *pCodecParameters =  NULL;
  // loop though all the streams and print its main information
    for (int i = 0; i < pFormatContext->nb_streams; i++){
        AVCodecParameters *pLocalCodecParameters =  NULL;
        pLocalCodecParameters = pFormatContext->streams[i]->codecpar;
        logging("AVStream->time_base before open coded %d/%d", pFormatContext->streams[i]->time_base.num, pFormatContext->streams[i]->time_base.den);
        logging("AVStream->r_frame_rate before open coded %d/%d", pFormatContext->streams[i]->r_frame_rate.num, pFormatContext->streams[i]->r_frame_rate.den);
        logging("AVStream->start_time %" PRId64, pFormatContext->streams[i]->start_time);
        logging("AVStream->duration %" PRId64, pFormatContext->streams[i]->duration);

        logging("finding the proper decoder (CODEC)");

        AVCodec *pLocalCodec = NULL;
        pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

        if (pLocalCodec==NULL) {
          logging("ERROR unsupported codec!");
          return -1;
        }

        // when the stream is a video we store its index, codec parameters and codec
        if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            pCodec = pLocalCodec;
            pCodecParameters = pLocalCodecParameters;
            logging("Video Codec: resolution %d x %d", pLocalCodecParameters->width, pLocalCodecParameters->height);
            // print its name, id and bitrate
            logging("\tCodec %s ID %d bit_rate %lld", pLocalCodec->name, pLocalCodec->id, pCodecParameters->bit_rate);
        } else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            logging("Audio Codec: %d channels, sample rate %d", pLocalCodecParameters->channels, pLocalCodecParameters->sample_rate);
        }
    }

    pCodecContext = avcodec_alloc_context3(pCodec);
    if (!pCodecContext){
        logging("failed to allocated memory for AVCodecContext");
        return -1;
    }
    if(thread_count != 0){
        pCodecContext->thread_count = thread_count;
        pCodecContext->thread_type = FF_THREAD_FRAME;
    }

    if (avcodec_parameters_to_context(pCodecContext, pCodecParameters) < 0){
        logging("failed to copy codec params to codec context");
        return -1;
    }
    logging("thread_count: %d", pCodecContext->thread_count);

    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0){
        logging("failed to open codec through avcodec_open2");
        return -1;
    }

    pFrame = av_frame_alloc();
    if (!pFrame){
        logging("failed to allocated memory for AVFrame");
        return -1;
    }

    pPacket = av_packet_alloc();
    if (!pPacket){
        logging("failed to allocated memory for AVPacket");
        return -1;
    }
}

uint8_t* decode(){
    uint8_t* l_buffer = NULL;
    if(p_global_bgr_buffer){
        free(p_global_bgr_buffer);
        p_global_bgr_buffer = 0;
    }
    while (av_read_frame(pFormatContext, pPacket) >= 0){
        if (pPacket->stream_index == video_stream_index) {
            //logging("AVPacket->pts %" PRId64, pPacket->pts);
            l_buffer = decode_packet(pCodecContext, pPacket, pFrame);
            av_packet_unref(pPacket);
            av_frame_unref(pFrame);
            break;
        }
        av_packet_unref(pPacket);
        av_frame_unref(pFrame);
    }
    return l_buffer;
}

void tiniav(){
    logging("releasing all the resources");
    avformat_close_input(&pFormatContext);
    avformat_free_context(pFormatContext);
    av_packet_free(&pPacket);
    av_frame_free(&pFrame);
    avcodec_free_context(&pCodecContext);
}

void logging(const char *fmt, ...){
    va_list args;
    fprintf( stderr, "LOG: " );
    va_start( args, fmt );
    vfprintf( stderr, fmt, args );
    va_end( args );
    fprintf( stderr, "\n" );
}

uint8_t*  decode_packet(AVCodecContext *pCodecContext, AVPacket *pPacket, AVFrame *pFrame){
    uint8_t* buffer = NULL;
    int response = avcodec_send_packet(pCodecContext, pPacket);
    if (response < 0) {
        logging("Error while sending a packet to the decoder: %s", av_err2str(response));
        return buffer;
    }

    while (response >= 0){
        response = avcodec_receive_frame(pCodecContext, pFrame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
            break;
        } else if (response < 0) {
            logging("Error while receiving a frame from the decoder: %s", av_err2str(response));
            return buffer;
        }

        if (response >= 0) {
            // logging(
            //     "Frame %d (type=%c, size=%d bytes) pts %d key_frame %d [DTS %d]",
            //     pCodecContext->frame_number,
            //     av_get_picture_type_char(pFrame->pict_type),
            //     pFrame->pkt_size,
            //     pFrame->pts,
            //     pFrame->key_frame,
            //     pFrame->coded_picture_number
            // );

            img_width = pFrame->width;
            img_height = pFrame->height;

            if(img_width > 0 && swsContext == 0){
                logging("test %d vs %d", pFrame->width, pFrame->height);
                swsContext = sws_getContext(pFrame->width, pFrame->height, AV_PIX_FMT_YUV420P,pFrame->width, pFrame->height, AV_PIX_FMT_BGR24,
                          NULL, NULL, NULL, NULL);
            }else if(swsContext == 0){
                return NULL;
            }

            int linesize[8] = {pFrame->linesize[0] * 3};
            int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_BGR24, pFrame->width, pFrame->height, 1);
            logging("jpeg bytes: %d", num_bytes);
            p_global_bgr_buffer = (uint8_t*) malloc(num_bytes * sizeof(uint8_t));
            uint8_t* bgr_buffer[8] = {p_global_bgr_buffer};

            /* do the conversion */
            int ret2 = sws_scale(swsContext, pFrame->data, pFrame->linesize, 0, pFrame->height, bgr_buffer, linesize);

            if (ret2 < 0){
                logging("error about sws scale");
            }
            // return dstframe->data[0];
            return bgr_buffer[0];
        }
    }
    return NULL;
}
