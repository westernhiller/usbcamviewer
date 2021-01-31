#ifndef IMAGEVIEWER_DIALOG_H
#define IMAGEVIEWER_DIALOG_H

#include <QDialog>
#include <QLabel>
#include "global.h"
#include "glcanvas.h"

class ImageViewerDlg : public QDialog
{
    Q_OBJECT
public:
    explicit ImageViewerDlg(QString imgfile, QWidget* parent = nullptr);
    ~ImageViewerDlg();

signals:
    void updateFrame(QImage);

private:
    GLCanvas*       m_pCanvas;

private slots:
    void clickOnCanvas();
};

#endif // CAMDIALOG_H
