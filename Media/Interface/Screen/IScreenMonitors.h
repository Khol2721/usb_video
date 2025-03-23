#ifndef ISCREENMONITORS_H
#define ISCREENMONITORS_H

#include <memory>

#include <QImage>

class IScreenMonitors
{
public:
    IScreenMonitors() = default;
    virtual ~IScreenMonitors() = default;

    virtual bool Init() = 0;

    void SetGeometry(const QRect& geometry);
    const QRect& GetGeometry() const;

    void SetFormat(QImage::Format format);
    QImage::Format GetFormat() const;

    virtual bool GetIsValid() const;

    virtual std::shared_ptr<QImage> GetPtrImageScreen() = 0;

    virtual std::shared_ptr<QImage> AsyncGetPtrImageScreen() = 0;

protected:
    QRect geometryScreen;
    QImage::Format targetFormat;
};

#endif // ISCREENMONITORS_H
