#include "MyCvFilter6.h"

MyCvFilter6::MyCvFilter6(const PtrBilderCvFilters& ptrBilderCvFilters, bool isReverse) :
    MyCvFilter(ptrBilderCvFilters, TypeLPF::Zero, TypeHPF::Zero, isReverse, false, false) { }

PtrMat MyCvFilter6::Convert(const PtrMat mat) const {

    //auto out = ptrBilderCvFilters->GetImageBoxFilter(mat);

    auto out = ptrBilderCvFilters->GetImageScharr(*mat);

    out = MyCvFilter::Convert(out);

    return out;
}
