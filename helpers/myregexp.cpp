#include "myregexp.h"

QString MyRegExp::GetRegExpLastNameFull_NotFirstNum(const QString &text)
{
    QRegExp reg = QRegExp("[0-9]+");
    QString result = "";

    int del = 0;
    int pos = 0;
    if((pos = reg.indexIn(text, 0)) != -1)
    {
        result = text;
        del = reg.cap(0).count() + pos;
        result = result.remove(0, del);
    }
    else result = text;

    return result;
}

QString MyRegExp::GetRegExpLastNameFull_FirstNum(const QString &text)
{
    QRegExp reg = QRegExp("[0-9]+");
    QString result = "";

    int pos = 0;
    if((pos = reg.indexIn(text, 0)) != -1)
    {
        result = text.mid(pos);
    }
    else {
        result = text;
    }

    return result;
}

QString MyRegExp::GetRegExpFirstName(const QString& text)
{
    auto list = GetListRegExpName(text);

    if(list.count() > 0)
        return list[0];//берем первую строку!
    else
        return nullptr;//TODO: Вызовем ошибку если что не так! (throw)
}

QVector<QString> MyRegExp::GetListRegExpName(const QString& text) {
    QRegExp reg = QRegExp("^[a-zA-Z]*");

    int pos = 0;
    QVector<QString> list;

    while((pos = reg.indexIn(text, pos)) != -1)
    {
        list << (reg.cap(0));
        pos += reg.matchedLength();
    }

    return list;
}

QString MyRegExp::GetRegExpLastName(const QString& text)
{
    auto list = GetListRegExpName(text);

    if(list.count() > 0)
        return list[list.count() - 1];//берем последние цифры!
    else
        return nullptr;//TODO: Вызовем ошибку если что не так! (throw)
}

QString MyRegExp::GetRegExpLastFractionalNum(const QString& text) {

    QRegExp reg = QRegExp("[+-]?\\d*\\.?\\d+");
    int pos = 0;
    QVector<QString> list;

    while((pos = reg.indexIn(text, pos)) != -1)
    {
        list << (reg.cap(0));
        pos += reg.matchedLength();
    }

    if(list.count() > 0)
        return list[list.count() - 1];//берем последние цифры!
    else
        return nullptr;//TODO: Вызовем ошибку если что не так! (throw)
}

QString MyRegExp::GetRegExpFirstFractionalNum(const QString& text) {

    QRegExp reg = QRegExp("[+-]?\\d*\\.?\\d+");
    int pos = 0;
    QVector<QString> list;

    while((pos = reg.indexIn(text, pos)) != -1)
    {
        list << (reg.cap(0));
        pos += reg.matchedLength();
    }

    if(list.count() > 0)
        return list[0];//берем первые цифры!
    else
        return nullptr;//TODO: Вызовем ошибку если что не так! (throw)
}
