#include "myfileinfo.h"

MyFileInfo::MyFileInfo(const QString& path)
{
    if(path.isNull() || path.isEmpty())
    {
        //берем путь к нашему корневому файлу!
        fileInfo = new QFileInfo(QCoreApplication::applicationFilePath());
    }
    else
        fileInfo = new QFileInfo(path);
}

FileStatus MyFileInfo::SearchFile()
{
    FileStatus status = NotPath;
    status = MyFileInfo::CustomSearchFile(fileInfo);
    return status;
}

FileStatus MyFileInfo::SearchFile(const QString& filePath)
{
    if(filePath.isNull() || filePath.isEmpty())
        return FileStatus::ErrorPath;

    const QFileInfo check_file(filePath);

    FileStatus status = NotPath;
    status = MyFileInfo::CustomSearchFile(&check_file);
    return status;
}

bool MyFileInfo::MkFolder(const QString& path)
{
    if(path.isNull() || path.isEmpty())
        return false;

    QDir dir(path);

    if(!dir.exists()) {
        return dir.mkdir(path);
    }
    else {
        return true;
    }
}

bool MyFileInfo::MkPath(const QString& path)
{
    if(path.isNull() || path.isEmpty())
        return false;

    QDir dir(path);
    if(!dir.exists()) {
        return dir.mkpath(path);
    }
    else {
        return true;
    }
}

QStringList MyFileInfo::GetListDirFolders(const QString& path, QDir::Filters filters) {

    QDir dir(path);
    return dir.entryList(filters);
}

bool MyFileInfo::DeleteAllFilesAndFolders(const QString& path) {

    QDir dir(path);
    return dir.removeRecursively();
}

QString MyFileInfo::GetPathDownFolder(const QString& fullPath) {

    const auto indexLastSlash = fullPath.lastIndexOf(GetSeparator());

    if(indexLastSlash >= 0) {
        return fullPath.left(indexLastSlash);
    }
    else {
        return nullptr;
    }
}

QString MyFileInfo::GetNameDownFolder(const QString& fullPath) {

    auto pathDownFolder = GetPathDownFolder(fullPath);

    if(pathDownFolder == nullptr) {
        return pathDownFolder;
    }

    const auto separator = GetSeparator();
    if(pathDownFolder.at(pathDownFolder.count() - 1) == separator) {
        pathDownFolder = pathDownFolder.left(pathDownFolder.count() - 1);
    }

    const auto indexLastSlash = pathDownFolder.lastIndexOf(separator);

    if(indexLastSlash >= 0) {
        return pathDownFolder.mid(indexLastSlash + 1, pathDownFolder.count() - indexLastSlash);
    }
    else {
        return nullptr;
    }
}

QString MyFileInfo::GetNameFile(const QString& fullPath) {

    const auto indexStart = fullPath.lastIndexOf(GetSeparator());

    if(indexStart >= 0) {
        return fullPath.mid(indexStart + 1);
    }
    else {
        return nullptr;
    }
}

quint64 MyFileInfo::GetSizeFilesInFolder(const QDir& dir) {

    quint64 sum { 0 };
    for(const QFileInfo& fInfo : dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
        if (fInfo.isFile())
            sum += fInfo.size();
        else if (fInfo.isDir()) {
            QDir subDir(fInfo.absoluteFilePath());
            if (subDir.exists()) {
                sum += GetSizeFilesInFolder(subDir);
            }
        }
    }
    return sum;
}

FileStatus MyFileInfo::CustomSearchFile(const QFileInfo* const fileInfo)
{
    FileStatus status = NotPath;

    bool isExists = fileInfo->exists();
    //bool isFile = fileInfo->isFile();//для файлов устройств не находит!
    bool isDir = fileInfo->isDir();//лучший вариант, так как определяет, папка это или нет!

    if(isExists && !isDir)
        status = TrueFile;
    else if(isExists && isDir)
        status = NotFile;

    return status;
}

MyFileInfo::~MyFileInfo() { delete fileInfo; }
