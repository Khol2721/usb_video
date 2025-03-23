#include "MyCvFilter.h"

MyCvFilter::MyCvFilter(
        PtrBilderCvFilters ptrBilderCvFilters,
        TypeLPF typeLPF,
        TypeHPF typeHPF,
        bool isReverse,
        bool isInputGrey,
        bool isOutputGrey) :
    ptrBilderCvFilters(ptrBilderCvFilters),
    typeLPF(typeLPF),
    typeHPF(typeHPF),
    isReverse(isReverse),
    isInputGrey(isInputGrey),
    isOutputGrey(isOutputGrey)
{ }

PtrMat MyCvFilter::Convert(const PtrMat mat) const {

    auto c_mat = CustomConvert(mat);

    if(!isReverse) {
        return c_mat;
    }
    else {
        auto out = ptrBilderCvFilters->GetImageReverse(*c_mat);
        return out;
    }
}

TypeLPF MyCvFilter::GetTypeLPF() const {
    return typeLPF;
}

TypeHPF MyCvFilter::GetTypeHPF() const {
    return typeHPF;
}

bool MyCvFilter::GetIsInputGrey() const {
    return isInputGrey;
}

bool MyCvFilter::GetIsOutputGrey() const {
    return isOutputGrey;
}

PtrMat MyCvFilter::CustomConvert(const PtrMat mat) const
{
    PtrMat out { mat };

    switch (typeLPF) {
    case TypeLPF::Bilateral:
        out = ptrBilderCvFilters->GetImageBilateralFilter(*mat);
        break;
    case TypeLPF::AdaptiveBilateral:
        out = ptrBilderCvFilters->GetImageAdaptiveBilateralFilter(*mat);
        break;
    case TypeLPF::GaussianBlur:
        out = ptrBilderCvFilters->GetImageGaussianBlur(*mat);
        break;
    case TypeLPF::MedianBlur:
        out = ptrBilderCvFilters->GetImageMedianBlur(*mat);
        break;
    case TypeLPF::Blur:
        out = ptrBilderCvFilters->GetImageBlur(*mat);
        break;
    case TypeLPF::Box:
        out = ptrBilderCvFilters->GetImageBoxFilter(*mat);
        break;
    default:
        break;
    }


    switch (typeHPF) {
    case TypeHPF::Canny:
        out = ptrBilderCvFilters->GetImageCanny(*out);
        break;
    case TypeHPF::Sobel:
        out = ptrBilderCvFilters->GetImageSobel(*out);
        break;
    case TypeHPF::Laplacian:
        out = ptrBilderCvFilters->GetImageLaplacian(*out);
        break;
    case TypeHPF::Scharr:
        out = ptrBilderCvFilters->GetImageScharr(*out);
        break;
    default:
        break;
    }

    return out;
}

MyCvFilter::~MyCvFilter() { }
