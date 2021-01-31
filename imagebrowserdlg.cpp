#include "imagebrowserdlg.h"
#include <QLayout>
#include <QToolTip>
#include <QMessageBox>
#include <QDir>
#include <QSettings>

#include "global.h"
#include "imagebutton.h"
#include "mainwindow.h"
#include "videoplayer.h"
#include "imageviewerdlg.h"

ImageBrowserDlg::ImageBrowserDlg(QWidget* parent)
    : QDialog(parent)
    , m_nRows(3)
    , m_nCols(3)
    , m_nCurrPage(0)
    , m_nSelected(0)
{
    setWindowFlags(Qt::FramelessWindowHint);
    m_pConfig = static_cast<MainWindow*>(parent)->getConfig();

    QColor clearColor;
    clearColor.setHsv(10, 255, 10);

    m_nImagePerPage = m_nRows * m_nCols;
    refreshImagelist();

    m_pGridCanvas = new QWidget(this);
    m_pGridCanvas->setGeometry(0, 0, SCREENWIDTH - 204, SCREENHEIGHT);
    QGridLayout* pGridLayout = new QGridLayout();

    int nMargin = 10;
    m_nCanvasWidth = (SCREENWIDTH - 204 - (m_nCols + 1) * nMargin) / m_nCols;
    m_nCanvasHeight = (SCREENHEIGHT - (m_nRows + 1) * nMargin) / m_nRows;

    for(int i = 0; i < m_nRows; i++)
    {
        for(int j = 0; j < m_nCols; j++)
        {
            ReplayCanvas* pCanvas = new ReplayCanvas(i, j, false, this);
            pCanvas->setGeometry((j + 1) * nMargin + j * m_nCanvasWidth, (i + 1) * nMargin + i * m_nCanvasHeight, m_nCanvasWidth, m_nCanvasHeight);
            connect(pCanvas, SIGNAL(clicked(int, int, bool)), this, SLOT(canvasClicked(int, int, bool)));
            m_listCanvas.append(pCanvas);
            pGridLayout->addWidget(pCanvas, i, j);
        }
    }
    m_pGridCanvas->setLayout(pGridLayout);

    QWidget *pToolbar = new QWidget(this);
    pToolbar->setGeometry(SCREENWIDTH - 204, 0, 204, SCREENHEIGHT);

    QVBoxLayout* pVLayout = new QVBoxLayout();
    ImageButton* pbtnReturn = new ImageButton(":/images/return.png", ":/images/returnpressed.png");
    pbtnReturn->setFixedWidth(204);
    pVLayout->addWidget(pbtnReturn);
    ImageButton* pbtnNext = new ImageButton(":/images/nextpage.png", ":/images/nextpagepressed.png");
    pbtnNext->setFixedWidth(204);
    pVLayout->addWidget(pbtnNext);
    ImageButton* pbtnPrev = new ImageButton(":/images/prevpage.png", ":/images/prevpagepressed.png");
    pbtnPrev->setFixedWidth(204);
    pVLayout->addWidget(pbtnPrev);
    ImageButton* pbtnErase = new ImageButton(":/images/erase.png", ":/images/erasepressed.png");
    pbtnErase->setFixedWidth(204);
    pVLayout->addWidget(pbtnErase);
    pToolbar->setLayout(pVLayout);

    connect(pbtnReturn, SIGNAL(clicked()), this, SLOT(goback()));
    connect(pbtnNext, SIGNAL(clicked()), this, SLOT(nextPage()));
    connect(pbtnPrev, SIGNAL(clicked()), this, SLOT(prevPage()));
    connect(pbtnErase, SIGNAL(clicked()), this, SLOT(onErase()));
}

ImageBrowserDlg::~ImageBrowserDlg()
{
}

void ImageBrowserDlg::displayPage(int nPage)
{
    m_filesOnCurrPage.clear();
    int nImagesOnPage = imagesOnPage(nPage);
    int idxStart = 0;
    if(nPage > 0)
        idxStart = m_nCols * m_nRows * nPage;
    for(int i = 0; i < nImagesOnPage; i++)
    {
        if(m_nSelected == i)
            m_listCanvas[i]->select();
        else
            m_listCanvas[i]->deselect();

        QString filename = m_filelist[idxStart + i];
        m_filesOnCurrPage.push_back(filename);
        ImageReader* pReader = new ImageReader(filename, this);
        m_imageReaderList.push_back(pReader);
        connect(pReader, SIGNAL(gotImage(QString, QImage)), this, SLOT(gotImage(QString, QImage)));
        connect(pReader, SIGNAL(killMe()), this, SLOT(killImageReader()));
        pReader->start();
    }
    for(int i = nImagesOnPage; i < m_nCols * m_nRows; i++)
        m_listCanvas[i]->clear();
}

void ImageBrowserDlg::reloadImages()
{
    refreshImagelist();
    m_nCurrPage = 0;
    displayPage(m_nCurrPage);
}

void ImageBrowserDlg::gotImage(QString imgfile, QImage image)
{
    for(int i = 0; i < m_filesOnCurrPage.size(); i++)
    {
        if(m_filesOnCurrPage[i].compare(imgfile) == 0)
        {
            m_listCanvas[i]->displayImage(image);
            break;
        }
    }
}

void ImageBrowserDlg::refreshImagelist()
{
    collectFiles();
    m_nTotalImages = m_filelist.size();
    m_nTotalPages = m_nTotalImages / m_nImagePerPage + 1;
    if(m_nTotalImages % m_nImagePerPage == 0)
        m_nTotalPages--;
}

