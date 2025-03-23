#ifndef IMYAUDIOREADER_H
#define IMYAUDIOREADER_H

#include <QString>

class IMyAudioReader
{
public:
    virtual void SetCodecOrDriver(const QString& name) = 0;
    virtual void SetPathFileOrDevice(const QString& path) = 0;

    virtual bool GetIsWork() const = 0;
};

#endif // IMYAUDIOREADER_H
