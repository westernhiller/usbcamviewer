#include "processthread.h"
#include "global.h"
#include "camdialog.h"
#include <opencv2/xphoto.hpp>

using namespace std;
using namespace cv;

ProcessThread::ProcessThread(ImageBuffer* pImageBufferIn, ImageBuffer* pImageBufferOut, ImageBuffer* pImageBufferOut2, QObject* parent)
    : QThread(parent)
    , m_pImageBufferIn(pImageBufferIn)
    , m_pImageBufferOut(pImageBufferOut)
    , m_pImageBufferOut2(pImageBufferOut2)
    , m_nFontSize(3)
    , m_pointFps1(120, 50)
    , m_pointFps2(122, 52)
    , m_pointClock1(800, 50)
    , m_pointClock2(802, 52)
{
    m_pConfig = static_cast<CamDialog*>(parent)->getConfig();
    switch(m_pConfig->width())
    {
    case 1280:
        m_pointFps1.x = 80;
        m_pointClock1.x = 500;
        m_nFontSize = 3;
        break;
    case 1024:
        m_pointFps1.x = 50;
        m_pointClock1.x = 400;
        m_nFontSize = 2;
        break;
    case 640:
        m_pointFps1.x = 20;
        m_pointClock1.x = 200;
        m_nFontSize = 2;
        break;
    case 1920:
    default:
        m_pointFps1.x = 120;
        m_pointClock1.x = 800;
        m_nFontSize = 3;
        break;
    }
    m_pointFps2.x = m_pointFps1.x + 2;
    m_pointClock2.x = m_pointClock1.x + 2;
}

ProcessThread::~ProcessThread()
{
    requestInterruption();
    quit();
    wait();
}

void ProcessThread::duplicateOutput(bool bDuplicate)
{
    m_bDuplicate = bDuplicate;
}

void ProcessThread::updateClock(QString clock)
{
    m_clock = clock;
}

void ProcessThread::run()
{
    if(!m_pImageBufferIn || !m_pImageBufferOut)
        return;

    while (!isInterruptionRequested())
    {
        FRAMEINFO* pFrame = m_pImageBufferIn->readImage();
        if(pFrame)
        {
            if (m_pConfig)
            {
                if((pFrame->matImg.cols != m_pConfig->width()) || (pFrame->matImg.rows != m_pConfig->height()))
                    resize(pFrame->matImg, pFrame->matImg, Size(m_pConfig->width(), m_pConfig->height()));

                if(m_pConfig->bDefog())
                {
                    pFrame->matImg = m_defog.removeHaze(pFrame->matImg);
                }
                if(m_pConfig->bFps())
                {
                    cv::putText(pFrame->matImg, m_fps.toStdString(), m_pointFps1, FONT_HERSHEY_PLAIN, m_nFontSize, Scalar(255, 255, 255));
                    cv::putText(pFrame->matImg, m_fps.toStdString(), m_pointFps2, FONT_HERSHEY_PLAIN, m_nFontSize, Scalar(0, 0, 0));
                }
                if(m_pConfig->bClock())
                {
                    cv::putText(pFrame->matImg, m_clock.toStdString(), m_pointClock1, FONT_HERSHEY_PLAIN, m_nFontSize, Scalar(255, 255, 255));
                    cv::putText(pFrame->matImg, m_clock.toStdString(), m_pointClock2, FONT_HERSHEY_PLAIN, m_nFontSize, Scalar(0, 0, 0));
                }
            }
            m_pImageBufferOut->writeImage(*pFrame);
/*
 *             if(m_pImageBufferOut->writeImage(*pFrame))
            {
                qDebug() << "ProcessThread write to buffer out ok";
            }
            else {
                qDebug() << "ProcessThread write to buffer out failed";
            }
*/
            if(m_bDuplicate && m_pImageBufferOut2)
                m_pImageBufferOut2->writeImage(*pFrame);

            pFrame->matImg.release();
            delete pFrame;
        }
        usleep(10000);
    }
}

void ProcessThread::updateFps(int fps)
{
    m_fps = QString("FPS: %1").arg(fps);
}

QImage mat2qimage(const Mat &mat)
{
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);          // Set the color table (used to translate colour indexes to qRgb values)          image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }      // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

Mat qimage2mat(const QImage &qimage)
{
    Mat mat, mat2;
    int from_to[] = {0, 0, 1, 1, 2, 2, 3, 3};
    switch(qimage.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = Mat(qimage.height(), qimage.width(), CV_8UC4, (uchar *)qimage.constBits(), qimage.bytesPerLine());
        mat2 = Mat(mat.rows, mat.cols, CV_8UC4);
        mixChannels(&mat, 1, &mat2, 1, from_to, 4);
        return mat2;
        break;
    case QImage::Format_RGB888:
        mat = Mat(qimage.height(), qimage.width(), CV_8UC3, (uchar *)qimage.constBits(), qimage.bytesPerLine());
        mat2 = Mat(mat.rows, mat.cols, CV_8UC3);
        mixChannels(&mat, 1, &mat2, 1, from_to, 3);
        cvtColor(mat2, mat2, COLOR_BGR2RGB);
        return mat2;
        break;
    case QImage::Format_Indexed8:
        mat = Mat(qimage.height(), qimage.width(), CV_8UC1, (uchar *)qimage.constBits(), qimage.bytesPerLine());
        break;
    default:
        break;
    }
    return mat;
}

