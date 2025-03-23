#ifndef MYVIDEOREADERWRITER_H
#define MYVIDEOREADERWRITER_H

#include <QMap>
#include <QDebug>
#include <QImage>
#include <QTimer>
#include <QtMath>
#include <QThread>
#include <QDateTime>
#include <QElapsedTimer>

#include <mutex> //TODO: Нужно везде где требуется ограничить доступ к межпоточным ресурсам!
#include <memory>
#include <functional>
//#include <type_traits>

#include <opencv2/opencv.hpp>
//#include <opencv2/videoio.hpp>
//#include "opencv2/calib3d/calib3d.hpp"
//#include "opencv2/videoio/registry.hpp"//появился в новой версии!

#include "helpers/myfileinfo.h"
#include "helpers/MyElapsedTimer.h"
#include "../Primitives/ConverterVariablesCV.h"
#include "../../../Interface/Video/IMyVideoReader.h"
#include "../../../System/SystemInitialization.h"
#include "MyVideoWriter.h"

#include "../Screen/ICvScreenMonitors.h"
//#include "../../Interface/Screen/IScreenMonitors.h"

#define MAX_I_NOT_FRAME 30 //TODO: в config! (не может быть <= 0)
#define DEF_FPS 60 //TODO: в config! (analog cam = 25, veb cam = 60)
#define DEF_TIMEOUT_CONNECT 1000//TODO: в config!
#define TIME_SLEEP_INIT_SIZE_MS 5000//TODO: в config!
#define DEF_COEFF_SPEED_PLAY 1
#define ADD_DELAY_TIMEOUT_READ_FRAME 1//TODO: в config!

#define STR_EQUAL 0

//#define IS_SHOW_DEL_TIME_READ_FRAME

//#define IS_WORK_RESIZE_DEVICE

//#define IS_TIMER_TEST //test time read frame!

using namespace cv;

Q_DECLARE_METATYPE(std::shared_ptr<QImage>)

typedef std::function<Mat(Mat&)> FunConvertMat;
typedef std::function<QImage(QImage&)> FunConvertImage;

///Поддерживает как считывание из файла-устройства, так и из видео-файла!
/// (есть все необходимое работы в режиме воспроизведения видео-файла)
/// Замечание: возникают кратковременные провисания на видео (это же видно на версии Сервала v3.51)!
/// Важно: реализация на таймерах не совсем корректна, так как точность у таймера скачет в районе 5мс,
/// а в реализации на sleep 1мс (заметной разници в производительности замечено не было),
/// однако реализация на thread::sleep полностью блокирует механизм сигналов/слотов, потому не подходит!
class MyVideoReaderWriter : public QObject, public IMyVideoReader
{
    Q_OBJECT
public:
    //TODO: Что думаешь по поводу применения паттерна State?
    enum class StateWork : uint8_t
    {
        WorkVideoDevice = 0,
        WorkPlay,
        WorkEmpty
    };

    MyVideoReaderWriter(
            TypeWorkWriter typeWorkWriter,
            IScreenMonitors* iScreenMonitors,
            FunConvertMat funConvertMat,
            FunConvertImage funConvertImage,
            QSize sizeFrame = QSize(),
            StateWork stateWork = StateWork::WorkVideoDevice,
            uint8_t defFps = DEF_FPS);
    virtual ~MyVideoReaderWriter();

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
    void SetIsCheckFileDevice(bool in);

    bool GetIsPlay() const override;
    bool GetIsWork() const override;

    int GetWidth() const override;
    int GetHeight() const override;

    bool GetIsExternalFrames() const;//Multy

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

private:

    bool WriteImageBeforeConvert(std::shared_ptr<QImage> ptrImage);
    void WriteImageAfterConvert(std::shared_ptr<QImage> ptrImage);

    StateWork stateWork;
    const StateWork startStateWork;

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
    bool isRun;

    FunConvertMat funConvertMat;
    FunConvertImage funConvertImage;

    QTimer* timerFrame;
    QTimer* timerConnect;

    bool isConnect;
    uint iNotFrame;

#ifdef IS_TIMER_TEST
    QElapsedTimer elTimerTest;
#endif

    ushort actualFps;

    //Выделим отдельную переменную чтобы не было путаницы в дальнейшем!
    QString pathFilePlay;

    ///Коэфф. скорости перемотки!
    float coeffSpeed;

    QSize sizeFrame;

    quint64 countTimePlayMs;

    bool isInit;

#if CV_MAJOR_VERSION >= 3
    const int apiPreference;
#endif

    bool isExternalFrames;//Multy
    std::shared_ptr<QImage> ptrExternalFrame;//Multy

    QMap<SystemConfigVideoDevice, bool> mapSystemConfigVideoDevice;

    std::shared_ptr<QImage> saveImage = nullptr;

    #ifdef IS_SHOW_DEL_TIME_READ_FRAME
    int64 previousTimeEpochMs = 0;
    #endif

    bool isWriteAfterConvert = true;

    MyVideoWriter* myVideoWriter;

    MyElapsedTimer myElapsedTimer;

    int64 delStopPlay = 0;
    int64 tikFrame = 0;//tikFrame == GetCurrentFramePlay! TODO: Лишняя переменная?
    int64 currentTimePlayMs = 0;
    int64 startSpeedTimePlayMs = 0;

    /*const enum class TypeReaderPlay {
        Normal,//по умолчанию!
        Custom//свой, основанный на собственной записи!
    } typeReaderPlay;*/
    const TypeWorkWriter typeWorkWriter;

    IScreenMonitors* iScreenMonitors = nullptr;

    const uint8_t defFps;

    bool isCheckFileDevice { true };

    void InitCountTimePlayMs();

    //look_guard используй!
    //https://en.cppreference.com/w/cpp/thread/lock_guard
    //std::mutex mutexData;

    void ReadFrame();
    int GetTimeoutFrame();

    void InitTimeoutTimerFrame();

    int64 CreateCurrentTimeCustomPlayMs();

    template<typename T>
    bool future_is_ready(std::future<T>& t) {
        return t.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }

    template<class T>
    void WriteFrame(std::shared_ptr<T> ptrFrame);

signals:
    void NewFrame(std::shared_ptr<QImage> img) override;
    void Sign_ClosePlay() override;

public slots:
    void SetIsRun(bool tf) override;
    void SetCoeffSpeedPlay(float coeff = DEF_COEFF_SPEED_PLAY) override;
    bool SetCurrentFramePlay(uint posFrame) override;
    bool SetCurrentTimePlayMs(quint64 posTime) override;

    void RestartPlay() override;
    void StartPlay(QString path) override;
    void ClosePlay() override;

    ///Метод для включения встраивания стороннего видео-изображения!
    void SetIsExternalFrames(bool tf);//Multy
    void SetExternalFrame(std::shared_ptr<QImage> img);//Multy

private slots:
    void TimeoutFrame();
    void TimeoutConnect();
    void CloseWork();
};

#endif // MYVIDEOREADERWRITER_H
