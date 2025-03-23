#ifndef IMYAUDIOWRITER_H
#define IMYAUDIOWRITER_H

#include <QString>

class IMyAudioWriter
{
    virtual bool GetIsWork() const = 0;

    virtual bool Start() = 0;
    virtual void Stop() = 0;

    virtual void SetPathWrite(const QString& newPath) = 0;

    virtual QString GetNameFileWrite() const = 0;
};

#endif // IMYAUDIOWRITER_H
