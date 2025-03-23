#ifndef BILDERCVFILTERS_H
#define BILDERCVFILTERS_H

#include "Interface/IBilderCvFilters.h"

class BilderCvFilters : public IBilderCvFilters
{
public:
    BilderCvFilters() = default;
    virtual ~BilderCvFilters() = default;

    //Фильтры низних частот (LPF):
    PtrMat GetImageBilateralFilter(const Mat& mat) const override;
    PtrMat GetImageAdaptiveBilateralFilter(const Mat& mat) const override;
    PtrMat GetImageGaussianBlur(const Mat& mat) const override;
    PtrMat GetImageMedianBlur(const Mat& mat) const override;
    PtrMat GetImageBlur(const Mat& mat) const override;
    PtrMat GetImageBoxFilter(const Mat& mat) const override;

    //Фильтры высоких частот (HPF):
    PtrMat GetImageCanny(const Mat& mat) const override;
    PtrMat GetImageSobel(const Mat& mat) const override;
    PtrMat GetImageLaplacian(const Mat& mat) const override;
    PtrMat GetImageScharr(const Mat& mat) const override;

    ///Изменить черное на белое и наоборот!
    PtrMat GetImageReverse(const Mat& mat) const override;

    ///Цветовая маска!
    PtrMat GetImageColorMask(const Mat& mat) const override;
};

#endif // BILDERCVFILTERS_H
