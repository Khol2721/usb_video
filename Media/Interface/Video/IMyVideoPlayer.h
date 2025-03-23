#ifndef IMYVIDEOPLAYER_H
#define IMYVIDEOPLAYER_H

#include <QSize>
#include <QImage>
#include <QString>
#include <qglobal.h>
#include <memory>

class IMyVideoPlayer
{
public:
    virtual void RestartPlay() = 0;
    virtual void StartPlay(QString path) = 0;
    virtual void ClosePlay() = 0; 

    virtual uint GetCountFramePlay() const = 0;
    virtual uint GetCurrentFramePlay() const = 0;
    virtual bool SetCurrentFramePlay(uint posFrame) = 0;

    virtual quint64 GetCountTimePlayMs() const = 0;
    virtual quint64 GetCurrentTimePlayMs() const = 0;
    virtual bool SetCurrentTimePlayMs(quint64 posTime) = 0;

    virtual void SetCoeffSpeedPlay(float coeff) = 0;
    virtual float GetCoeffSpeedPlay() const = 0;

    ///Запущено чтение кадров видео или нет! (ПАУЗА)
    virtual bool GetIsRun() const = 0;
    virtual void SetIsRun(bool tf) = 0;

protected:
    //signals:
    virtual void NewFrame(std::shared_ptr<QImage> img) = 0;
    virtual void Sign_ClosePlay() = 0;

    /*///Следует отображать следующий кадр или нет! (если да, то параметр обнуляется, возвращая true)
    virtual bool CheckIsNextFramePlay() = 0;*/
};

#endif // IMYVIDEOPLAYER_H
