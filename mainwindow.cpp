#include "mainwindow.h"
#include <QSettings>
#include <QStandardPaths>
#include <QDateTime>
#include <QDir>

MainWindow::MainWindow(QString cfgFile, QWidget *parent)
    : QMainWindow(parent)
    , m_config(cfgFile)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setCursor(Qt::OpenHandCursor);

    m_pCamDlg = new CamDialog(this);
    m_pSettingDlg = new SettingDialog(this);
    m_pImageBrowser = new ImageBrowserDlg(this);

    m_pStackedWidget = new QStackedWidget(this);
    m_pStackedWidget->addWidget(m_pCamDlg);
    m_pStackedWidget->addWidget(m_pSettingDlg);
    m_pStackedWidget->addWidget(m_pImageBrowser);

    m_pStackedWidget->setCurrentWidget(m_pCamDlg);

    setCentralWidget(m_pStackedWidget);

    connect(m_pCamDlg, SIGNAL(showWidget(WIDGET_ID)), this, SLOT(chooseWidget(WIDGET_ID)));
    connect(m_pSettingDlg, SIGNAL(showWidget(WIDGET_ID)), this, SLOT(chooseWidget(WIDGET_ID)));
    connect(m_pImageBrowser, SIGNAL(showWidget(WIDGET_ID)), this, SLOT(chooseWidget(WIDGET_ID)));

    connect(m_pSettingDlg, SIGNAL(restart()), this, SLOT(restart()));
    connect(this, SIGNAL(reloadImages()), m_pImageBrowser, SLOT(reloadImages()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    int keyValue = event->key();
    if(keyValue & Qt::Key_Escape)
    {
        close();
    }
}

void MainWindow::restart()
{
    close();
}

void MainWindow::chooseWidget(WIDGET_ID nIndex)
{
    switch(nIndex)
    {
    case WIDGET_MAIN:
        m_pStackedWidget->setCurrentWidget(m_pCamDlg);
        break;
    case WIDGET_SETTINGS:
        m_pStackedWidget->setCurrentWidget(m_pSettingDlg);
        break;
    case WIDGET_BROWSER:
        emit reloadImages();
        m_pStackedWidget->setCurrentWidget(m_pImageBrowser);
        break;
    case WIDGET_INVALID:
    default:
        break;
    }
}

