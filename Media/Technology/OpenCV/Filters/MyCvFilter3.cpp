#include "MyCvFilter3.h"

MyCvFilter3::MyCvFilter3(const PtrBilderCvFilters& ptrBilderCvFilters, bool isReverse) :
    MyCvFilter(ptrBilderCvFilters, TypeLPF::Zero, TypeHPF::Zero, isReverse, false, false) { }

PtrMat MyCvFilter3::Convert(const PtrMat mat) const {

    auto out = ptrBilderCvFilters->GetImageMedianBlur(*mat);
    //out = ptrBilderCvFilters->GetImageCanny(*out);
    out = ptrBilderCvFilters->GetImageSobel(*out);

    out = MyCvFilter::Convert(out);

    return out;
}
