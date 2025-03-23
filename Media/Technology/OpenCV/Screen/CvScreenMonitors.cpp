#include "CvScreenMonitors.h"

CvScreenMonitors::CvScreenMonitors() { targetFormat = QImage::Format::Format_Invalid; }

CvScreenMonitors::CvScreenMonitors(const QRect& geometry, QImage::Format format)
{
    SetFormat(format);
    SetGeometry(geometry);

    Init();
}

PtrMatScreen CvScreenMonitors::GetPtrMatScreen() {
    auto ptrMat = std::make_shared<MatScreen>();
    cvScreenShot->CreateScreenShot(*ptrMat);
    return ptrMat;
}

PtrImageScreen CvScreenMonitors::GetPtrImageScreen() {

    auto ptrMat = GetPtrMatScreen();
    PtrImageScreen ptrImageScreen = std::make_shared<QImage>(ConverterVariablesCV::CvMatToQImage(*ptrMat));

    PtrImageScreen result = nullptr;

    if(ptrImageScreen->format() != targetFormat) {
        result = std::make_shared<QImage>(ptrImageScreen->convertToFormat(targetFormat));
    }
    else {
        result = ptrImageScreen;
    }

    return result;
}

PtrImageScreen CvScreenMonitors::AsyncGetPtrImageScreen() {
    if(futureImageScreen.valid()) {
        //const auto timeStart = QDateTime::currentMSecsSinceEpoch();
        //futureImageScreen.wait();
        if(!future_is_ready(futureImageScreen)) {
            //qDebug() << "future NOT IS ready!";
            return ptrImageScreen;
        }
        //qDebug() << "TIME = " << QDateTime::currentMSecsSinceEpoch() - timeStart;
        auto result = futureImageScreen.get();
        futureImageScreen = std::async(std::launch::async, &CvScreenMonitors::GetPtrImageScreen, this);

        if(!result->isNull()) {
            ptrImageScreen = result;
        }

        //qDebug() << "future IS ready!"
        return result;
    }
    else {
        futureImageScreen = std::async(std::launch::async, &CvScreenMonitors::GetPtrImageScreen, this);
        return ptrImageScreen;
    }
}

bool CvScreenMonitors::Init() {

    if(!GetIsValid()) {
        return false;
    }

    cvScreenShot = new CvScreenShot(geometryScreen.x(), geometryScreen.y(), geometryScreen.width(), geometryScreen.height());
    ptrImageScreen = std::make_shared<QImage>(geometryScreen.width(), geometryScreen.height(), targetFormat);
    return true;
}

CvScreenMonitors::~CvScreenMonitors() {
    delete cvScreenShot;
}
