#include "camdialog.h"
#include <QLayout>
#include <QDir>

#include "imagebutton.h"
#include "mainwindow.h"
#include "dehaze.h"

CamDialog::CamDialog(QWidget* parent)
    : QDialog(parent)
    , m_pPhotoSaver(nullptr)
    , m_pVideoSaver(nullptr)
    , m_bFullscreen(true)
    , m_nFrames(0)
    , m_bRecording(false)
    , m_nProcess(3)
{
    setWindowFlags(Qt::FramelessWindowHint);
    m_pConfig = static_cast<MainWindow*>(parent)->getConfig();

    m_pCapturer = new CaptureThread(this);
    qDebug() << "camera image size (" << m_pCapturer->width() << ", " << m_pCapturer->height() << ")";
    m_pSourceBuffer = new ImageBuffer("source", 10, m_pCapturer->width(), m_pCapturer->height(), CV_8UC3);
    m_pCapturer->setImageBuffer(m_pSourceBuffer);

    m_pVideoBuffer = new ImageBuffer("video", 10, m_pConfig->width(), m_pConfig->height(), CV_8UC3);
    m_pDisplayBuffer = new ImageBuffer("display", 5, m_pConfig->width(), m_pConfig->height(), CV_8UC3);

    m_pDisplayer = new DisplayThread(m_pDisplayBuffer, this);

    initGUI();

    connect(m_pDisplayer, SIGNAL(updateImage(QImage)), this, SLOT(gotFrame(QImage)));

    m_pDisplayer->start();

    for(int i = 0; i < m_nProcess; i++)
    {
        ProcessThread* pProcessor = new ProcessThread(m_pSourceBuffer, m_pDisplayBuffer, m_pVideoBuffer, this);
        connect(this, SIGNAL(updateFps(int)), pProcessor, SLOT(updateFps(int)));
        connect(this, SIGNAL(updateClock(QString)), pProcessor, SLOT(updateClock(QString)));
        connect(m_pCapturer, SIGNAL(updateAtmosphericLight(float, float, float)), pProcessor->getDehaze(), SLOT(updateAtmosphericLight(float, float, float)));
        connect(this, SIGNAL(recordVideo(bool)), pProcessor, SLOT(duplicateOutput(bool)));
        m_processList.push_back(pProcessor);
        pProcessor->start();
    }
    m_pCapturer->start();

    QTimer* t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this, SLOT(onTimer()));
    t->start(1000);

    QTimer* t2 = new QTimer(this);
    connect(t2, SIGNAL(timeout()), m_pCapturer, SLOT(calcAtmosphericLight()));
    t2->start(60000);
}

CamDialog::~CamDialog()
{
    delete m_pCapturer;
    delete m_pDisplayer;
    for(int i = 0; i < m_processList.size(); i++)
        delete m_processList[i];
    m_processList.clear();
    if(m_pPhotoSaver)
        delete m_pPhotoSaver;
    if(m_pVideoSaver)
        delete m_pVideoSaver;

    delete m_pSourceBuffer;
    delete m_pDisplayBuffer;
}

void CamDialog::restartCapturer()
{
    delete m_pCapturer;
    m_pCapturer = new CaptureThread(this);
    m_pCapturer->start();
}

void CamDialog::showMessage(QString msg)
{
    m_pMessage->setText(msg);
    m_pMessage->show();
    m_timerMessage->start(3000);
}

void CamDialog::onTimer()
{
    m_mutex.lock();
    if(m_pConfig->bFps())
        emit updateFps(m_nFrames);
    m_nFrames = 0;

    if(m_pConfig->bClock())
    {
        QDateTime timeNow = QDateTime::currentDateTime();
        QString clock = timeNow.toString("yyyy-MM-dd-HH-mm-ss");
        QString pathToday = m_pConfig->path() + "/" + timeNow.toString("yyyyMMdd");
        if(pathToday != m_pConfig->pathToday())
        {
            m_pConfig->setPathToday(pathToday);
            QDir dir(pathToday);
            if (!dir.exists())
              dir.mkdir(pathToday);
        }
        emit updateClock(clock);
    }
    m_mutex.unlock();
}

