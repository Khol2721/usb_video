#include "MyCvFilter4.h"

MyCvFilter4::MyCvFilter4(const PtrBilderCvFilters& ptrBilderCvFilters, bool isReverse) :
    MyCvFilter(ptrBilderCvFilters, TypeLPF::Zero, TypeHPF::Zero, isReverse, false, false) { }

PtrMat MyCvFilter4::Convert(const PtrMat mat) const {

    auto out = ptrBilderCvFilters->GetImageBoxFilter(*mat);
    out = ptrBilderCvFilters->GetImageCanny(*out);

    out = MyCvFilter::Convert(out);

    return out;
}
