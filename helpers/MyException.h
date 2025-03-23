#ifndef MYEXCEPTION_H
#define MYEXCEPTION_H

#include <QString>
#include <stdexcept>

#include "MyMessage.h"

class MyException : public std::runtime_error
{
public:
    //explicit тут необходим, чтобы предотвратить неявные преобразования передаваемых параметров, если они не соответствуют типу!
    explicit MyException(const char* message) : std::runtime_error(message) { MyMessage::SaveLog(message); }
    explicit MyException(const std::string& message) : std::runtime_error(message) { MyMessage::SaveLog(message.data()); }
    explicit MyException(const QString& message) : std::runtime_error(message.toStdString()) { MyMessage::SaveLog(message); }

    QString GetMassage() const {
        return QString(this->what());
    }

    const char* GetPtrMassage() const {
        return this->what();
    }
};

//!Не выводит сообщение в console!
/*#include <QException>

class MyException : public QException
{
public:
    MyException(const QString& text);

    void raise() const override;
    QException *clone() const override;

    QString GetMassage() const;

protected:
    QString text;
};*/

#endif // MYEXCEPTION_H
