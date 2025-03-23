#ifndef IMYAUDIOPLAYER_H
#define IMYAUDIOPLAYER_H

#include <memory>

#include <qglobal.h>
#include <QString>

//TODO: Интерфейсы IMyAudioPlayer и IMyVideoPlayer крайне похожи,
//как и другие..., желательно бы сделать объединение!
class IMyAudioPlayer
{
public:
    virtual bool RestartPlay() = 0;
    virtual bool StartPlay(const QString& path) = 0;
    virtual bool ClosePlay() = 0;

    virtual quint64 GetCountTimePlayMs() const = 0;
    virtual quint64 GetCurrentTimePlayMs() const = 0;
    virtual bool SetCurrentTimePlayMs(quint64 posTime) = 0;

    virtual void SetCoeffSpeedPlay(float coeff) = 0;
    virtual float GetCoeffSpeedPlay() const = 0;

    virtual bool GetIsOpened() const = 0;

    ///Запущено чтение кадров или нет! (ПАУЗА)
    virtual bool GetIsRun() const = 0;
    virtual void SetIsRun(bool tf) = 0;

    virtual bool GetIsWork() const = 0;

    virtual bool GetIsPlay() const = 0;

    ///Возвращает громкость в диапазоне [0, 100]! ( < 0 - error)
    virtual qint8 GetVolume() const = 0;

    ///Устанавливает громкость в диапазоне [0, 100]!
    virtual void SetVolume(qint8 volume) = 0;

protected:
    ///Сигнал в случае изменения состояния работы проигрывателя!
    virtual void Sign_IsWorkPlay(bool isWork) = 0;
    //Замена следующих методов:
    //virtual void Sign_ClosePlay() = 0;
    //virtual void Sign_OpenPlay() = 0;
};

#endif // IMYAUDIOPLAYER_H
