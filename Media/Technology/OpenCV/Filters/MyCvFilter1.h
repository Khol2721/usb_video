#ifndef MYCVFILTER1_H
#define MYCVFILTER1_H

#include "Interface/MyCvFilter.h"

/// Bilateral (LPF), Canny (HPF)!
class MyCvFilter1 : public MyCvFilter
{
public:
    MyCvFilter1(
            const PtrBilderCvFilters& ptrBilderCvFilters,
            bool isReverse = DEF_IS_REVERSE,
            bool IsInputGrey = true,
            bool isOutputGrey = true);

    virtual ~MyCvFilter1() = default;

    PtrMat Convert(const PtrMat mat) const;
};

#endif // MYCVFILTER1_H
