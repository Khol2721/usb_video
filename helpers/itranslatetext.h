#ifndef ITRANSLATETEXT_H
#define ITRANSLATETEXT_H

#include <QString>

struct ITranslateText
{
    ITranslateText() = default;
    virtual ~ITranslateText() = default;

    const char* GetConstCharPtr(const QString& str) const {
        return G_GetConstCharPtr(str);
    }

    static const char* G_GetConstCharPtr(const QString& str) {
        return qUtf8Printable(str);//qPrintable - error!
    }
};

#endif // ITRANSLATETEXT_H
