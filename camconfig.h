#ifndef CAMCONFIG_H
#define CAMCONFIG_H

#include <QMutex>
#include "global.h"

class CamConfig
{
public:
    CamConfig(QString);
    CamConfig();
    CamConfig(CamConfig& rhs);
    ~CamConfig();

    bool    loadConfig(QString);
    bool    saveConfig();

    inline QString path() { return m_path;}
    inline QString pathToday() { return m_pathToday;}
    inline QString camId() { return m_camid;}
    inline bool bFps() { return m_bFPS;}
    inline bool bDefog() { return m_bDefog;}
    inline bool bClock() { return m_bClock;}
    inline int width() { return m_nWidth;}
    inline int height() { return m_nHeight;}
    inline int nResolution() { return m_nResolution;}

    inline void setFps(bool bFps) { m_bFPS = bFps;}
    inline void setDefog(bool bDefog) { m_bDefog = bDefog;}
    inline void setClock(bool bClock) { m_bClock = bClock;}
    inline void setPathToday(QString pathToday) { m_pathToday = pathToday;}
    void setResolution(int nRes);
    void setPath(QString path);

    inline bool pathChanged(QString path) { return m_path.compare(path) != 0;}
    inline bool fpsChanged(bool bfps) { return m_bFPS != bfps;}
    inline bool defogChanged(bool bdefog) { return m_bDefog != bdefog;}
    inline bool clockChanged(bool bClock) { return m_bClock != bClock;}
    inline bool resolutionChanged(int nResolution) { return m_nResolution != nResolution;}

private:
    QString m_cfgFile;
    QString m_path;
    QString m_pathToday;
    QString m_camid;
    int m_nWidth;
    int m_nHeight;
    int m_nResolution;
    bool m_bClock;
    bool m_bFPS;
    bool m_bDefog;
};

#endif // CAMCONFIG_H

