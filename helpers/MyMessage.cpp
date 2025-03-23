#include "MyMessage.h"

void MyMessage::ShowMessage(const QString& msg, const QString& info) {
    Show<const QString&>(msg, info);
}

void MyMessage::ShowError(const QString& msg, const QString& info) {
    SaveLog(Show<const QString&>(msg, info));
}

void MyMessage::SaveLog(const QString& msg) {

    MyFileInfo myFileInfo;
    const auto dir = myFileInfo.GetFilePath() + myFileInfo.GetSeparator() + "logMessages.txt";

    QFile file(dir);

    file.open(QIODevice::Append);

    if(!file.isOpen()) {
        return;
    }

    QTextStream outStream(&file);
    outStream << QDateTime::currentDateTime().toString("dd.MM.yyyy_hh:mm:ss ") << msg << endl;

    file.close();
}

template<typename T>
QString MyMessage::Show(T message, T info) {

    QString str;
    if(info == nullptr) {
        str = message;
    }
    else {
        str = QString("%1: %2").arg(info).arg(message);
    }

    #ifdef QT_DEBUG
    qDebug() << str;
    #endif

    return str;
}
template QString MyMessage::Show<const char*>(const char*, const char*);
template QString MyMessage::Show<const QString&>(const QString&, const QString&);
