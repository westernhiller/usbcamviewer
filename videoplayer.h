#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QDialog>
#include <QTimer>

#include "global.h"
#include "glcanvas.h"

class VideoPlayer : public QDialog
{
    Q_OBJECT

public:
    VideoPlayer(QString videofile, QWidget *parent = nullptr);
    ~VideoPlayer() override;

signals:
    void updateFrame(QImage);

private:
    QString     m_videofile;
    GLCanvas*   m_pCanvas;
    cv::VideoCapture* m_pVideo;
    QTimer*     m_timer;

private slots:
    void onTimer();
    void clickOnCanvas();
};

#endif // VIDEOPLAYER_H
