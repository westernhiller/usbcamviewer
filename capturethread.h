#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QThread>
#include <QImage>
#include "global.h"
#include "camconfig.h"
#include "imagebuffer.h"
#include "v4l2capture.h"

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    explicit CaptureThread(QObject* parent = nullptr);
    ~CaptureThread();

    inline void     setImageBuffer(ImageBuffer* pImageBuffer) { m_pImageBuffer = pImageBuffer;}
    inline bool     isCameraValid() { return m_bCameraValid;}
    inline int      width() { return m_nWidth;}
    inline int      height() { return m_nHeight;}
signals:
    void            updateImage(QImage);
    void            updateAtmosphericLight(float, float, float);

protected:
    void            run();

private:
    CamConfig*      m_pConfig;
    ImageBuffer*    m_pImageBuffer;
    bool            m_bUpdateAtmosphericLight;
    V4L2Capture *   m_pVcap;
    bool            m_bCameraValid;
    int             m_nWidth;
    int             m_nHeight;

    bool initCamera();
    std::tuple<bool,std::vector<uint8_t>,uint64_t,uint64_t,uint64_t>
        decodeJpeg2X(uint8_t* pJpegData,uint64_t JpegdataSize,const char* convrt_flag);

public slots:
    void calcAtmosphericLight();
};

#endif // CAPTURETHREAD_H
