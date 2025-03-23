#include "MyVideoFrame.h"

MyVideoFrame::MyVideoFrame() {
    videoCapture = nullptr;
}

uint MyVideoFrame::GetCountFramePlay() const {
    return static_cast<uint>(videoCapture->get(MY_CV_CAP_PROP_FRAME_COUNT));
}

uint MyVideoFrame::GetCurrentFramePlay() const {
    return static_cast<uint>(videoCapture->get(MY_CV_CAP_PROP_POS_FRAMES));
}

bool MyVideoFrame::SetCurrentFramePlay(uint posFrame) {
    return videoCapture->set(MY_CV_CAP_PROP_POS_FRAMES, posFrame);
    //какие удобные методы get/set (каждый флаг осуществляет свое действие)
}

MyPtrImage MyVideoFrame::GetScreenFrame(const QString& path) {

    if(videoCapture == nullptr) {
        if(!Init(path)) {
            return nullptr;
        }
    }

    return GetImage();
}

bool MyVideoFrame::Init(const QString& path) {

    //можно отключать чувствительность к регистру Qt::CaseInsensitive!
    if(!QString::compare(this->path, path)) {// == 0 совпали!
        return true;
    }

    if(videoCapture != nullptr) {
        Delete();
    }

    this->path = path;

    videoCapture = new cv::VideoCapture(path.toStdString());

    return GetIsOpened();
}

std::shared_ptr<QImage> MyVideoFrame::GetImage(uint currentFrame) {

    cv::Mat mat;
    bool isFrame = false;
    for(uint i = 0; i < currentFrame; i++) {
        isFrame = videoCapture->read(mat);
    }

    if(!isFrame || mat.empty()) {
        return nullptr;
    }

    auto ptrImage = std::make_shared<QImage>(ConverterVariablesCV::CvMatToQImage(mat));
    return ptrImage;
}

bool MyVideoFrame::GetIsOpened() const {

    const bool isOpened = videoCapture->isOpened();

    if (!isOpened) {
        //qDebug() << "videoCapture: Не удалось открыть видеофайл!";
        qDebug() << "videoCapture: No open video frame!";
    }

    return isOpened;
}

bool MyVideoFrame::SetSizeFrame(QSize size) {

    if(!size.isNull() && !size.isEmpty()) {

        //Установка разрешения, если камера его поддерживает! (флаги возврата videoCapture->set НЕ РАБОТАЮТ!!!!)

        if(videoCapture == nullptr) {
            return true;//сработает поздняя инициализация!
        }

        const auto videoW = GetWidth();
        if(videoW != size.width()) {
            videoCapture->set(MY_CV_CAP_PROP_FRAME_WIDTH, size.width());//3
        }

        //Замечание! Если увидешь впросы при выводе qDebug(), то это кирилица, которая не читается на удаленной машине!

        //! Нужно время на установку! (не успевает обновить разрешение)
        /*const auto newVideoW = GetWidth();
        if(newVideoW != size.width()) {
            qDebug() << "Не удалось установить разрешение изображения по ширине!";
            return false;
        }*/

        const auto videoH = GetHeight();
        if(videoH != size.height()) {
            videoCapture->set(MY_CV_CAP_PROP_FRAME_HEIGHT, size.height());//4
        }

        //! Нужно время на установку! (не успевает обновить разрешение)
        /*const auto newVideoH = GetHeight();
        if(newVideoH != size.height()) {
            qDebug() << "Не удалось установить разрешение изображения по высоте!";
            return false;
        }*/

        return true;
    }
    else {
        return false;
    }
}

inline int MyVideoFrame::GetWidth() const {
    return static_cast<int>(videoCapture->get(MY_CV_CAP_PROP_FRAME_WIDTH));
}

inline int MyVideoFrame::GetHeight() const {
    return static_cast<int>(videoCapture->get(MY_CV_CAP_PROP_FRAME_HEIGHT));
}

void MyVideoFrame::Delete() {
    videoCapture->release();//освобождение ресурсов!
    delete videoCapture;
    videoCapture = nullptr;
}

ushort MyVideoFrame::GetFps() const {
    return static_cast<ushort>(videoCapture->get(MY_CV_CAP_PROP_FPS));
}

MyVideoFrame::~MyVideoFrame() {
    Delete();
}
