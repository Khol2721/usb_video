#ifndef MYCVFILTER7_H
#define MYCVFILTER7_H

#include "Interface/MyCvFilter.h"

/// NOT (LPF), Laplacian (HPF)!
class MyCvFilter7 : public MyCvFilter
{
public:
    MyCvFilter7(const PtrBilderCvFilters& ptrBilderCvFilters, bool isReverse = DEF_IS_REVERSE);
    virtual ~MyCvFilter7() = default;

    PtrMat Convert(const PtrMat mat) const;
};

#endif // MYCVFILTER7_H
