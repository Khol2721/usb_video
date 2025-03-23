#ifndef IBILDERCVFILTERS_H
#define IBILDERCVFILTERS_H

#include "../../../Primitives/DefinesOpenCV.h"

//TODO: требуется Settings!

typedef cv::Mat Mat;
typedef cv::Ptr<cv::Mat> PtrMat;

//TODO: По хорошему данный интерфейс следует вынести и разделить от OpenCV
//в пользу более универсального решения, но в этом нет необходимости (пока что...)!
class IBilderCvFilters
{
public:
    //Фильтры низних частот (LPF):
    virtual PtrMat GetImageBilateralFilter(const Mat& mat) const = 0;
    virtual PtrMat GetImageAdaptiveBilateralFilter(const Mat& mat) const = 0;
    virtual PtrMat GetImageGaussianBlur(const Mat& mat) const = 0;
    virtual PtrMat GetImageMedianBlur(const Mat& mat) const = 0;
    virtual PtrMat GetImageBlur(const Mat& mat) const = 0;
    virtual PtrMat GetImageBoxFilter(const Mat& mat) const = 0;

    //Фильтры верхних частот (HPF):
    virtual PtrMat GetImageCanny(const Mat& mat) const = 0;
    virtual PtrMat GetImageSobel(const Mat& mat) const = 0;
    virtual PtrMat GetImageLaplacian(const Mat& mat) const = 0;
    virtual PtrMat GetImageScharr(const Mat& mat) const = 0;

    ///Изменить черное на белое и наоборот!
    virtual PtrMat GetImageReverse(const Mat& mat) const = 0;

    ///Цветовая маска!
    virtual PtrMat GetImageColorMask(const Mat& mat) const = 0;
};

#endif // IBILDERCVFILTERS_H
