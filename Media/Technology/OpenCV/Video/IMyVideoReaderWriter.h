#ifndef IMYVIDEOREADERWRITER_H
#define IMYVIDEOREADERWRITER_H

#include "MyVideoWriter.h"
#include "../../../Interface/Video/IMyVideoReader.h"

class IMyVideoReaderWriter : public QObject, public IMyVideoReader
{
    Q_OBJECT
public:
    IMyVideoReaderWriter(const QSize& sizeFrame, uint8_t defFps, QObject* parent = nullptr);
    virtual ~IMyVideoReaderWriter() = default;

    MyVideoWriter& GetMyVideoWriter();

    virtual bool GetIsExternalFrames() const = 0;

    struct InfoMat {
        cv::Ptr<cv::Mat> ptrMat;
        qint64 timeReadMatMs;
    };
    InfoMat GetInfoMat();

    bool GetIsNewMat() {
        bool value = isNewMat;
        isNewMat = false;
        return value;
    }

protected:
    MyVideoWriter* myVideoWriter;
    bool isRun;

    QSize sizeFrame;
    const uint8_t defFps;

    std::mutex mutexMat;
    InfoMat infoMat;
    std::atomic_bool isNewMat { false };

    void SetInfoMat(cv::Ptr<cv::Mat> ptrMat, qint64 timeReadMatMs);

signals:
    void NewFrame(std::shared_ptr<QImage> img) override;
    void Sign_ClosePlay() override;

public slots:
    virtual void SetIsRun(bool tf) override = 0;
    virtual void SetCoeffSpeedPlay(float coeff) override = 0;
    virtual bool SetCurrentFramePlay(uint posFrame) override = 0;
    virtual bool SetCurrentTimePlayMs(quint64 posTime) override = 0;

    virtual void RestartPlay() override = 0;
    virtual void StartPlay(QString path) override = 0;
    virtual void ClosePlay() override = 0;

    virtual void SetIsExternalFrames(bool in) = 0;//Multy
    virtual void SetExternalFrame(std::shared_ptr<QImage> img) = 0;//Multy
};

#endif // IMYVIDEOREADERWRITER_H
