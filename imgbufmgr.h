#ifndef IMAGEBUFFERMANAGER_H
#define IMAGEBUFFERMANAGER_H

#include <string>
#include "rmsys.h"
#include "imagebuffer.h"

using namespace std;

namespace remotech {

#define IMGBUFMGR_INST       ImageBufferManager::GetInstance()               // get singleton instance

class RMSHARED_EXPORT ImageBufferManager :public Singleton<ImageBufferManager>
{
public:
    friend class Singleton<ImageBufferManager>;

    ImageBufferManager();
    ~ImageBufferManager();

    LPIMGBUF addImageBuffer(string name, int width, int height, int nType, int num);

    LPIMGBUF getImageBuffer(int index);

    LPIMGBUF getImageBuffer(string name);

    void adjustResolution(int width, int height);

protected:
    vector<LPIMGBUF> m_listImageBuffer;
    vector<string> m_listImageBufferName;
};

}

#endif // FRAMEBUFFERMANAGER_H
