#include "MyVideoReaderProxyMulty.h"

Q_DECLARE_METATYPE(std::shared_ptr<QImage>)
MyVideoReaderProxyMulty::MyVideoReaderProxyMulty(KeyVideoReader keyMain, IMyVideoReader* videoReader) : keyMainVideoReader(keyMain) {

    qRegisterMetaType<std::shared_ptr<QImage>>();

    const auto stateSubscribe = SubscribeVideoReader(keyMain, videoReader);

    if(!stateSubscribe) {
        throw;
    }

    keyCurrentVideoReader = keyMain;

    auto ptrMainVideoReader = GetMainVideoReader();

    connect(ptrMainVideoReader, &IMyVideoReaderWriter::NewFrame, this, &MyVideoReaderProxyMulty::NewFrame);
    connect(ptrMainVideoReader, &IMyVideoReaderWriter::Sign_ClosePlay, this, &MyVideoReaderProxyMulty::Sign_ClosePlay);

    connect(this, &MyVideoReaderProxyMulty::SetIsRun, ptrMainVideoReader, &IMyVideoReaderWriter::SetIsRun);
    connect(this, &MyVideoReaderProxyMulty::SetCoeffSpeedPlay, ptrMainVideoReader, &IMyVideoReaderWriter::SetCoeffSpeedPlay);
    connect(this, &MyVideoReaderProxyMulty::SetCurrentFramePlay, ptrMainVideoReader, &IMyVideoReaderWriter::SetCurrentFramePlay);
    connect(this, &MyVideoReaderProxyMulty::SetCurrentTimePlayMs, ptrMainVideoReader, &IMyVideoReaderWriter::SetCurrentTimePlayMs);

    connect(this, &MyVideoReaderProxyMulty::StartPlay, ptrMainVideoReader, &IMyVideoReaderWriter::StartPlay);
    connect(this, &MyVideoReaderProxyMulty::ClosePlay, ptrMainVideoReader, &IMyVideoReaderWriter::ClosePlay);
    connect(this, &MyVideoReaderProxyMulty::RestartPlay, ptrMainVideoReader, &IMyVideoReaderWriter::RestartPlay);
}

//IMyVideoPlayer:
ushort MyVideoReaderProxyMulty::GetFps() const {
    return GetMainVideoReader()->GetFps();
}

float MyVideoReaderProxyMulty::GetCoeffSpeedPlay() const {
    return GetMainVideoReader()->GetCoeffSpeedPlay();
}

bool MyVideoReaderProxyMulty::GetIsRun() const {
    return GetMainVideoReader()->GetIsRun();
}

bool MyVideoReaderProxyMulty::GetIsOpened() const {
    return GetMainVideoReader()->GetIsOpened();
}

quint64 MyVideoReaderProxyMulty::GetCountTimePlayMs() const {
    return GetMainVideoReader()->GetCountTimePlayMs();
}
quint64 MyVideoReaderProxyMulty::GetCurrentTimePlayMs() const {
    return GetMainVideoReader()->GetCurrentTimePlayMs();
}

//IMyVideoFrame and IMyVideoPlayer:
uint MyVideoReaderProxyMulty::GetCountFramePlay() const {
    return GetMainVideoReader()->GetCountFramePlay();
}

uint MyVideoReaderProxyMulty::GetCurrentFramePlay() const {
    return GetMainVideoReader()->GetCurrentFramePlay();
}

bool MyVideoReaderProxyMulty::SetSizeFrame(QSize size) {
    return GetMainVideoReader()->SetSizeFrame(size);
}

//IMyVideoReader:
void MyVideoReaderProxyMulty::SetVideoIdDevice(const QString& path, qint8 num) {
    return GetMainVideoReader()->SetVideoIdDevice(path, num);
}

void MyVideoReaderProxyMulty::SetPathFileDevice(const QString& path) {
    return GetMainVideoReader()->SetPathFileDevice(path);
}

void MyVideoReaderProxyMulty::SetNumDevice(qint8 num) {
    return GetMainVideoReader()->SetNumDevice(num);
}

bool MyVideoReaderProxyMulty::GetIsPlay() const {
    return GetMainVideoReader()->GetIsPlay();
}

bool MyVideoReaderProxyMulty::GetIsWork() const {
    return GetMainVideoReader()->GetIsWork();
}

//This:
KeyVideoReader MyVideoReaderProxyMulty::GetKeyMainVideoReader() const {
    return keyMainVideoReader;
}

KeyVideoReader MyVideoReaderProxyMulty::GetKeyCurrentVideoReader() const {
    return keyCurrentVideoReader;
}

bool MyVideoReaderProxyMulty::SubscribeVideoReader(KeyVideoReader key, IMyVideoReader* reader) {

    if(reader != nullptr) {
        mapMyVideoReader[key] = dynamic_cast<IMyVideoReaderWriter*>(reader);
        return true;
    }
    else {
        return false;
    }
}

bool MyVideoReaderProxyMulty::UnsubscribeVideoReader(KeyVideoReader key) {
    return mapMyVideoReader.remove(key);//remove возвращает кол-во удалений по повтору ключа!
}

bool MyVideoReaderProxyMulty::SwitchVideoReader(KeyVideoReader key) {


    if(mapMyVideoReader.value(key) != nullptr) {

        auto ptrMainVideoReader = GetMainVideoReader();

        if(key != keyMainVideoReader) {

            if(keyCurrentVideoReader != keyMainVideoReader) {
                auto ptrCurrentVideoReader = GetCurrentVideoReader();
                disconnect(ptrCurrentVideoReader, &IMyVideoReaderWriter::NewFrame, ptrMainVideoReader, &IMyVideoReaderWriter::SetExternalFrame);
            }

            auto newMyVideoReader = mapMyVideoReader[key];
            connect(newMyVideoReader, &IMyVideoReaderWriter::NewFrame, ptrMainVideoReader, &IMyVideoReaderWriter::SetExternalFrame);
            ptrMainVideoReader->SetIsExternalFrames(true);
        }
        else {
            ptrMainVideoReader->SetIsExternalFrames(false);
        }

        keyCurrentVideoReader = key;
        return true;
    }
    else {
        return false;
    }
}

inline IMyVideoReaderWriter* MyVideoReaderProxyMulty::GetCurrentVideoReader() const {
    return mapMyVideoReader[keyCurrentVideoReader];
}

inline IMyVideoReaderWriter* MyVideoReaderProxyMulty::GetMainVideoReader() const {
    return mapMyVideoReader[keyMainVideoReader];
}

int MyVideoReaderProxyMulty::GetWidth() const {
    return GetMainVideoReader()->GetWidth();
}

int MyVideoReaderProxyMulty::GetHeight() const {
    return GetMainVideoReader()->GetHeight();
}

//---------------
void MyVideoReaderProxyMulty::SetSystemConfigVideoDevice(SystemConfigVideoDevice type, bool isWork) {
    GetMainVideoReader()->SetSystemConfigVideoDevice(type, isWork);
}

bool MyVideoReaderProxyMulty::GetSystemConfigVideoDevice(SystemConfigVideoDevice type) const {
    return GetMainVideoReader()->GetSystemConfigVideoDevice(type);
}

void MyVideoReaderProxyMulty::InitSystemConfigVideoDevice() {
    GetMainVideoReader()->InitSystemConfigVideoDevice();
}
//---------------
