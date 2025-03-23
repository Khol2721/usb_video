#ifndef MYVIDEOFRAME_H
#define MYVIDEOFRAME_H

#include <QDebug>
#include <QImage>
#include <QtMath>

#include <memory>
#include <future>

#include <opencv2/opencv.hpp>
//#include <opencv2/videoio.hpp>

#include "../../../Interface/Screen/IMyScreenFrame.h"
#include "../../../Interface/Video/IMyVideoFrame.h"
#include "../Primitives/ConverterVariablesCV.h"

#define VIDEO_CURRENT_FRAME 2

///Предназначен для разового считывания фрейма! (без наворотов MyVideoReader)
class MyVideoFrame : public IMyVideoFrame, public IMyScreenFrame
{
public:
    MyVideoFrame();
    virtual ~MyVideoFrame();

    uint GetCountFramePlay() const;
    uint GetCurrentFramePlay() const;
    bool SetCurrentFramePlay(uint posFrame);

    ushort GetFps() const override;

    MyPtrImage GetScreenFrame(const QString& path) override;

    int GetWidth() const override;
    int GetHeight() const override;

protected:
    bool Init(const QString& path);
    bool SetSizeFrame(QSize size) override;

    bool GetIsOpened() const override;

    MyPtrImage GetImage(uint currentFrame = VIDEO_CURRENT_FRAME);

private:
    cv::VideoCapture* videoCapture;
    QString path;

    void Delete();
};

#endif // MYVIDEOFRAME_H
