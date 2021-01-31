#include "settingdialog.h"
#include "ui_settingdialog.h"
#include <QSettings>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

#include "mainwindow.h"
#include "imagebutton.h"

SettingDialog::SettingDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingDialog)
{
//    this->setStyleSheet("QFileDialog QWidget {font: 28pt;}");
    ui->setupUi(this);
    m_pConfig = static_cast<MainWindow*>(parent)->getConfig();

    ui->editPath->setText(m_pConfig->path());
    connect(ui->btnBrowse, SIGNAL(clicked()), this, SLOT(onBrowse()));
    ui->checkFPS->setChecked(m_pConfig->bFps());
    ui->checkClock->setChecked(m_pConfig->bClock());
    ui->checkDefog->setChecked(m_pConfig->bDefog());
    ui->comboBox->setCurrentIndex(m_pConfig->nResolution());

    ImageButton* pbtnReturn = new ImageButton(":/images/return.png", ":/images/returnpressed.png", true, this);
    pbtnReturn->setGeometry(1076, 596, 204, 204);

    connect(pbtnReturn, SIGNAL(clicked()), this, SLOT(onReturn()));
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::onBrowse()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::DirectoryOnly);
    fileDialog->setOption(QFileDialog::ShowDirsOnly, false);
    fileDialog->setWindowTitle(QStringLiteral("短波红外图像保存"));
    fileDialog->setDirectory(m_pConfig->path());
//    fileDialog->setAcceptMode(QFileDialog::AcceptSave);
    fileDialog->setViewMode(QFileDialog::Detail);
    if(fileDialog->exec() == QDialog::Accepted)
    {
        ui->editPath->setText(fileDialog->selectedFiles().first());
    }
}

void SettingDialog::onReturn()
{
    QString path = ui->editPath->text();
    bool bFPS = ui->checkFPS->isChecked();
    bool bClock = ui->checkClock->isChecked();
    bool bDefog = ui->checkDefog->isChecked();
    int nRes = ui->comboBox->currentIndex();

    bool bUpdated = false;
    if(m_pConfig->pathChanged(path))
    {
        bUpdated = true;
        m_pConfig->setPath(path);
    }
    if(m_pConfig->fpsChanged(bFPS))
    {
        m_pConfig->setFps(bFPS);
        bUpdated = true;
    }
    if(m_pConfig->clockChanged(bClock))
    {
        m_pConfig->setClock(bClock);
        bUpdated = true;
    }
    if(m_pConfig->defogChanged(bDefog))
    {
        m_pConfig->setDefog(bDefog);
        bUpdated = true;
    }
    if(m_pConfig->resolutionChanged(nRes))
    {
        QMessageBox *msg = new QMessageBox(QMessageBox::Information, "相机",
                                 QStringLiteral("分辨率调整重启后生效"),
                                 QMessageBox::Ok, this);
        msg->exec();
/*
        {
            m_pConfig->saveConfig();
            emit restart();
        }
        else
*/
        {
            CamConfig tempCfg(*m_pConfig);
            tempCfg.setResolution(nRes);
            tempCfg.saveConfig();
        }
    }
    else if(bUpdated)
    {
        m_pConfig->saveConfig();
    }


    emit showWidget(WIDGET_MAIN);
}
