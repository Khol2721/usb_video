#ifndef MYCVFILTERMANAGER_H
#define MYCVFILTERMANAGER_H

#include <QVector>

#include "IImageConverter.h"
#include "../Filters/Interface/MyCvFilter.h"
#include "../Filters/BildersCvFilters/BilderCvFilters.h"

///Осуществляет переключение фильтров (My) по мере вызова пользователя!
class MyCvFilterManager : public IImageConverter
{
public:
    MyCvFilterManager();
    virtual ~MyCvFilterManager();

    ushort AddFilter(TypeLPF typeLPF, TypeHPF typeHPF,
                   bool isReverse,
                   bool isInputGrey, bool isOutputGrey);

    bool SetFilter(ushort key);

    ushort GetCountFilter() const;
    ushort GetKeyCurrentFilter() const;
    const MyCvFilter& GetCurrentFilter() const;

    PtrMat Convert(PtrMat mat) const override;

private:
    QVector<MyCvFilter*> listFilter;
    ushort keyCurrentFilter;

    PtrBilderCvFilters ptrBilderCvFilters;
};

#endif // MYCVFILTERMANAGER_H
