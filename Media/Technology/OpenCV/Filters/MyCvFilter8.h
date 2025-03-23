#ifndef MYCVFILTER8_H
#define MYCVFILTER8_H

#include "Interface/MyCvFilter.h"

/// NOT (LPF), Laplacian (HPF)!
class MyCvFilter8 : public MyCvFilter
{
public:
    MyCvFilter8(const PtrBilderCvFilters& ptrBilderCvFilters, bool isReverse = DEF_IS_REVERSE);
    virtual ~MyCvFilter8() = default;

    PtrMat Convert(const PtrMat mat) const;
};

#endif // MYCVFILTER8_H
