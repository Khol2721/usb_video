#include "MyCvFilterManager.h"

MyCvFilterManager::MyCvFilterManager() {

    ptrBilderCvFilters = std::make_shared<BilderCvFilters>();

    /*//Для Serval (default)!!!!
    mapMyCvFilter[TypeMyCvFilter::N_Zero] = new MyCvFilter(ptrBilderCvFilters);

    //Для 3М!!!!
    mapMyCvFilter[TypeMyCvFilter::N_1] = new MyCvFilter1(ptrBilderCvFilters, FILTER_IMAGE_FILTER_IMAGE_DEF_IS_REVERSE, false, false);*/
}

ushort MyCvFilterManager::AddFilter(TypeLPF typeLPF, TypeHPF typeHPF,
                                    bool isReverse,
                                    bool isInputGrey, bool isOutputGrey) {
    const ushort key = GetCountFilter();
    listFilter.append(new MyCvFilter(ptrBilderCvFilters,
                                     typeLPF, typeHPF,
                                     isReverse,
                                     isInputGrey, isOutputGrey));
    return key;
}

bool MyCvFilterManager::SetFilter(ushort key) {

    if(key >= GetCountFilter()) {
        return false;
    }

    keyCurrentFilter = key;
    return true;
}

ushort MyCvFilterManager::GetCountFilter() const {
    return listFilter.count();
}

ushort MyCvFilterManager::GetKeyCurrentFilter() const {
    return keyCurrentFilter;
}

const MyCvFilter& MyCvFilterManager::GetCurrentFilter() const {
    return *listFilter.at(keyCurrentFilter);
}

PtrMat MyCvFilterManager::Convert(PtrMat mat) const {
    return listFilter.at(keyCurrentFilter)->Convert(mat);
}

MyCvFilterManager::~MyCvFilterManager() {
    qDeleteAll(listFilter);
    listFilter.clear();
}
