#ifndef MYCVFILTER4_H
#define MYCVFILTER4_H

#include "Interface/MyCvFilter.h"

/// BoxFilter (LPF), Canny (HPF)!
class MyCvFilter4 : public MyCvFilter
{
public:
    MyCvFilter4(const PtrBilderCvFilters& ptrBilderCvFilters, bool isReverse = DEF_IS_REVERSE);
    virtual ~MyCvFilter4() = default;

    PtrMat Convert(const PtrMat mat) const;
};

#endif // MYCVFILTER4_H
