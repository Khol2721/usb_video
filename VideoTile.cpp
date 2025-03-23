#include "VideoTile.h"

QString VideoTile::g_pathFiles { "" };

QString VideoTile::g_nameImage { "" };

QString VideoTile::g_templateDateTime { "" };

IMyScreenFrame* VideoTile::g_iMyScreenFrame { nullptr };

TypeVideoInfo VideoTile::g_typeVideoInfo = TypeVideoInfo::Unknown;

void VideoTile::G_Init(TypeVideoInfo typeVideoInfo) {
    switch (typeVideoInfo) {
        case TypeVideoInfo::Normal:
        g_iMyScreenFrame = new MyVideoFrame();
        break;
        case TypeVideoInfo::Custom:
        g_iMyScreenFrame = new MyScreenFrame();
        break;
    default:
        throw;
    }

    g_typeVideoInfo = typeVideoInfo;
}

VideoTile::VideoTile(const QString& name, const QSize& size) : QObject(), icon(nullptr)
{
    if(g_pathFiles.isNull() || g_pathFiles.isEmpty()) {
        throw;
    }

    const auto separator = MyFileInfo::GetSeparator();
    if(g_pathFiles.back() != separator) {
        path = QString("%1%2%3").arg(g_pathFiles).arg(separator).arg(name);
    }
    else {
        path = g_pathFiles + name;
    }

    if(g_templateDateTime.isNull() || g_templateDateTime.isEmpty()) {
        throw;
    }

    auto strDateTimeFile = name.left(name.indexOf('/'));
    strDateTimeFile = MyRegExp::GetRegExpLastNameFull_FirstNum(strDateTimeFile);//TODO: не универсально, но сойдет!

    auto myTemplateDateTime = g_templateDateTime.mid(IndexOfTemplateDateTime(g_templateDateTime));
    dateTimeFile = QDateTime::fromString(strDateTimeFile, myTemplateDateTime);

    sizeIcon = size;

    ptrImage = nullptr;
    InitFrame();

    //NEW!
    switch (g_typeVideoInfo) {
    case TypeVideoInfo::Normal: {
        auto myVideoFrame = dynamic_cast<MyVideoFrame*>(g_iMyScreenFrame);
        durationSec = (quint64)(myVideoFrame->GetCountFramePlay()*(uint)(myVideoFrame->GetFps()));
        break;
    }
    case TypeVideoInfo::Custom: {
        auto pathFileDuration = MyFileInfo::GetPathDownFolder(path) + separator + MyVideoWriter::G_GetCustomNameFileVideoInfo();
        QFile fileDuration(pathFileDuration);
        if(fileDuration.open(QIODevice::ReadOnly))  {
            const auto arrayInfo = fileDuration.readLine();
            const QString strDurationSec = QString(arrayInfo);
            durationSec = strDurationSec.toULong();
            fileDuration.close();
        }
        else {
            //В таком случае файл был записан неисправно! (не известна продолжительность файла)
            return;
        }
        break;
    }
    default:
        throw;
    }

    isValid = true;
}

int VideoTile::IndexOfTemplateDateTime(const QString& str) {

    for(int i_ch = 1; i_ch < str.count(); i_ch++) {

        if(str[i_ch] == 'h' && str[i_ch - 1] == 'h') {
            return i_ch - 1;
        }
        else if(str[i_ch] == 'm' && str[i_ch - 1] == 'm') {
            return i_ch - 1;
        }
        else if(str[i_ch] == 's' && str[i_ch - 1] == 's') {
            return i_ch - 1;
        }
        else if(str[i_ch] == 'd' && str[i_ch - 1] == 'd') {
            return i_ch - 1;
        }
        else if(str[i_ch] == 'M' && str[i_ch - 1] == 'M') {
            return i_ch - 1;
        }
        else if(str[i_ch] == 'y' && str[i_ch - 1] == 'y') {
            return i_ch - 1;
        }
    }

    return -1;
}

void VideoTile::InitFrame() {

    ptrImage = g_iMyScreenFrame->GetScreenFrame(MyFileInfo::GetPathDownFolder(path) + MyFileInfo::GetSeparator() + "screen.png");

    if(ptrImage != nullptr) {
        QPixmap map;
        //SmoothTransformation - убирает искажения изображения при повороте камеры,
        //FastTransformation - ускореное преобразование (это нам и нужно для VideoTile)
        const auto resizeFrame = ptrImage->scaled(sizeIcon.width(), sizeIcon.height(), Qt::IgnoreAspectRatio, Qt::FastTransformation);
        map.convertFromImage(resizeFrame);//*ptrImage
        CreateIcon(map);
    }
    else {//видео было повреждено, либо отсутствовало устройство!
        QPixmap map(sizeIcon.width(), sizeIcon.height());
        map.fill(Qt::white);
        CreateIcon(map);
    }
}

void VideoTile::CreateIcon(const QPixmap& pixmap) {
    delete icon;
    icon = new QIcon(pixmap);
}

void VideoTile::G_SetPathFiles(const QString& pathFiles) {
    MyFileInfo myFileInfo;
    if(!myFileInfo.MkPath(pathFiles)) {
        qDebug() << "Error: Not create dir:" << pathFiles + "!";
        throw;
    }

    g_pathFiles = pathFiles;
}

const QString& VideoTile::G_GetPathFiles() {
    return g_pathFiles;
}

void VideoTile::G_SetTemplateDateTime(const QString& str) {
    g_templateDateTime = str;
}

const QString& VideoTile::G_GetTemplateDateTime() {
    return g_templateDateTime;
}

const QString& VideoTile::G_GetNameImage() {
    return g_nameImage;
}

void VideoTile::G_SetNameImage(const QString& name) {
    g_nameImage = name;
}

VideoTile::~VideoTile() {
    delete icon;
}
