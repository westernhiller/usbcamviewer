#ifndef PROCESSTHREAD_H
#define PROCESSTHREAD_H

#include <QThread>
#include <QImage>
#include "global.h"
#include "camconfig.h"
#include "imagebuffer.h"
#include "dehaze.h"

class ProcessThread : public QThread
{
    Q_OBJECT
public:
    explicit ProcessThread(ImageBuffer* pImageBufferIn = nullptr,
                           ImageBuffer* pImageBufferOut = nullptr,
                           ImageBuffer* pImageBufferOut2 = nullptr,
                           QObject* parent = nullptr);
    ~ProcessThread();

    inline Dehaze* getDehaze() { return &m_defog;}

signals:

protected:
    void            run();

private:
    CamConfig*      m_pConfig;
    ImageBuffer*    m_pImageBufferIn;
    ImageBuffer*    m_pImageBufferOut;
    ImageBuffer*    m_pImageBufferOut2;
    Dehaze          m_defog;
    QString         m_fps;
    QString         m_clock;
    int             m_nFontSize;
    bool            m_bDuplicate;
    cv::Point       m_pointFps1;
    cv::Point       m_pointFps2;
    cv::Point       m_pointClock1;
    cv::Point       m_pointClock2;

public slots:
    void            updateFps(int);
    void            updateClock(QString);
    void            duplicateOutput(bool);
};

QImage mat2qimage(const cv::Mat &mat);
cv::Mat qimage2mat(const QImage &qimage);
cv::Mat autoWhiteBalance(const cv::Mat& imgin);

#endif // PROCESSTHREAD_H
