#ifndef FILEINFO_H
#define FILEINFO_H

#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>

enum FileStatus {
    NotFile,
    TrueFile,
    NotPath,
    ErrorPath
};

class MyFileInfo
{
public:
    MyFileInfo(const QString& path = nullptr);
    virtual ~MyFileInfo();

    ///Подробная проверка наличия файла! (для драйверов важен флаг NotPath)
    static FileStatus SearchFile(const QString& filePath);

    ///Проверка и создание папки (если нужно)!
    static bool MkFolder(const QString& filePath);
    ///Проверка и создание пути по директории (если нужно)!
    static bool MkPath(const QString& filePath);

    //!По умолчанию находит папки только верхнего уровня!
    static QStringList GetListDirFolders(const QString& path, QDir::Filters filters = QDir::Dirs | QDir::NoDotAndDotDot);//new!

    ///Удаляет все файлы и папки, расположенные по заданной директории!
    static bool DeleteAllFilesAndFolders(const QString& path);//new!

    ///Возвращает директорию папки низкого уровня! (убирает наименование файла из пути)
    static QString GetPathDownFolder(const QString& fullPath);//new!

    ///Возвращает имя папки низкого уровня!
    static QString GetNameDownFolder(const QString& fullPath);//new!

    ///Возвращает имя файла (и формат) по заданной директории!
    static QString GetNameFile(const QString& fullPath);//new!

    //https://www.cyberforum.ru/qt/thread1210174.html
    ///Возвращает размер занятого пространства в указанной папке!
    static quint64 GetSizeFilesInFolder(const QDir& dir);//new!

    QString GetFullFilePath() const { return fileInfo->filePath(); }
    QString GetFilePath() const { return fileInfo->absolutePath(); }
    QString GetFileName() const { return fileInfo->fileName(); }
    static QChar GetSeparator() { return QDir::separator(); }//new!

    FileStatus SearchFile();

private:
    QFileInfo* fileInfo;

    static FileStatus CustomSearchFile(const QFileInfo* const fileInfo);
};

#endif // FILEINFO_H
