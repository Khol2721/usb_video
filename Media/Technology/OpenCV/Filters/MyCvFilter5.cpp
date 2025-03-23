#include "MyCvFilter5.h"

MyCvFilter5::MyCvFilter5(const PtrBilderCvFilters& ptrBilderCvFilters, bool isReverse) :
    MyCvFilter(ptrBilderCvFilters, TypeLPF::Zero, TypeHPF::Zero, isReverse, false, false) { }

PtrMat MyCvFilter5::Convert(const PtrMat mat) const {

    auto out = ptrBilderCvFilters->GetImageSobel(*mat);

    out = MyCvFilter::Convert(out);

    return out;
}
