#include "videothread.h"
#include <QDateTime>
#include "camdialog.h"

using namespace std;
using namespace cv;

VideoThread::VideoThread(ImageBuffer* pImageBuffer, QObject* parent)
    : QThread(parent)
    , m_pImageBuffer(pImageBuffer)
    , m_pVideoSaver(nullptr)
    , m_framecnt(0)
{
    m_pConfig = static_cast<CamDialog*>(parent)->getConfig();
}

VideoThread::~VideoThread()
{
    requestInterruption();
    stopRecording();
    quit();
    wait();
}

void VideoThread::run()
{
    if(!m_pImageBuffer)
        return;

    startRecording();
    while(!isInterruptionRequested())
    {
        FRAMEINFO* pFrame = m_pImageBuffer->readImage();
        if(pFrame && pFrame->matImg.data)
        {
            m_mutex.lock();
            if(m_pVideoSaver)
                m_pVideoSaver->write(pFrame->matImg);
            m_mutex.unlock();

            if(m_framecnt++ == 15000)
            {
                stopRecording();
                startRecording();
            }
        }
        delete pFrame;
        usleep(10000);
    }
}

void VideoThread::startRecording()
{
    m_videofile = m_pConfig->pathToday() + tr("/") + QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss") + tr(".avi");
    m_mutex.lock();
    m_pVideoSaver = new VideoWriter(m_videofile.toStdString(),
//                                    VideoWriter::fourcc('M', 'J', 'P', 'G'),
                                    VideoWriter::fourcc('M', 'P', '4','2'),
                                    25,
                                    Size(m_pConfig->width(), m_pConfig->height()),
                                    true);

    m_framecnt = 0;
    m_mutex.unlock();
}

void VideoThread::stopRecording()
{
    bool bfinished = false;
    m_mutex.lock();
    if(m_pVideoSaver)
    {
        delete m_pVideoSaver;
        m_pVideoSaver = nullptr;
        bfinished = true;
    }
    m_mutex.unlock();

    if(bfinished)
        emit videoSaved(m_videofile);
}
