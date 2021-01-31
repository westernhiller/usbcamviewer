#include "imageviewerdlg.h"
#include <QTimer>

#include "global.h"

using namespace std;
using namespace cv;

ImageViewerDlg::ImageViewerDlg(QString imgfile, QWidget* parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);

    QColor clearColor;
    clearColor.setHsv(255, 255, 63);

    m_pCanvas = new GLCanvas(QImage(imgfile), this);
    m_pCanvas->setClearColor(clearColor);
    m_pCanvas->setGeometry(0, 0, SCREENWIDTH, SCREENHEIGHT);

    connect(m_pCanvas, SIGNAL(clicked()), this, SLOT(clickOnCanvas()));
}

ImageViewerDlg::~ImageViewerDlg()
{
}

void ImageViewerDlg::clickOnCanvas()
{
    close();
}

