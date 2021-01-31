#ifndef DISPLAYTHREAD_H
#define DISPLAYTHREAD_H

#include <QThread>
#include <QImage>
#include "global.h"
#include "imagebuffer.h"

class DisplayThread : public QThread
{
    Q_OBJECT
public:
    explicit DisplayThread(ImageBuffer* pImageBuffer = nullptr, QObject* parent = nullptr);
    ~DisplayThread();

signals:
    void            updateImage(QImage);

protected:
    void            run();

private:
    ImageBuffer*    m_pImageBuffer;
};

#endif // CAPTURETHREAD_H
