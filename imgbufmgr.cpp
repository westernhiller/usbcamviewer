#include "remotech/imgbufmgr.h"
#include <string.h>
#include <stdio.h>

namespace remotech {

ImageBufferManager::ImageBufferManager()
{
}

ImageBufferManager::~ImageBufferManager()
{
    release();
}

void ImageBufferManager::release()
{
    for(vector<LPIMGBUF>::iterator it = m_listImageBuffer.begin(); it != m_listImageBuffer.end(); it++)
        delete *it;
    m_listImageBuffer.erase(m_listImageBuffer.begin(), m_listImageBuffer.end());
}

LPIMGBUF ImageBufferManager::addImageBuffer(string name, int width, int height, int nType, int num)
{
    LPIMGBUF pImgBuf = getImageBuffer(name);        // if imagebuffer already exists, delete it
    CRmLock lock(m_Lock);
    if(pImgBuf)
        delete pImgBuf;

    pImgBuf = new ImageBuffer(name, num, width, height, nType);
    m_listImageBuffer.push_back(pImgBuf);

    return pImgBuf;
}


LPIMGBUF ImageBufferManager::getImageBuffer(int index)
{
    CRmLock lock(m_Lock);
    if((index < 0) || (index >= (int)m_listImageBuffer.size()))
        return NULL;

    return m_listImageBuffer[index];
}

LPIMGBUF ImageBufferManager::getImageBuffer(string name)
{ 
    CRmLock lock(m_Lock);
    vector<LPIMGBUF>::iterator it = m_listImageBuffer.begin();

    while(it != m_listImageBuffer.end())
    {
        LPIMGBUF pImgBuf = (LPIMGBUF)(*it);
        if(pImgBuf->getName().compare(name) == 0)
            return pImgBuf;
        it++;
    }

    return NULL;
}

void ImageBufferManager::adjustResolution(int width, int height, string name)
{
    CRmLock lock(m_Lock);

    vector<LPIMGBUF>::iterator it = m_listImageBuffer.begin();

    while(it != m_listImageBuffer.end())
    {
        LPIMGBUF pImgBuf = (LPIMGBUF)(*it);
        if(pImgBuf->getName().compare(name) == 0)
        {
            pImgBuf->resize(width, height);
            break;
        }
        it++;
    }
}

}
