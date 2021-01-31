#ifndef IMAGE_READER_H
#define IMAGE_READER_H

#include <QThread>
#include <QImage>
#include <QTimer>

#include "global.h"

class ImageReader : public QThread
{
    Q_OBJECT
public:
    explicit ImageReader(QString imgfile, QObject* parent = nullptr);
    ~ImageReader();

signals:
    void gotImage(QString, QImage);
    void killMe();

protected:
    void            run();

private:
    QString     m_imgfile;

    bool isFileVideo(QString filename);
};

#endif // VIDEO_THUMB_H
