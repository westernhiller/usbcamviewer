#include "capturethread.h"
#include "global.h"
#include "camdialog.h"
#include "darkchannel.h"

#include "v4l2capture.h"
#include "turbojpeg.h"
#include "jpegdecode.h"

using namespace std;
using namespace cv;

CaptureThread::CaptureThread(QObject* parent)
    : QThread(parent)
    , m_pImageBuffer(nullptr)
    , m_bUpdateAtmosphericLight(false)
    , m_pVcap(nullptr)
    , m_bCameraValid(false)
    , m_nWidth(IMAGEWIDTH)
    , m_nHeight(IMAGEHEIGHT)
{
    m_pConfig = static_cast<CamDialog*>(parent)->getConfig();
    m_bCameraValid = initCamera();
}

CaptureThread::~CaptureThread()
{
    requestInterruption();
    if(m_pVcap)
        delete m_pVcap;
    quit();
    wait();
}

void CaptureThread::calcAtmosphericLight()
{
    m_bUpdateAtmosphericLight = true;
}

bool CaptureThread::initCamera()
{
    m_pVcap = new V4L2Capture((char *)m_pConfig->camId().toStdString().data(), m_pConfig->width(), m_pConfig->height());
    m_pVcap->openDevice();

    if(-1 == m_pVcap->initDevice(m_nWidth, m_nHeight))
    {
        qDebug() << "failed initializing camera " << m_pConfig->camId();
        return false;
    }
    else {
        qDebug() << "CaptureThread::initCamera() OK with sie of (" << m_nWidth << ", " << m_nHeight << ")";
    }
    m_pVcap->stopCapture();

    return true;
}

void CaptureThread::run()
{
    if(!m_pConfig || !m_pImageBuffer || !m_pVcap || !m_bCameraValid)
        return;

    m_pVcap->startCapture();
    unsigned char *yuv422frame = nullptr;
    unsigned long yuvframeSize = 0;
    double t1 = (double)getTickCount();
    while (!isInterruptionRequested())
    {
//qDebug() << "CaptureThread::run() ";
        if(-1 != m_pVcap->getFrame((void **) &yuv422frame, (int *)&yuvframeSize))
        {
qDebug() << "get a frame ";
            Mat imgFrame = Jpeg2Mat(yuv422frame, yuvframeSize);
            if(imgFrame.data)
            {
//qDebug() << "image valid (" << imgFrame.cols << "x" << imgFrame.rows << ")";
                FRAMEINFO frame;
                frame.matImg = imgFrame;
                frame.timeStamp = QDateTime::currentDateTime();
                m_pImageBuffer->writeImage(frame);
//                if(m_pImageBuffer->writeImage(frame))
//                    qDebug() << "capture write image ok";
//                else {
//                    qDebug() << "capture write image failed";
//                }

                if(m_bUpdateAtmosphericLight)
                {
                    m_bUpdateAtmosphericLight = false;
                    cv::Vec3f atmosphericLight = estimateAtmosphericLight(imgFrame);
                    emit updateAtmosphericLight(atmosphericLight[0], atmosphericLight[1], atmosphericLight[2]);
                }
                imgFrame.release();
            }

            m_pVcap->backFrame();
            double t2 = (double)getTickCount();
            qDebug() <<  time_diff(t1, t2) << " seconds";
            t1 = t2;
        }
        usleep(20000);
    }
}

