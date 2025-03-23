#include "MyCvFilter2.h"

MyCvFilter2::MyCvFilter2(const PtrBilderCvFilters& ptrBilderCvFilters, bool isReverse) :
    MyCvFilter(ptrBilderCvFilters, TypeLPF::Zero, TypeHPF::Zero, isReverse, false, false) { }

PtrMat MyCvFilter2::Convert(PtrMat mat) const {

    auto out = ptrBilderCvFilters->GetImageGaussianBlur(*mat);
    out = ptrBilderCvFilters->GetImageCanny(*out);

    out = MyCvFilter::Convert(out);

    return out;
}
