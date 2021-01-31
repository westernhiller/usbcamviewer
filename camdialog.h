#ifndef CAMDIALOG_H
#define CAMDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QMutex>

#include "global.h"
#include "camconfig.h"
#include "glcanvas.h"
#include "imagebuffer.h"
#include "capturethread.h"
#include "displaythread.h"
#include "photothread.h"
#include "videothread.h"
#include "processthread.h"

class CamDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CamDialog(QWidget* parent = nullptr);
    ~CamDialog();

    CamConfig* getConfig() { return m_pConfig;}

signals:
    void showWidget(WIDGET_ID);
    void updateFrame(QImage);
    void updateFps(int);
    void updateClock(QString);
    void recordVideo(bool);

private:
    GLCanvas*       m_pCanvas;
    QWidget*        m_pToolbar;
    CamConfig*      m_pConfig;
    CaptureThread*  m_pCapturer;
    DisplayThread*  m_pDisplayer;
    PhotoThread*    m_pPhotoSaver;
    VideoThread*    m_pVideoSaver;
    ImageBuffer*    m_pSourceBuffer;
    ImageBuffer*    m_pDisplayBuffer;
    ImageBuffer*    m_pVideoBuffer;
    QLabel*         m_pMessage;
    bool            m_bFullscreen;
    int             m_nFrames;
    QMutex          m_mutex;
    bool            m_bRecording;
    int             m_nTimeOut;
    int             m_nProcess;
    QTimer*         m_timerMessage;

    QList<ProcessThread*> m_processList;

    void    initGUI();
    void    showMessage(QString);

private slots:
    void gotFrame(QImage);
    void clickOnCanvas();
    void onTimer();
    void onTimerMessage();
    void onPhoto();
    void onPhotoSaved(QString);
    void onVideo();
    void onVideoSaved(QString);
    void onSettings();
    void onReplay();
    void restartCapturer();
};

#endif // CAMDIALOG_H
