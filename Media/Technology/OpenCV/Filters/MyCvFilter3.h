#ifndef MYCVFILTER3_H
#define MYCVFILTER3_H

#include "Interface/MyCvFilter.h"

/// MedianBlur (LPF), Canny (HPF)!
class MyCvFilter3 : public MyCvFilter
{
public:
    MyCvFilter3(const PtrBilderCvFilters& ptrBilderCvFilters, bool isReverse = DEF_IS_REVERSE);
    virtual ~MyCvFilter3() = default;

    PtrMat Convert(const PtrMat mat) const;
};

#endif // MYCVFILTER3_H
