#include "camconfig.h"
#include <QSettings>
#include <QStandardPaths>
#include <QDateTime>
#include <QDir>

CamConfig::CamConfig()
    : m_cfgFile("")
    , m_path("")
    , m_pathToday("")
    , m_camid("/dev/video0")
    , m_nWidth(1920)
    , m_nHeight(1080)
    , m_nResolution(0)
    , m_bClock(true)
    , m_bFPS(true)
    , m_bDefog(false)
{

}

CamConfig::CamConfig(CamConfig& rhs)
{
    this->m_cfgFile = rhs.m_cfgFile;
    this->m_bFPS = rhs.m_bFPS;
    this->m_path = rhs.m_path;
    this->m_pathToday = rhs.m_pathToday;
    this->m_camid = rhs.m_camid;
    this->m_bClock = rhs.m_bClock;
    this->m_bDefog = rhs.m_bDefog;
    this->m_nWidth = rhs.m_nWidth;
    this->m_nHeight = rhs.m_nHeight;
    this->m_nResolution = rhs.m_nResolution;
}

CamConfig::CamConfig(QString cfgfile)
    : m_cfgFile(cfgfile)
    , m_path("")
    , m_pathToday("")
    , m_camid("/dev/video0")
    , m_nWidth(1920)
    , m_nHeight(1080)
    , m_nResolution(0)
    , m_bClock(true)
    , m_bFPS(true)
    , m_bDefog(false)
{
    loadConfig(cfgfile);
}

CamConfig::~CamConfig()
{

}

void CamConfig::setPath(QString path)
{
    m_path = path;
    QDateTime timeNow = QDateTime::currentDateTime();
    m_pathToday = m_path + timeNow.toString("yyyyMMdd");

    QDir dir(m_pathToday);
    if (!dir.exists())
      dir.mkdir(m_pathToday);
}

bool CamConfig::loadConfig(QString cfgfile)
{
    QSettings settings(cfgfile, QSettings::NativeFormat);
    settings.beginGroup("common");
    m_path = settings.value("path", QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)).toString();
    while(m_path.right(1) == "/")
        m_path.chop(1);
    m_bFPS = settings.value("fps", "true").toBool();
    m_bClock = settings.value("clock", "true").toBool();
    m_bDefog = settings.value("defog", "false").toBool();
    m_camid = settings.value("camera", "/dev/video2").toString();
    QString resolution = settings.value("resolution", "1920x1080").toString();

    if(resolution.compare("1280x1024") == 0)
    {
        m_nResolution = 1;
        m_nWidth = 1280;
        m_nHeight = 1024;
    }
    else if(resolution.compare("1024x768") == 0)
    {
        m_nResolution = 2;
        m_nWidth = 1024;
        m_nHeight = 768;
    }
    else if(resolution.compare("640x480") == 0)
    {
        m_nResolution = 3;
        m_nWidth = 640;
        m_nHeight = 480;
    }
    else if(resolution.compare("1280x480") == 0)
    {
        m_nResolution = 4;
        m_nWidth = 1280;
        m_nHeight = 480;
    }
    else // if(resolution.compare("1920x1080") == 0)
    {
        m_nResolution = 0;
        m_nWidth = 1920;
        m_nHeight = 1080;
    }

    settings.endGroup();

    QDateTime timeNow = QDateTime::currentDateTime();
    m_pathToday = m_path + "/" + timeNow.toString("yyyyMMdd");

    QDir dir(m_pathToday);
    if (!dir.exists())
      dir.mkdir(m_pathToday);

    return true;
}

void CamConfig::setResolution(int nRes)
{
    m_nResolution = nRes;
    switch(nRes)
    {
    case 1:
        m_nWidth = 1280;
        m_nHeight = 1024;
        break;
    case 2:
        m_nWidth = 1024;
        m_nHeight = 768;
        break;
    case 3:
        m_nWidth = 640;
        m_nHeight = 480;
        break;
    case 4:
        m_nWidth = 1280;
        m_nHeight = 480;
        break;
    case 0:
    default:
        m_nWidth = 1920;
        m_nHeight = 1080;
        break;
    }
}

bool CamConfig::saveConfig()
{
    QSettings settings(m_cfgFile, QSettings::NativeFormat);
    settings.beginGroup("common");
    settings.setValue("path", m_path);
    settings.setValue("fps", m_bFPS);
    settings.setValue("clock", m_bClock);
    settings.setValue("camera", m_camid);
    settings.setValue("defog", m_bDefog);
    QString resolution = QString("%1x%2").arg(m_nWidth).arg(m_nHeight);
    settings.setValue("resolution", resolution);
    settings.endGroup();

    return true;
}

