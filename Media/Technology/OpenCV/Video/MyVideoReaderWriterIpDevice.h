#ifndef MYVIDEOREADERWRITERIPDEVICE_H
#define MYVIDEOREADERWRITERIPDEVICE_H

#include "MyVideoReaderWriterDevice.h"

class MyVideoReaderWriterIpDevice : public MyVideoReaderWriterDevice
{
    Q_OBJECT
public:
    MyVideoReaderWriterIpDevice(
            TypeWorkWriter typeWorkWriter,
            FunConvertMat funConvertMat,
            FunConvertImage funConvertImage,
            uint8_t defFps, QObject* parent = nullptr);
    virtual ~MyVideoReaderWriterIpDevice() = default;

    ushort GetFps() const override;

    struct IpInfo {
        QString toAdress;
        QString login;
        QString password;
    };
    static IpInfo G_GetIpInfo(const QString& path);

protected:
    bool CheckIsDisconnect() override;
};

#endif // MYVIDEOREADERWRITERIPDEVICE_H
