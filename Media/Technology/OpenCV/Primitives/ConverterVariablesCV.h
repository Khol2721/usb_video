#ifndef CONVERTERVARIABLESCV_H
#define CONVERTERVARIABLESCV_H

//Qt:
#include <QImage>
#include <QDebug>

//OpenCV:
#include <opencv2/opencv.hpp>

#include "DefinesOpenCV.h"

typedef cv::Mat Mat;
typedef cv::UMat UMat;

class ConverterVariablesCV
{
public:
    ConverterVariablesCV() = delete;

    static QImage CvMatToQImage(const Mat& inMat);
    static QImage CvMatToQImage(const UMat& inMat);

    //https://programmer.group/convert-qimage-to-cv-mat.html
    static Mat QImageToCvMat(const QImage& image, bool& isToRGB);
    //IplImage - устарел! (до opencv 2.0) (актуален cv::Mat)
};

#endif // CONVERTERVARIABLESCV_H
