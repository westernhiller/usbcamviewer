#include "videoplayer.h"
#include <QMessageBox>
#include <QTimer>
#include "global.h"
#include "processthread.h"

VideoPlayer::VideoPlayer(QString videofile, QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    QColor clearColor;
    clearColor.setHsv(255, 255, 63);

    m_pCanvas = new GLCanvas(this);
    m_pCanvas->setClearColor(clearColor);
    m_pCanvas->setGeometry(0, 0, SCREENWIDTH, SCREENHEIGHT);

    connect(m_pCanvas, SIGNAL(clicked()), this, SLOT(clickOnCanvas()));
    connect(this, SIGNAL(updateFrame(QImage)), m_pCanvas, SLOT(displayImage(QImage)));

    m_pVideo = new cv::VideoCapture(videofile.toStdString());
    if(m_pVideo && m_pVideo->isOpened())
    {
        m_timer = new QTimer(this);
        connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
        m_timer->start(40);
    }
}

VideoPlayer::~VideoPlayer()
{
    if(m_pVideo)
        delete m_pVideo;
}

void VideoPlayer::onTimer()
{
    cv::Mat frame;
    if(!m_pVideo->read(frame))
    {
        m_timer->stop();
        return;
    }

    QImage image = mat2qimage(frame);
    emit updateFrame(image);
}

void VideoPlayer::clickOnCanvas()
{
    close();
}

