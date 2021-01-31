#include "myvideowidget.h"

MyVideoWidget::MyVideoWidget(QWidget *parent)
    : QVideoWidget(parent)
{
}

MyVideoWidget::~MyVideoWidget()
{

}

void MyVideoWidget::mousePressEvent(QMouseEvent *event)
{
    emit clicked();
}

