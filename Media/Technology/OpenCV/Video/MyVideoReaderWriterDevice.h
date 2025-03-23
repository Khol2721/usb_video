#ifndef MYVIDEOREADERWRITERDEVICE_H
#define MYVIDEOREADERWRITERDEVICE_H

#include <QMap>
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
#include "helpers/MyException.h"
#include "../Primitives/ConverterVariablesCV.h"
#include "../../../System/SystemInitialization.h"
#include "IMyVideoReaderWriter.h"

//#define MAX_I_NOT_FRAME 30 //TODO: в config! (не может быть <= 0)
#define DEF_FPS 60 //TODO: в config! (analog cam = 25, veb cam = 60)
#define DEF_TIMEOUT_CONNECT 1000//TODO: в config!
#define ADD_DELAY_TIMEOUT_READ_FRAME 1//TODO: в config!
#define IS_AUTO_INTERVAL_READ_FRAME //Нужен для автоопределения и контроля оптимальной частоты чтения кадров!

//#define IS_TEST_VIEW_IMAGE//тестовое отображение наложенной графики без видеоустройства!

using namespace cv;

typedef std::function<Mat(Mat&)> FunConvertMat;
typedef std::function<QImage(QImage&)> FunConvertImage;

class MyVideoReaderWriterDevice : public IMyVideoReaderWriter
{
    Q_OBJECT
public:
    MyVideoReaderWriterDevice(
            TypeWorkWriter typeWorkWriter,
            FunConvertMat funConvertMat,
            FunConvertImage funConvertImage,
            uint8_t defFps = DEF_FPS, QObject* parent = nullptr);
    virtual ~MyVideoReaderWriterDevice();

    //IMyVideoPlayer:
    float GetCoeffSpeedPlay() const override;

    bool GetIsRun() const override;
    bool GetIsOpened() const override;

    quint64 GetCountTimePlayMs() const override;
    quint64 GetCurrentTimePlayMs() const override;

    //IMyVideoFrame and IMyVideoPlayer:
    virtual ushort GetFps() const override;
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

    void SetIsWriteAfterConvert(bool in);

protected:
    MyPtrImage GetImage();
    bool Init();

    //This:
    void Close();
    void Start();
    void Stop();

    virtual bool CheckIsDisconnect();

    uint max_iNotFrame { 30 };

private:

    bool WriteImageBeforeConvert(std::shared_ptr<QImage> ptrImage);
    bool WriteImageAfterConvert(std::shared_ptr<QImage> ptrImage);

    struct VideoId
    {
    public:
        VideoId() : VideoId(-1) { }//по умолчанию! (определяет система)

        VideoId(qint8 num) : isInitNum(true) {
            this->path = "";
            this->num = num;
        }

        VideoId(const QString& path, qint8 num) : isInitNum(true) {
            this->path = path;
            this->num = num;
        }

        VideoId(const QString& path) : isInitNum(false) {
            this->path = path;
            this->num = -2;//недействительный!
        }

        qint8 GetNum() const {
            return num;
        }

        const QString& GetPath() const {
            return path;
        }

        bool CheckNum() {

            if(-1 <= num) {
                return true;
            }
            else {
                return false;
            }
        }

        bool CheckPath() {

            if(!path.isNull() && !path.isEmpty()) {
                return true;
            }
            else {
                return false;
            }
        }

        bool GetIsInitNum() const {
            return isInitNum;
        }

    private:
        QString path = nullptr;
        qint8 num = std::numeric_limits<qint8>::min();
        bool isInitNum;
    } videoId;

    cv::VideoCapture* videoCapture;

    FunConvertMat funConvertMat;
    FunConvertImage funConvertImage;

    QTimer* timerFrame;
    QTimer* timerConnect;

    uint iNotFrame;

#ifdef IS_TIMER_TEST
    QElapsedTimer elTimerTest;
#endif

    QSize sizeFrame;

#if CV_MAJOR_VERSION >= 3
    const int apiPreference;
#endif

    bool isExternalFrames;
    std::shared_ptr<QImage> ptrExternalFrame;

    QMap<SystemConfigVideoDevice, bool> mapSystemConfigVideoDevice;

    std::shared_ptr<QImage> saveImage = nullptr;

    #ifdef IS_SHOW_DEL_TIME_READ_FRAME
    int64 previousTimeEpochMs = 0;
    #endif

    bool isWriteAfterConvert = true;

    bool isSetResize { false };

    #ifdef IS_AUTO_INTERVAL_READ_FRAME
    int i_currentTimeoutConnect { 0 };
    #endif

    bool isCheckReadInterval { false };

    static std::mutex g_mutexInit;


    void ReadFrame();
    int GetTimeoutFrame();

    void InitTimeoutTimerFrame();

    template<class T>
    void WriteFrame(std::shared_ptr<T> ptrFrame);

public slots:
    void SetIsRun(bool in) override;
    void SetCoeffSpeedPlay(float coeff) override;
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
    void TimeoutConnect();
    void CloseWork();
};

#endif // MYVIDEOREADERWRITERDEVICE_H
