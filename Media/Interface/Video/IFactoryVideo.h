#ifndef IFACTORYVIDEO_H
#define IFACTORYVIDEO_H

#include "IMyVideoFrame.h"
#include "IMyVideoReader.h"

///TODO: Планируется сделать абстрактную фабрику для подержки нескольких реализаций работы с видео!
class IFactoryVideo
{
public:
    virtual IMyVideoFrame* CreateMyVideoFrame() = 0;
    virtual IMyVideoFrame* CreateMyVideoReader() = 0;
};

#endif // IFACTORYVIDEO_H
