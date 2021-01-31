#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QKeyEvent>

#include "global.h"
#include "camconfig.h"
#include "settingdialog.h"
#include "camdialog.h"
#include "imagebrowserdlg.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString cfgFile, QWidget *parent = 0);
    ~MainWindow() override;

    CamConfig* getConfig() { return &m_config;}

signals:
    void reloadImages();

private:
    QStackedWidget* m_pStackedWidget;
    CamDialog*      m_pCamDlg;
    SettingDialog*  m_pSettingDlg;
    ImageBrowserDlg* m_pImageBrowser;
    CamConfig       m_config;

protected:
    void keyPressEvent(QKeyEvent* event) override;

public slots:
    void chooseWidget(WIDGET_ID nIndex);
    void restart();
};

#endif // MAINWINDOW_H
