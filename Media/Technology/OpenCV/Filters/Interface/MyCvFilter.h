#ifndef MYCVFILTER_H
#define MYCVFILTER_H

#include <memory>

#include "../BildersCvFilters/Interface/IBilderCvFilters.h"

typedef std::shared_ptr<IBilderCvFilters> PtrBilderCvFilters;

#define DEF_IS_REVERSE false

enum class TypeLPF : unsigned char
{
    Zero = 0,
    Bilateral,
    AdaptiveBilateral,
    GaussianBlur,
    MedianBlur,
    Blur,
    Box
};

enum class TypeHPF : unsigned char
{
    Zero = 0,
    Canny,
    Sobel,
    Laplacian,
    Scharr,
    ColorMask
};

///Базовый класс для преобразования изображения в нужный формат (чертеж IBilderCvFilters)
class MyCvFilter
{
public:
    MyCvFilter(
            PtrBilderCvFilters ptrBilderCvFilters,
            TypeLPF typeLPF,
            TypeHPF typeHPF,
            bool isReverse,
            bool IsInputGrey,
            bool isOutputGrey);
    virtual ~MyCvFilter();

    ///Чертеж преобразования изображения (Паттерн Строитель)
    PtrMat Convert(const PtrMat mat) const;

    TypeLPF GetTypeLPF() const;
    TypeHPF GetTypeHPF() const;

    bool GetIsInputGrey() const;
    bool GetIsOutputGrey() const;

protected:
    const PtrBilderCvFilters ptrBilderCvFilters;

    TypeLPF typeLPF;
    TypeHPF typeHPF;

    const bool isReverse;
    const bool isInputGrey, isOutputGrey;

    virtual PtrMat CustomConvert(const PtrMat mat) const;
};

#endif // MYCVFILTER_H
