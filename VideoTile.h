#ifndef VIDEOTILE_H
#define VIDEOTILE_H

#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include <QDateTime>
#include <QIcon>

#include "helpers/myregexp.h"
#include "helpers/myfileinfo.h"

#include "Media/Technology/OpenCV/Video/MyVideoFrame.h"
#include "Media/Technology/OpenCV/Screen/MyScreenFrame.h"
#include "Media/Technology/OpenCV/Video/MyVideoWriter.h"

enum class TypeVideoInfo {
    Unknown,
    Normal,
    Custom
};

///Плитка UI (из ранней версии) переделана в информативный класс для доступа к файлам и отрисовки изображения!
class VideoTile : public QObject
{
    Q_OBJECT
public:
    VideoTile(const QString& name, const QSize& size);
    virtual ~VideoTile();

    QString GetPath() const { return path; }

    QDate GetDateFile() const { return dateTimeFile.date(); }
    QTime GetTimeFile() const { return dateTimeFile.time(); }

    const QDateTime& GetDateTimeFile() const { return dateTimeFile; }

    quint64 GetDurationMs() const { return durationSec; }//new!

    const QIcon& GetIcon() const { return *icon; }

    static void G_SetPathFiles(const QString& pathFiles);
    static const QString& G_GetPathFiles();

    //! Важно: формат даты например "dd/MM/yyyy" (с сохранением обозначений), формат времени например hh:mm:ss (с сохранением обозначений)
    static void G_SetTemplateDateTime(const QString& str);
    static const QString& G_GetTemplateDateTime();

    static int IndexOfTemplateDateTime(const QString& str);

    static void G_Init(TypeVideoInfo typeVideoInfo);

    bool GetIsValid() const { return isValid; }

    static void G_SetNameImage(const QString& name);
    static const QString& G_GetNameImage();

private:
    void InitFrame();

    QString path;

    QDateTime dateTimeFile;

    quint64 durationSec;

    QSize sizeIcon;

    std::shared_ptr<QImage> ptrImage;

    static QString g_pathFiles;
    static QString g_templateDateTime;
    static IMyScreenFrame* g_iMyScreenFrame;
    static TypeVideoInfo g_typeVideoInfo;

    static QString g_nameImage;

    QIcon* icon;

    bool isValid { false };

    void CreateIcon(const QPixmap& pixmap);
};

#endif // VIDEOTILE_H
