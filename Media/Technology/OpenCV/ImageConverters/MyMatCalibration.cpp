#include "MyMatCalibration.h"

MyMatCalibration::MyMatCalibration() {
    isCalibration = false;
    numZoomPath = 0;
}

void MyMatCalibration::SetListZoomPath(const QVector<QString>& listZoomPath) {
    this->listZoomPath = listZoomPath;
}

void MyMatCalibration::SetNumZoomPath(uint8_t numZoomPath, bool tfInit) {

    if(!tfInit) {
        if(this->numZoomPath == numZoomPath) {
            return;
        }
    }

    if(numZoomPath >= listZoomPath.count()) {
        return;
    }

    this->numZoomPath = numZoomPath;
    InitCalibration(listZoomPath[numZoomPath]);
}

PtrMat MyMatCalibration::Convert(PtrMat ptrMat) const {

    auto& mat = *ptrMat;

    PtrMat ptrOut = cv::makePtr<cv::Mat>();
    auto& out = *ptrOut;

    if(isCalibration) {
        if(map1.empty() || map2.empty()) {
            //Создаем карты калибровки!
            initUndistortRectifyMap(
                settingsCalibration.cameraMatrix,
                        settingsCalibration.distCoeffs,
                        Mat(),
                cv::getOptimalNewCameraMatrix(
                            settingsCalibration.cameraMatrix,
                            settingsCalibration.distCoeffs,
                            mat.size(), 1, mat.size(), 0),
                        mat.size(), CV_16SC2, map1, map2);
        }

        remap(mat, out, map1, map2, INTER_LINEAR);

        //Обрезаем часть изображения!
        out = out(cv::Rect(
                      settingsCalibration.pointCut.x,
                      settingsCalibration.pointCut.y,
                      settingsCalibration.sizeCut.width,
                      settingsCalibration.sizeCut.height));
    }

    return ptrOut;
}

void MyMatCalibration::InitCalibration(const QString& pathFileDevice) {

    if(pathFileDevice.isNull() || pathFileDevice.isEmpty()) {
        isCalibration = false;
    }
    else {
        //Ищем файл!
        cv::FileStorage fileStorage;

        //Пусть будет форман YAML, т.к. сокращенные записи 1. (1.0) выделяются ошибкой в json! (что вызывает сомнения)
        const bool isConfig = fileStorage.open(pathFileDevice.toStdString() + ".yaml", FileStorage::READ | FileStorage::FORMAT_YAML);

        if(isConfig) {
            //Считываем настройки!
            fileStorage["CameraMatrix"] >> settingsCalibration.cameraMatrix;
            fileStorage["DistCoeffs"] >> settingsCalibration.distCoeffs;
            fileStorage["PointCut"] >> settingsCalibration.pointCut;
            fileStorage["SizeCut"] >> settingsCalibration.sizeCut;

            isCalibration = true;
        }
        else {
            isCalibration = false;
        }
    }
}

bool MyMatCalibration::GetIsCalibration() const {
    return isCalibration;
}

MyMatCalibration::~MyMatCalibration() { }
