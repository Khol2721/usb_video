#ifndef IMYVIDEOWRITER_H
#define IMYVIDEOWRITER_H

#include <QString>

//TODO: Интерфейст необходимо дополнить методами, предположительно закомментированными!
class IMyVideoWriter
{
public:
    virtual bool GetIsRun() const = 0;

    //void NewFrame(std::shared_ptr<QImage> ptrImage);

    virtual bool Start() = 0;
    virtual void Stop() = 0;

    //void SetSizeVideo(const QSize& size);

    //void SetRectScreen(const QRect& rect);

    //void SetFps(double fps);

    /*void SetCodec(const MyCvFourcc newMyCvFourcc);
    void SetPathWriteVideo(const QString& newPath,
                       FormatFileSaveVideo formatFileSaveVideo = FormatFileSaveVideo::Def);

    void SetPathWriteScreen(const QString& newPath,
                       FormatFileSaveScreen formatFileSaveScreen = FormatFileSaveScreen::Def);*/

    //void DoScreen();

    //! Если запись не запущена, то вернет -1!
    virtual qint64 GetCurrentTimeMs() const = 0;

    //! Если запись не запущена, то вернет -1!
    virtual qint64 GetStartTimeEpochMs() const = 0;

    virtual QString GetNameFileVideo() const = 0;
    virtual QString GetNameFileScreen() const = 0;

    //const QString& GetNameFileVideoInfo() const;
};

#endif // IMYVIDEOWRITER_H
