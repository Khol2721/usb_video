#ifndef MYMATCALIBRATION_H
#define MYMATCALIBRATION_H

#include <QString>
#include <QVector>

#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include "IImageConverter.h"

using namespace cv;

struct SettingsCalibration {
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    cv::Point pointCut;
    cv::Size sizeCut;
};

///Класс калибровки изображения в соответствии с зумом!
/// (требует предварительного формирования путей и файлов калибровки)
class MyMatCalibration : public IImageConverter
{
public:
    MyMatCalibration();
    virtual ~MyMatCalibration();

    void SetListZoomPath(const QVector<QString>& listZoomPath);

    void SetNumZoomPath(uint8_t numZoom, bool tfInit = false);

    PtrMat Convert(PtrMat ptrMat) const override;

    bool GetIsCalibration() const;

private:
    SettingsCalibration settingsCalibration;
    bool isCalibration;
    cv::Mat map1, map2;

    uint8_t numZoomPath;

    QVector<QString> listZoomPath;

    void InitCalibration(const QString& pathFileDevice);
};

#endif // MYMATCALIBRATION_H
