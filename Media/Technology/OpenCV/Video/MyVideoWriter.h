#ifndef MYVIDEOWRITER_H
#define MYVIDEOWRITER_H

#include <QSize>
#include <QString>
#include <QDateTime>
#include <QImage>
#include <QDebug>
#include <QThread>
#include <QFile>

#include <mutex>

#include <opencv2/opencv.hpp>
//#include <opencv2/videoio.hpp>

#include "../../../Interface/Video/IMyVideoWriter.h"

#include "helpers/myfileinfo.h"
#include "helpers/MyException.h"
#include "../Primitives/ConverterVariablesCV.h"

//#define DEF_NAME_FILE_VIDEO_INFO "videoInfo.txt"

typedef cv::Mat Mat;

enum class TypeWorkWriter {
    Normal,//по умолчанию!
    Custom//свой, основанный на собственной записи!
};

enum class FormatFileSaveVideo : quint8 {
    Def = 0,
    AVI = Def
};

enum class FormatFileSaveScreen : quint8 {
    Def = 0,
    JPG = Def,
    PNG
};

typedef union {
    char data[4];
    struct {
        char c1;
        char c2;
        char c3;
        char c4;
        } name;
    } MyCvFourcc;

class MyVideoWriter : public IMyVideoWriter
{
public:
    MyVideoWriter(TypeWorkWriter typeWorkWriter);
    MyVideoWriter(
            TypeWorkWriter typeWorkWriter,
            const QString& pathWriteVideo,
            const QString& pathWriteScreen,
            const MyCvFourcc& myCvFourcc,
            const QSize& sizeVideo,
            double fpsVideo);
    virtual ~MyVideoWriter();

    bool Start() override;
    void Stop() override;

    bool GetIsRun() const override;

    void NewFrame(std::shared_ptr<QImage> ptrImage);

    template<class T>
    void NewFrame(std::shared_ptr<T> ptrMat);

    //Init:
    void SetCodec(const MyCvFourcc newMyCvFourcc);
    void SetPathWriteVideo(const QString& newPath,
                       FormatFileSaveVideo formatFileSaveVideo = FormatFileSaveVideo::Def);

    void SetPathWriteScreen(const QString& newPath,
                       FormatFileSaveScreen formatFileSaveScreen = FormatFileSaveScreen::Def);

    void SetSizeVideo(const QSize& size);

    void SetRectScreen(const QRect& rect);

    void SetFps(double fps);

    void DoScreen();

    //! Если запись не запущена, то вернет -1!
    qint64 GetCurrentTimeMs() const override;//new!

    //! Если запись не запущена, то вернет -1!
    qint64 GetStartTimeEpochMs() const override;//new!

    QString GetNameFileVideo() const override;//new!
    QString GetNameFileScreen() const override;//new!

    const QString& GetNameFileVideoInfo() const;

    static QString G_GetDefStrFormatVideo();//new!
    static QString G_GetDefStrFormatScreen();//new!

    static QString G_GetStrFormatVideo(FormatFileSaveVideo format);
    static QString G_GetStrFormatScreen(FormatFileSaveScreen format);

    static const QString& G_GetCustomNameFileVideoInfo();

    TypeWorkWriter GetTypeWorkWriter() const;

private:
    const TypeWorkWriter typeWorkWriter;

    std::shared_ptr<Mat> frameStart = nullptr;

    cv::VideoWriter* writer;
    //cv::VideoWriter лучше, т.к. CvVideoWriter - устарел! (до opencv 2.0)

    QString pathWriteFrameStart;
    QString pathWriteVideo;
    QString pathWriteScreen;
    MyCvFourcc myCvFourcc;
    double fpsVideo;

    QSize sizeVideo;

    QRect rectScreen;

    bool isRun;
    bool isDoScrean;

    qint64 startTimeEpochMs;

    std::mutex mutex;

    int countFrame = 0;

    static const QString g_customNameFileVideoInfo;

    std::shared_ptr<Mat> CreateScreen(const Mat& mat) const;
    std::shared_ptr<Mat> CreateScreen(const UMat& mat) const;

    Mat CreateNullFrame() const;
};

#endif // MYVIDEOWRITER_H
