#ifndef MYVIDEOWIDGET_H
#define MYVIDEOWIDGET_H

#include <QVideoWidget>

class MyVideoWidget : public QVideoWidget
{
    Q_OBJECT

public:
    MyVideoWidget(QWidget *parent = 0);
    ~MyVideoWidget();

signals:
    void clicked();

protected:
    virtual void mousePressEvent(QMouseEvent *ev);
};

#endif // VIDEOPLAYER_H
