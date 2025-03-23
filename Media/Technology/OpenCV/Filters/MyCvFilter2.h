#ifndef MYCVFILTER2_H
#define MYCVFILTER2_H

#include "Interface/MyCvFilter.h"

/// GaussianBlur (LPF), Canny (HPF)!
class MyCvFilter2 : public MyCvFilter
{
public:
    MyCvFilter2(const PtrBilderCvFilters& ptrBilderCvFilters, bool isReverse = DEF_IS_REVERSE);
    virtual ~MyCvFilter2() = default;

    PtrMat Convert(const PtrMat mat) const;
};

#endif // MYCVFILTER2_H