int ImageBrowserDlg::canvasAt(const QPoint &pos)
{
    for(int i = 0; i < m_nRows; i++)
    {
        int y = i * m_nCanvasHeight;
        if(pos.y() - y <= m_nCanvasHeight)
        {
            for(int j = 0; j < m_nCols; j++)
            {
                int x = j * m_nCanvasWidth;

                if(pos.x() - x <= m_nCanvasWidth)
                {
                    int ret = i * m_nCols + j;
                    if(ret < m_filesOnCurrPage.size())
                        return ret;
                    else {
                        return -1;
                    }
                }
            }
        }
    }

    return -1;
}

bool ImageBrowserDlg::event(QEvent *event)
{
    if(event->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        int index = canvasAt(helpEvent->pos());
        if (index != -1) {
            QToolTip::showText(helpEvent->globalPos(), m_filesOnCurrPage[index]);
        } else {
            QToolTip::hideText();
            event->ignore();
        }
    }

    return QDialog::event(event);
}
int ImageBrowserDlg::imagesOnPage(int page)
{
    if(page >= m_nTotalPages)
        return 0;
    if(page < 0)
        return 0;

    if(page == m_nTotalPages-1)
        return m_nTotalImages - m_nImagePerPage * (m_nTotalPages - 1);

    return m_nImagePerPage;
}

bool ImageBrowserDlg::isFileVideo(QString filename)
{
    QString ext = filename.right(4);
    if(ext.compare(".avi") == 0)
        return true;

    if(ext.compare(".mp4") == 0)
        return true;

    return false;
}


void ImageBrowserDlg::canvasClicked(int row, int col, bool bSelected)
{
    int idx = row * m_nCols + col;
    if(idx >= m_filesOnCurrPage.size())
        return;

    if(bSelected)
    {
        QString filename = m_filesOnCurrPage[idx];
        if(isFileVideo(filename))
        {
            VideoPlayer vplayer(filename, this);
            vplayer.setFixedSize(SCREENWIDTH, SCREENHEIGHT);
            vplayer.exec();
        }
        else {
            ImageViewerDlg ivd(filename, this);
            ivd.setFixedSize(SCREENWIDTH, SCREENHEIGHT);
            ivd.exec();
        }
    }
    else if(m_listCanvas[m_nSelected]->isVisible())
    {
        m_listCanvas[m_nSelected]->deselect();
        m_listCanvas[idx]->select();
        m_nSelected = idx;
    }
}

void ImageBrowserDlg::collectFiles()
{
    m_filelist.clear();
    // 遍历目录
    QDir dir(m_pConfig->path());
    if(!dir.exists())
    {
        return;
    }
    QStringList imageList;
    foreach(QFileInfo mfi ,dir.entryInfoList(QDir::NoFilter, QDir::Name |QDir::Reversed))
    {
        if(!mfi.isFile())
        {
            if(mfi.fileName()=="." || mfi.fileName() == "..")
                continue;

            travelDir(mfi.absoluteFilePath(), imageList);
        }
    }
    m_filelist.append(imageList);
}

void ImageBrowserDlg::travelDir(QString path, QStringList& imagelist)
{
    QDir dir(path);
    QStringList filters;
    filters<<QString("*.avi")<<QString("*.mp4")<<QString("*.png")<<QString("*.jpg")<<QString("*.bmp");

    QStringList images = dir.entryList(filters, QDir::Files | QDir::NoSymLinks, QDir::Name |QDir::Reversed);
    foreach(QString filename, images)
    {
        imagelist.append(path + "/" + filename);
    }
}

void ImageBrowserDlg::nextPage()
{
    if(m_nCurrPage + 1 < m_nTotalPages)
        displayPage(++m_nCurrPage);
}

void ImageBrowserDlg::prevPage()
{
    if(m_nCurrPage > 0)
        displayPage(--m_nCurrPage);
}

void ImageBrowserDlg::onErase()
{
    QMessageBox *msg = new QMessageBox(QMessageBox::Information, "相机",
                             QStringLiteral("确定要删除选中的图像/视频文件吗？"),
                              QMessageBox::Yes | QMessageBox::No, this);
    QFont font;
    font.setBold(true);
    msg->setFont(font);

    if(QMessageBox::Yes == msg->exec())
    {
        int idx = m_nSelected + m_nCurrPage * m_nImagePerPage;
        QFile file(m_filelist[idx]);
        file.remove();
        m_filelist.removeAt(idx);
        int nImagesOnPage = imagesOnPage(m_nCurrPage);
        if(m_nSelected == 0)
        {
            if(nImagesOnPage == 1)
                if(m_nCurrPage > 0)
                    m_nCurrPage--;
        }
        else if(m_nSelected == nImagesOnPage - 1)
        {
            m_nSelected--;
        }
        m_nTotalImages = m_filelist.size();
        m_nTotalPages = m_nTotalImages / m_nImagePerPage + 1;
        if(m_nTotalImages % m_nImagePerPage == 0)
            m_nTotalPages--;
        displayPage(m_nCurrPage);
    }
}

void ImageBrowserDlg::goback()
{
    emit showWidget(WIDGET_MAIN);
}

void ImageBrowserDlg::killImageReader()
{
    ImageReader* pSender = (ImageReader *)sender();
    for(int i = 0; i < m_imageReaderList.size(); i++)
    {
        if(pSender == m_imageReaderList[i])
        {
            m_imageReaderList.removeAt(i);
            delete pSender;
            return;
        }
    }
}

