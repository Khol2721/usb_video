#ifndef IMYSCREENFRAME_H
#define IMYSCREENFRAME_H

#include <QImage>
#include <QString>

#include <memory>

typedef std::shared_ptr<QImage> MyPtrImage;

class IMyScreenFrame
{
public:
    virtual MyPtrImage GetScreenFrame(const QString& path) = 0;
};

#endif // IMYSCREENFRAME_H
