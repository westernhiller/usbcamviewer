#ifndef IMAGEBROWSER_DIALOG_H
#define IMAGEBROWSER_DIALOG_H

#include <QDialog>
#include <QLabel>
#include "global.h"
#include "camconfig.h"
#include "replaycanvas.h"
#include "imagereader.h"

class ImageBrowserDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ImageBrowserDlg(QWidget* parent = nullptr);
    ~ImageBrowserDlg();

signals:
    void showWidget(WIDGET_ID);
    void erase();
    void eraseButtonClicked();
    void clearCanvas();

private:
    CamConfig* m_pConfig;
    int m_nRows;
    int m_nCols;
    int m_nCurrPage;
    int m_nTotalImages;
    int m_nTotalPages;
    int m_nImagePerPage;
    int m_nSelected;
    QString m_cfgFile;
    QWidget* m_pGridCanvas;
    int m_nCanvasWidth;
    int m_nCanvasHeight;

    QList<ReplayCanvas* > m_listCanvas;
    QList<ImageReader*> m_imageReaderList;

    QStringList m_filelist;
    QStringList m_filesOnCurrPage;

    void loadConfig(QString cfgFile);
    void collectFiles();
    void travelDir(QString path, QStringList& imagelist);
    int imagesOnPage(int page);
    bool isFileVideo(QString filename);
    void displayPage(int);
    void refreshImagelist();
    int canvasAt(const QPoint &pos);

protected:
    bool event(QEvent *event) override;

private slots:
    void nextPage();
    void prevPage();
    void goback();
    void onErase();
    void canvasClicked(int row, int col, bool bSelected);
    void killImageReader();
    void gotImage(QString imgfile, QImage image);
    void reloadImages();
};

#endif // CAMDIALOG_H
