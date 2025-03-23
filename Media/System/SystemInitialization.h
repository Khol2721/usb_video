#ifndef SYSTEMINITIALIZATION_H
#define SYSTEMINITIALIZATION_H

/// Сервер ядра ОС Linux (Video 4 Linux)!
#define VIDEO_SERVER_V4L

#include <QDebug>

#ifdef VIDEO_SERVER_V4L
#include <string.h>
#include <fcntl.h>//open file descriptor!
#include <unistd.h>//close file descriptor!
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#endif

typedef unsigned int uint;

class SystemInitialization
{
public:
    SystemInitialization() = delete;
    virtual ~SystemInitialization() = delete;

    ///Общий метод инициализации аналоговой камеры!
    static bool InitAnalogVideoDevice(const char* path) {

        #ifdef VIDEO_SERVER_V4L
        return SelectFormatInput(path,  V4L2_STD_PAL_D1, 0);
        #endif
    }

    #ifdef VIDEO_SERVER_V4L
    ///Установка формата обращения к устройству видео через V4L (достаточно установить 1 раз до перезагрузки системы)
    static bool SelectFormatInput(const char* path, v4l2_std_id std_id, uint number) {

        #define ERROR_DEVICE() { qDebug() << "SelectFormatInput: ERROR_DEVICE!"; close(pDevice); return false; }

        const auto pDevice = open(path, O_RDWR | O_NONBLOCK, 0);//создаем файловый дескриптор на чтение и запись (O_RDWR) в неблокирующем режиме (O_NONBLOCK)

        if(pDevice == -1) {

    #ifdef QT_DEBUG
            qDebug() << "Cannot open " << errno << strerror(errno);
    #endif
            ERROR_DEVICE();
        }

        struct v4l2_input input;
        memset(&input, 0, sizeof(input));
        input.index = number;

        if(ioctl(pDevice, VIDIOC_G_INPUT, &input.index) == -1){
    #ifdef QT_DEBUG
            qDebug() << "error VIDIOC_G_INPUT";
    #endif
            ERROR_DEVICE();
        }
        if(ioctl(pDevice, VIDIOC_ENUMINPUT, &input) == -1) {
    #ifdef QT_DEBUG
            qDebug() << "error VIDIOC_ENUMINPUT";
    #endif
            ERROR_DEVICE();
        }

        if((input.std & std_id) == 0) {
    #ifdef QT_DEBUG
            qDebug() << "Format: " << QString((char *)input.std) << "not suppoted";
    #endif

            ERROR_DEVICE();
        }
        if(ioctl(pDevice, VIDIOC_S_STD, &std_id) == -1) {
    #ifdef QT_DEBUG
            qDebug() << "Can`t change format: " << strerror(errno);
    #endif

            ERROR_DEVICE();
        }

        close(pDevice);
        return true;
    }
    #endif
};

#endif // SYSTEMINITIALIZATION_H
