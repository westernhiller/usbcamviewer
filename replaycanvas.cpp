#include <QDir>
#include <QDebug>
#include <QLayout>
#include <QPainter>
#include <QPen>
#include "replaycanvas.h"

ReplayCanvas::ReplayCanvas(int row, int col, bool selected, QWidget *parent)
    : QLabel(parent)
    , m_nCol(col)
    , m_nRow(row)
    , m_bSelected(selected)
    , m_bShowImage(false)
{
}

ReplayCanvas::~ReplayCanvas()
{
}

void ReplayCanvas::mousePressEvent(QMouseEvent *ev)
{
    emit clicked(m_nRow, m_nCol, m_bSelected);
}

void ReplayCanvas::displayImage(QImage imgDisplay)
{
    m_bShowImage = true;
    m_pix = QPixmap::fromImage(imgDisplay.scaled(width(), height(), Qt::KeepAspectRatioByExpanding));
    update();
}

void ReplayCanvas::clear()
{
    m_bShowImage = false;
    update();
}

void ReplayCanvas::paintEvent(QPaintEvent *e)
{
    if(m_bShowImage)
    {
        QPainter p(this);
        if(m_bSelected)
        {
            QPen myPen(Qt::green, 5, Qt::SolidLine);
            p.setPen(myPen);
            p.drawRect(3, 3, width() - 5, height() - 5);
            p.drawPixmap(3, 3, width() - 5, height() - 5, m_pix);
        }
        else {
            p.drawPixmap(0, 0, width(), height(), m_pix);
        }
    }
}


void ReplayCanvas::select()
{
    m_bSelected = true;
    update();
}

void ReplayCanvas::deselect()
{
    m_bSelected = false;
    update();
}