void CamDialog::initGUI()
{
    QColor clearColor;
    clearColor.setHsv(255, 255, 63);

    m_pCanvas = new GLCanvas(this);
    m_pCanvas->setClearColor(clearColor);
    m_pCanvas->setGeometry(0, 0, SCREENWIDTH, SCREENHEIGHT);

    connect(m_pCanvas, SIGNAL(clicked()), this, SLOT(clickOnCanvas()));
    connect(this, SIGNAL(updateFrame(QImage)), m_pCanvas, SLOT(displayImage(QImage)));

    m_pMessage = new QLabel(this);
    m_pMessage->setGeometry(100, SCREENHEIGHT - 40, SCREENWIDTH - 100, 40);
    m_pMessage->setStyleSheet("QLabel { background:transparent; font: 14pt; color: blue }");
    m_pMessage->hide();

    m_timerMessage = new QTimer(this);
    connect(m_timerMessage, SIGNAL(timeout()), this, SLOT(onTimerMessage()));

    m_pToolbar = new QWidget(this);
    m_pToolbar->setGeometry(SCREENWIDTH - 204, 0, 204, SCREENHEIGHT);

    QVBoxLayout* pVLayout = new QVBoxLayout();
    ImageButton* pbtnPhoto = new ImageButton(":/images/photo.png", ":/images/photopressed.png");
    pbtnPhoto->setFixedWidth(204);
    pVLayout->addWidget(pbtnPhoto);
    ImageButton* pbtnVideo = new ImageButton(":/images/record.png", ":/images/stop.png", false);
    pbtnVideo->setFixedWidth(204);
    pVLayout->addWidget(pbtnVideo);
    ImageButton* pbtnSettings = new ImageButton(":/images/settings.png", ":/images/settingspressed.png");
    pbtnSettings->setFixedWidth(204);
    pVLayout->addWidget(pbtnSettings);
    ImageButton* pbtnReplay = new ImageButton(":/images/replay.png", ":/images/replaypressed.png");
    pbtnReplay->setFixedWidth(204);
    pVLayout->addWidget(pbtnReplay);
    m_pToolbar->setLayout(pVLayout);
    m_pToolbar->hide();

    connect(pbtnPhoto, SIGNAL(clicked()), this, SLOT(onPhoto()));
    connect(pbtnVideo, SIGNAL(clicked()), this, SLOT(onVideo()));
    connect(pbtnReplay, SIGNAL(clicked()), this, SLOT(onReplay()));
    connect(pbtnSettings, SIGNAL(clicked()), this, SLOT(onSettings()));
}

void CamDialog::onVideo()
{
    m_bRecording = !m_bRecording;
    if(m_bRecording)
    {
        m_pVideoBuffer->cleanUp();
        m_pVideoSaver = new VideoThread(m_pVideoBuffer, this);
        connect(m_pVideoSaver, SIGNAL(videoSaved(QString)), this, SLOT(onVideoSaved(QString)));
        emit recordVideo(true);
        m_pVideoSaver->start();
    }
    else {
        if(m_pVideoSaver)
        {
            delete m_pVideoSaver;
            m_pVideoSaver = nullptr;
        }
        emit recordVideo(false);
    }
}

void CamDialog::onTimerMessage()
{
    m_timerMessage->stop();
    m_pMessage->hide();
}

void CamDialog::onVideoSaved(QString videofile)
{
    showMessage(videofile + QStringLiteral(" 已保存."));
}

void CamDialog::onPhoto()
{
    m_pPhotoSaver = new PhotoThread(m_pDisplayBuffer, this);
    connect(m_pPhotoSaver, SIGNAL(photoSaved(QString)), this, SLOT(onPhotoSaved(QString)));
    m_pPhotoSaver->start();
}

void CamDialog::onPhotoSaved(QString photofile)
{
    showMessage(photofile + QStringLiteral(" 已保存."));
    if(m_pPhotoSaver)
    {
        delete m_pPhotoSaver;
        m_pPhotoSaver = nullptr;
    }
}

void CamDialog::onSettings()
{
    emit showWidget(WIDGET_SETTINGS);
}

void CamDialog::onReplay()
{
    emit showWidget(WIDGET_BROWSER);
}

void CamDialog::gotFrame(QImage image)
{
    emit updateFrame(image);
    m_mutex.lock();
    m_nFrames++;
    m_mutex.unlock();
}

void CamDialog::clickOnCanvas()
{
    m_bFullscreen = !m_bFullscreen;
    if(m_bFullscreen)
        m_pToolbar->hide();
    else
        m_pToolbar->show();
}
