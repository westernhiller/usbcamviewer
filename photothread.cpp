#include "photothread.h"
#include <QDateTime>
#include <QDebug>
#include "camdialog.h"

PhotoThread::PhotoThread(ImageBuffer* pImageBuffer, QObject* parent)
    : QThread(parent)
    , m_pImageBuffer(pImageBuffer)
{
    m_pConfig = static_cast<CamDialog*>(parent)->getConfig();
}

PhotoThread::~PhotoThread()
{
    requestInterruption();
    quit();
    wait();
}

void PhotoThread::run()
{
    if(!m_pImageBuffer)
        return;

    while (!isInterruptionRequested())
    {
        FRAMEINFO* pFrame = m_pImageBuffer->readImage(true);
        if(pFrame && pFrame->matImg.data)
        {
            QDateTime timeNow = QDateTime::currentDateTime();
            QString time = timeNow.toString("yyyy-MM-dd-HH-mm-ss-zzz");
            QString filename = m_pConfig->pathToday() + tr("/") + time + ".png";
            imwrite(filename.toStdString(), pFrame->matImg);
            delete pFrame;
            emit photoSaved(QString(filename));
            break;
        }
        usleep(10000);
    }
}
