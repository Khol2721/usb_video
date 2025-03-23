#ifndef IMYVIDEOFRAME_H
#define IMYVIDEOFRAME_H

#include <QImage>
#include <QString>

#include <memory>

typedef std::shared_ptr<QImage> MyPtrImage;

class IMyVideoFrame
{
public:
    virtual ushort GetFps() const = 0;

    virtual bool GetIsOpened() const = 0;

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;

    virtual bool SetSizeFrame(QSize size) = 0;
};

#endif // IMYVIDEOFRAME_H
