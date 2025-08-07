#include "videodecoder.h"
#include <QDebug>

VideoDecoder::VideoDecoder(QObject *parent)
    : QThread(parent)
{
    avformat_network_init();
}

VideoDecoder::~VideoDecoder()
{
    stopDecoding();
    wait();
    avformat_network_deinit();
}

void VideoDecoder::startDecoding(const QString &url)
{
    QMutexLocker locker(&m_mutex);
    if (isRunning()) stopDecoding();
    m_url = url;
    m_stopped = false;
    start();
}

void VideoDecoder::stopDecoding()
{
    m_stopped = true;
}

void VideoDecoder::run()
{
    playbackTimer_.start();
    firstPts_ = -1;
    cleanup();

    m_formatCtx = avformat_alloc_context();
    if (avformat_open_input(&m_formatCtx, m_url.toStdString().c_str(), nullptr, nullptr) != 0) {
        emit decodingFailed("Failed to open input: " + m_url);
        return;
    }

    if (avformat_find_stream_info(m_formatCtx, nullptr) < 0) {
        emit decodingFailed("Failed to find stream info");
        return;
    }

    m_videoStreamIndex = -1;
    for (unsigned int i = 0; i < m_formatCtx->nb_streams; ++i) {
        if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            m_videoStreamIndex = i;
            break;
        }
    }

    if (m_videoStreamIndex == -1) {
        emit decodingFailed("No video stream found");
        return;
    }

    AVCodecParameters *codecPar = m_formatCtx->streams[m_videoStreamIndex]->codecpar;
    m_codec = avcodec_find_decoder(codecPar->codec_id);
    if (!m_codec) {
        emit decodingFailed("Unsupported codec");
        return;
    }

    m_codecCtx = avcodec_alloc_context3(m_codec);
    if (avcodec_parameters_to_context(m_codecCtx, codecPar) < 0) {
        emit decodingFailed("Failed to copy codec parameters");
        return;
    }

    if (avcodec_open2(m_codecCtx, m_codec, nullptr) < 0) {
        emit decodingFailed("Failed to open codec");
        return;
    }

    m_frame = av_frame_alloc();
    m_packet = av_packet_alloc();

    m_swsCtx = sws_getContext(
        m_codecCtx->width,
        m_codecCtx->height,
        m_codecCtx->pix_fmt,
        m_codecCtx->width,
        m_codecCtx->height,
        AV_PIX_FMT_RGB24,
        SWS_BILINEAR,
        nullptr, nullptr, nullptr
        );

    int rgbStride[1] = { 3 * m_codecCtx->width };
    uint8_t *rgbBuffer = new uint8_t[rgbStride[0] * m_codecCtx->height];

    AVFrame *rgbFrame = av_frame_alloc();
    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, rgbBuffer, AV_PIX_FMT_RGB24,
                         m_codecCtx->width, m_codecCtx->height, 1);

    while (!m_stopped && av_read_frame(m_formatCtx, m_packet) >= 0) {
        if (m_packet->stream_index == m_videoStreamIndex) {
            if (avcodec_send_packet(m_codecCtx, m_packet) == 0) {
                while (avcodec_receive_frame(m_codecCtx, m_frame) == 0) {

                    // --- 播放节奏控制开始 ---
                    double pts_sec = 0.0;
                    if (m_frame->pts != AV_NOPTS_VALUE) {
                        pts_sec = m_frame->pts * av_q2d(m_formatCtx->streams[m_videoStreamIndex]->time_base);
                    }

                    if (firstPts_ < 0) {
                        firstPts_ = pts_sec;
                        playbackTimer_.restart();
                    }

                    double elapsed = playbackTimer_.elapsed() / 1000.0; // 转成秒
                    double waitTime = pts_sec - firstPts_ - elapsed;
                    if (waitTime > 0) {
                        QThread::msleep(static_cast<unsigned long>(waitTime * 1000));
                    }
                    // --- 播放节奏控制结束 ---

                    sws_scale(m_swsCtx,
                              m_frame->data, m_frame->linesize,
                              0, m_codecCtx->height,
                              rgbFrame->data, rgbFrame->linesize);

                    QImage img(rgbFrame->data[0], m_codecCtx->width, m_codecCtx->height,
                               rgbFrame->linesize[0], QImage::Format_RGB888);

                    emit frameDecoded(img.copy());
                }
            }
        }
        av_packet_unref(m_packet);
    }

    delete[] rgbBuffer;
    av_frame_free(&rgbFrame);
    cleanup();
}

void VideoDecoder::cleanup()
{
    if (m_packet) {
        av_packet_free(&m_packet);
        m_packet = nullptr;
    }

    if (m_frame) {
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }

    if (m_codecCtx) {
        avcodec_free_context(&m_codecCtx);
        m_codecCtx = nullptr;
    }

    if (m_formatCtx) {
        avformat_close_input(&m_formatCtx);
        m_formatCtx = nullptr;
    }

    if (m_swsCtx) {
        sws_freeContext(m_swsCtx);
        m_swsCtx = nullptr;
    }
}
