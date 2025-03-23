#include "MyCvFilter8.h"

MyCvFilter8::MyCvFilter8(const PtrBilderCvFilters& ptrBilderCvFilters, bool isReverse) :
    MyCvFilter(ptrBilderCvFilters, TypeLPF::Zero, TypeHPF::Zero, isReverse, false, false) { }

PtrMat MyCvFilter8::Convert(const PtrMat mat) const {

    auto out = ptrBilderCvFilters->GetImageColorMask(*mat);

    out = MyCvFilter::Convert(out);

    return out;
}
