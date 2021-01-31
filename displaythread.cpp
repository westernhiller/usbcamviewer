#include "displaythread.h"
#include "global.h"
#include "processthread.h"

DisplayThread::DisplayThread(ImageBuffer* pImageBuffer, QObject* parent)
    : QThread(parent)
    , m_pImageBuffer(pImageBuffer)
{
}

DisplayThread::~DisplayThread()
{
    requestInterruption();
    quit();
    wait();
}

void DisplayThread::run()
{
    if(!m_pImageBuffer)
        return;

    while (!isInterruptionRequested())
    {
        FRAMEINFO* pFrame = m_pImageBuffer->readImage();
        if(pFrame)
        {
            QImage image = mat2qimage(pFrame->matImg);
            emit updateImage(image.copy());
            pFrame->matImg.release();
            delete pFrame;
        }
        usleep(10000);
    }
}
