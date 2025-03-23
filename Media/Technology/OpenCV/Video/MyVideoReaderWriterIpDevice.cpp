#include "MyVideoReaderWriterIpDevice.h"

MyVideoReaderWriterIpDevice::MyVideoReaderWriterIpDevice(TypeWorkWriter typeWorkWriter,
                                                         FunConvertMat funConvertMat,
                                                         FunConvertImage funConvertImage,
                                                         uint8_t defFps, QObject* parent) :
    MyVideoReaderWriterDevice(typeWorkWriter, funConvertMat, funConvertImage, defFps, parent) {
    max_iNotFrame = 1;
}

ushort MyVideoReaderWriterIpDevice::GetFps() const {
    //! videoCapture->get(MY_CV_CAP_PROP_FPS) - с ip-камерами работает неправильно!
    //! (не показывает значение согласно вручную установленным настройкам)
    return defFps;
}

MyVideoReaderWriterIpDevice::IpInfo MyVideoReaderWriterIpDevice::G_GetIpInfo(const QString& path) {

    //const QString strTest = "rtsp://admin:12345678@192.168.2.108:554/Stream/Live/101";
    IpInfo ipInfo;
    auto listStr = path.split(':');
    ipInfo.login = listStr[1].mid(2);//убираем "//" перед логином!

    auto listStr2 = listStr[2].split('@');
    ipInfo.password = listStr2[0];
    ipInfo.toAdress = listStr2[1];

    return ipInfo;
}

bool MyVideoReaderWriterIpDevice::CheckIsDisconnect() {
    return false;//У ip-камеры файл dev отсутствует!
}
