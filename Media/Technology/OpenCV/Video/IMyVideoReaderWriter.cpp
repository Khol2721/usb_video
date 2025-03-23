#include "IMyVideoReaderWriter.h"

Q_DECLARE_METATYPE(std::shared_ptr<QImage>)
IMyVideoReaderWriter::IMyVideoReaderWriter(const QSize& sizeFrame, uint8_t defFps, QObject*parent) :
    QObject(parent), isRun(false), sizeFrame(sizeFrame), defFps(defFps) {
    qRegisterMetaType<std::shared_ptr<QImage>>();
}

MyVideoWriter& IMyVideoReaderWriter::GetMyVideoWriter() {
    return *myVideoWriter;
}

IMyVideoReaderWriter::InfoMat IMyVideoReaderWriter::GetInfoMat() {
    const std::lock_guard<std::mutex> lock(mutexMat);
    return infoMat;
}

void IMyVideoReaderWriter::SetInfoMat(cv::Ptr<cv::Mat> ptrMat, qint64 timeReadMatMs) {

    isNewMat = true;

    const std::lock_guard<std::mutex> lock(mutexMat);
    infoMat.ptrMat = ptrMat;
    infoMat.timeReadMatMs = timeReadMatMs;
}

