#ifndef CVSCREENMONITORS_H
#define CVSCREENMONITORS_H

#include <future>
#include <memory>

#include <QRect>
#include <QImage>
#include <QDateTime>//test!

#include <QApplication>
#include <QScreen>
#include <QPixmap>
#include <QPainter>

#include "../Primitives/ConverterVariablesCV.h"

#include "ICvScreenMonitors.h"

#include "CvScreenShot.h"

typedef std::shared_ptr<QImage> PtrImageScreen;

class CvScreenMonitors : public ICvScreenMonitors
{
public:
    CvScreenMonitors();
    CvScreenMonitors(const QRect& geometry, QImage::Format format);
    virtual ~CvScreenMonitors();

    PtrImageScreen GetPtrImageScreen() override;

    PtrImageScreen AsyncGetPtrImageScreen() override;

    PtrMatScreen GetPtrMatScreen() override;

    bool Init() override;

private:
    PtrImageScreen ptrImageScreen { nullptr };

    std::future<PtrImageScreen> futureImageScreen;

    CvScreenShot* cvScreenShot { nullptr };


    template<typename T>
    bool future_is_ready(std::future<T>& t) {
        return t.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }
};

#endif // CVSCREENMONITORS_H
