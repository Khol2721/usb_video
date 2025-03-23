#include "MyCvFilter1.h"

MyCvFilter1::MyCvFilter1(const PtrBilderCvFilters& ptrBilderCvFilters,
                         bool isReverse,
                         bool IsInputGrey,
                         bool isOutputGrey) :
    MyCvFilter(ptrBilderCvFilters, TypeLPF::Bilateral, TypeHPF::Canny, isReverse, IsInputGrey, isOutputGrey) { }

PtrMat MyCvFilter1::Convert(const PtrMat mat) const {

    /*//Serval
    auto out = ptrBilderCvFilters->GetImageBilateralFilter(*mat);
    //out = ptrBilderCvFilters->GetImageCanny(*out);
    out = ptrBilderCvFilters->GetImageScharr(*out);*/

    //Pult3M
    auto out = ptrBilderCvFilters->GetImageBlur(*mat);

    out = MyCvFilter::Convert(out);

    return out;
}
