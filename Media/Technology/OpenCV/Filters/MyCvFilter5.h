#ifndef MYCVFILTER5_H
#define MYCVFILTER5_H

#include "Interface/MyCvFilter.h"

/// NOT (LPF), Sobel (HPF)!
class MyCvFilter5 : public MyCvFilter
{
public:
    MyCvFilter5(const PtrBilderCvFilters& ptrBilderCvFilters, bool isReverse = DEF_IS_REVERSE);
    virtual ~MyCvFilter5() = default;

    PtrMat Convert(const PtrMat mat) const;
};

#endif // MYCVFILTER5_H
