#ifndef MYMESSAGE_H
#define MYMESSAGE_H

#include <QString>
#include <QDateTime>
#include <QTextStream>

#ifdef QT_DEBUG
#include <QDebug>
#endif

#include "myfileinfo.h"

class MyMessage
{
public:
    MyMessage() = delete;
    virtual ~MyMessage() = delete;

    static void ShowMessage(const QString& msg, const QString& info = nullptr);
    static void ShowError(const QString& msg, const QString& info = nullptr);
    static void SaveLog(const QString& msg);

private:
    template<typename T>
    static QString Show(T str, T info);
};

#endif // MYMESSAGE_H
