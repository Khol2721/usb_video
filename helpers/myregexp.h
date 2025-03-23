#ifndef MYREGEXP_H
#define MYREGEXP_H

#include <QRegExp>
#include <QVector>

class MyRegExp//нужно дорабатывать!
{
public:
    ///Оставляет полный текст после появления первых цифр! (первые цифры НЕ остаются)
    static QString GetRegExpLastNameFull_NotFirstNum(const QString& text);

    ///Оставляет полный текст после появления первых цифр! (первые цифры остаются)
    static QString GetRegExpLastNameFull_FirstNum(const QString& text);

    ///Оставляет первый непрерывный набор букв!
    static QString GetRegExpFirstName(const QString& text);

    ///Оставляет последний непрерывный набор цифр!
    static QString GetRegExpLastName(const QString& text);

    ///Оставляет последний непрерывный набор дробных цифр!
    static QString GetRegExpLastFractionalNum(const QString& text);

    ///Оставляет первый непрерывный набор дробных цифр!
    static QString GetRegExpFirstFractionalNum(const QString& text);

private:
    static QVector<QString> GetListRegExpName(const QString& text);
};

#endif // MYREGEXP_H
