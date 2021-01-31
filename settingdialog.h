#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include "global.h"
#include "camconfig.h"

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = nullptr);
    ~SettingDialog();

signals:
    void showWidget(WIDGET_ID);
    void restart();

private:
    Ui::SettingDialog *ui;
    CamConfig* m_pConfig;
    QLineEdit*  m_pEditPath;

private slots:
    void onBrowse();
    void onReturn();
};

#endif // SETTINGDIALOG_H
