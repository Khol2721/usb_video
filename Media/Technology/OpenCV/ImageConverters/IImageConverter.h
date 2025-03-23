#ifndef IIMAGECONVERTER_H
#define IIMAGECONVERTER_H

#include <opencv2/opencv.hpp>

typedef cv::Ptr<cv::Mat> PtrMat;

class IImageConverter
{
    virtual PtrMat Convert(PtrMat mat) const = 0;
};

#endif // IIMAGECONVERTER_H
