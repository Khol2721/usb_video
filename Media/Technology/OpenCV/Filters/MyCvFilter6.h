#ifndef MYCVFILTER6_H
#define MYCVFILTER6_H

#include "Interface/MyCvFilter.h"

/// NOT (LPF), Scharr (HPF)!
class MyCvFilter6 : public MyCvFilter
{
public:
    MyCvFilter6(const PtrBilderCvFilters& ptrBilderCvFilters, bool isReverse = DEF_IS_REVERSE);
    virtual ~MyCvFilter6() = default;

    PtrMat Convert(const PtrMat mat) const;
};

#endif // MYCVFILTER6_H
