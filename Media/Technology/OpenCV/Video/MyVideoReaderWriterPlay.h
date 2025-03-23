#ifndef MYVIDEOREADERWRITERPLAY_H
#define MYVIDEOREADERWRITERPLAY_H

#include <QDebug>
#include <QImage>
#include <QTimer>
#include <QtMath>
#include <QThread>
#include <QDateTime>
#include <QElapsedTimer>

#include <memory>
#include <functional>

#include <opencv2/opencv.hpp>

#include "helpers/myfileinfo.h"
#include "helpers/MyElapsedTimer.h"
#include "../Primitives/ConverterVariablesCV.h"
#include "../../../System/SystemInitialization.h"
#include "IMyVideoReaderWriter.h"

#include "../Screen/ICvScreenMonitors.h"

#define MAX_I_NOT_FRAME 30 //TODO: в config! (не может быть <= 0)
#define DEF_FPS 60 //TODO: в config! (analog cam = 25, veb cam = 60)
#define DEF_COEFF_SPEED_PLAY 1

#define STR_EQUAL 0

using namespace cv;

class MyVideoReaderWriterPlay : public IMyVideoReaderWriter
{
    Q_OBJECT
public:
    MyVideoReaderWriterPlay(
            TypeWorkWriter typeWorkWriter,
            IScreenMonitors* iScreenMonitors,
            uint8_t defFps = DEF_FPS, QObject* parent = nullptr);
    virtual ~MyVideoReaderWriterPlay();

    //IMyVideoPlayer:
    float GetCoeffSpeedPlay() const override;

    bool GetIsRun() const override;
    bool GetIsOpened() const override;

    quint64 GetCountTimePlayMs() const override;
    quint64 GetCurrentTimePlayMs() const override;

    //IMyVideoFrame and IMyVideoPlayer:
    ushort GetFps() const override;
    uint GetCountFramePlay() const override;
    uint GetCurrentFramePlay() const override;

    virtual bool SetSizeFrame(QSize size) override;//Не безопасен после инициализации!

    //IMyVideoReader:
    void SetNumDevice(qint8 num) override;//Не безопасен после инициализации!
    void SetPathFileDevice(const QString& path) override;//Не безопасен после инициализации!
    void SetVideoIdDevice(const QString& path, qint8 num) override;//Не безопасен после инициализации!

    bool GetIsPlay() const override;
    bool GetIsWork() const override;

    int GetWidth() const override;
    int GetHeight() const override;

    bool GetIsExternalFrames() const override;

    void SetSystemConfigVideoDevice(SystemConfigVideoDevice type, bool isWork) override;
    bool GetSystemConfigVideoDevice(SystemConfigVideoDevice type) const override;
    void InitSystemConfigVideoDevice() override;

    MyVideoWriter& GetMyVideoWriter();

protected:
    MyPtrImage GetImage();
    bool Init();

    //This:
    void Close();
    void Start();
    void Stop();

private:
    cv::VideoCapture* videoCapture;

    QTimer* timerFrame;

    uint iNotFrame;

#ifdef IS_TIMER_TEST
    QElapsedTimer elTimerTest;
#endif

    //Выделим отдельную переменную чтобы не было путаницы в дальнейшем!
    QString pathFilePlay;

    ///Коэфф. скорости перемотки!
    float coeffSpeed;

    quint64 countTimePlayMs;


#if CV_MAJOR_VERSION >= 3
    const int apiPreference;
#endif

    std::shared_ptr<QImage> saveImage = nullptr;

    #ifdef IS_SHOW_DEL_TIME_READ_FRAME
    int64 previousTimeEpochMs = 0;
    #endif

    IScreenMonitors* iScreenMonitors { nullptr };

    MyElapsedTimer myElapsedTimer;

    int64 delStopPlay = 0;
    int64 tikFrame = 0;//tikFrame == GetCurrentFramePlay! TODO: Лишняя переменная?
    int64 currentTimePlayMs = 0;
    int64 startSpeedTimePlayMs = 0;

    bool isSetResize { false };

    bool isPlay { false };

    void InitCountTimePlayMs();

    void ReadFrame();
    int GetTimeoutFrame();

    void InitTimeoutTimerFrame();

    int64 CreateCurrentTimeCustomPlayMs();

    template<class T>
    void WriteFrame(std::shared_ptr<T> ptrFrame);

public slots:
    void SetIsRun(bool in) override;
    void SetCoeffSpeedPlay(float coeff = DEF_COEFF_SPEED_PLAY) override;
    bool SetCurrentFramePlay(uint posFrame) override;
    bool SetCurrentTimePlayMs(quint64 posTime) override;

    void RestartPlay() override;
    void StartPlay(QString path) override;
    void ClosePlay() override;

    ///Метод для включения встраивания стороннего видео-изображения!
    void SetIsExternalFrames(bool in) override;
    void SetExternalFrame(std::shared_ptr<QImage> img) override;

private slots:
    void TimeoutFrame();
    void CloseWork();
};

#endif // MYVIDEOREADERWRITERPLAY_H
