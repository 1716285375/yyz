#ifndef VIDEODECODE_H
#define VIDEODECODE_H

#include <QThread>
#include <QMutex>
#include <QImage>
#include <QString>
#include <atomic>
#include <functional>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

class VideoDecoder : public QThread
{
    Q_OBJECT
public:
    explicit VideoDecoder(QObject *parent = nullptr);
    ~VideoDecoder();

    void startDecoding(const QString &url);
    void stopDecoding();

signals:
    void frameDecoded(const QImage &frame);
    void decodingFailed(const QString &reason);

protected:
    void run() override;

private:
    QElapsedTimer playbackTimer_;
    double firstPts_ = -1.0;

    QString m_url;
    std::atomic<bool> m_stopped = false;
    QMutex m_mutex;

    AVFormatContext *m_formatCtx = nullptr;
    AVCodecContext *m_codecCtx = nullptr;
    const AVCodec *m_codec = nullptr;
    AVFrame *m_frame = nullptr;
    AVPacket *m_packet = nullptr;
    SwsContext *m_swsCtx = nullptr;
    int m_videoStreamIndex = -1;

    void cleanup();
};

#endif // VIDEODECODE_H
