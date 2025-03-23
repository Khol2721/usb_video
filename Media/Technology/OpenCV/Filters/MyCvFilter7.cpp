#include "MyCvFilter7.h"

MyCvFilter7::MyCvFilter7(const PtrBilderCvFilters& ptrBilderCvFilters, bool isReverse) :
    MyCvFilter(ptrBilderCvFilters, TypeLPF::Zero, TypeHPF::Zero, isReverse, false, false) { }

PtrMat MyCvFilter7::Convert(const PtrMat mat) const {

    auto out = ptrBilderCvFilters->GetImageLaplacian(*mat);

    out = MyCvFilter::Convert(out);

    return out;
}
