#ifndef MYVIDEOREADERPROXYMULTY_H
#define MYVIDEOREADERPROXYMULTY_H

#include <QSize>
#include <QString>
#include <QDebug>
#include <QMap>//не лучшее решение, т.к. частым будет обращение по ключу (переделай на QHash)

#include "IMyVideoReaderWriter.h"
#include "Media/Interface/Video/IMyVideoReader.h"

typedef quint8 KeyVideoReader;

///Паттерн Заместитель (Proxy) ->
///Multy обозначает, что он способен переключать выдачу изображений с любого из подписчиков MyVideoReader!
class MyVideoReaderProxyMulty : public QObject, public IMyVideoReader
{
    Q_OBJECT
public:
    MyVideoReaderProxyMulty(KeyVideoReader keyMain, IMyVideoReader* videoReader);
    virtual ~MyVideoReaderProxyMulty() = default;//очисткой MyVideoReader он не занимается!

    //IMyVideoPlayer:
    ushort GetFps() const override;

    float GetCoeffSpeedPlay() const override;

    bool GetIsRun() const override;
    bool GetIsOpened() const override;

    quint64 GetCountTimePlayMs() const override;
    quint64 GetCurrentTimePlayMs() const override;

    //IMyVideoFrame and IMyVideoPlayer:
    uint GetCountFramePlay() const override;
    uint GetCurrentFramePlay() const override;

    bool SetSizeFrame(QSize size) override;

    //IMyVideoReader:
    void SetVideoIdDevice(const QString& path, qint8 num) override;
    void SetPathFileDevice(const QString& path) override;
    void SetNumDevice(qint8 num) override;

    bool GetIsPlay() const override;
    bool GetIsWork() const override;

    int GetWidth() const override;
    int GetHeight() const override;

    void SetSystemConfigVideoDevice(SystemConfigVideoDevice type, bool isWork) override;
    bool GetSystemConfigVideoDevice(SystemConfigVideoDevice type) const override;
    void InitSystemConfigVideoDevice() override;

    //this:
    KeyVideoReader GetKeyMainVideoReader() const;
    KeyVideoReader GetKeyCurrentVideoReader() const;
    bool SubscribeVideoReader(KeyVideoReader key, IMyVideoReader* reader);
    bool UnsubscribeVideoReader(KeyVideoReader key);
    bool SwitchVideoReader(KeyVideoReader key);

signals:
    void NewFrame(std::shared_ptr<QImage> img) override;
    void Sign_ClosePlay() override;

    void SetIsRun(bool tf) override;
    void SetCoeffSpeedPlay(float coeff) override;
    bool SetCurrentFramePlay(uint posFrame) override;
    bool SetCurrentTimePlayMs(quint64 posTime) override;

    void RestartPlay() override;
    void StartPlay(QString path) override;
    void ClosePlay() override;

private:
    QMap<KeyVideoReader, IMyVideoReaderWriter*> mapMyVideoReader;
    const KeyVideoReader keyMainVideoReader;
    KeyVideoReader keyCurrentVideoReader;

    inline IMyVideoReaderWriter* GetCurrentVideoReader() const;
    inline IMyVideoReaderWriter* GetMainVideoReader() const;
};

#endif // MYVIDEOREADERPROXYMULTY_H
