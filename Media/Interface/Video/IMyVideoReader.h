#ifndef IMYVIDEOREADER_H
#define IMYVIDEOREADER_H

#include "IMyVideoFrame.h"
#include "IMyVideoPlayer.h"

enum class SystemConfigVideoDevice : quint8
{
    //Возьмем за основу механизм метода set из opencv!
    Default = 0,
    AnalogDevice = 1
    //... тут добавляем нужные режимы при модификации!
};

class IMyVideoReader : public IMyVideoFrame, public IMyVideoPlayer
{
public:
    virtual void SetVideoIdDevice(const QString& path, qint8 num) = 0;
    virtual void SetPathFileDevice(const QString& path) = 0;
    virtual void SetNumDevice(qint8 num) = 0;

    virtual bool GetIsPlay() const = 0;
    virtual bool GetIsWork() const = 0;

    virtual void SetSystemConfigVideoDevice(SystemConfigVideoDevice type, bool isWork) = 0;
    virtual bool GetSystemConfigVideoDevice(SystemConfigVideoDevice type) const = 0;
    virtual void InitSystemConfigVideoDevice() = 0;
};

#endif // IMYVIDEOREADER_H
