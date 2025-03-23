QT       += core gui network
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    VideoPlay.cpp \
    VideoTile.cpp \
    Dialog/mymessagebox.cpp \
    helpers/itranslatetext.cpp \
    helpers/MyException.cpp \
    helpers/myfileinfo.cpp \
    helpers/myregexp.cpp \
    helpers/MyMessage.cpp \
    helpers/MyElapsedTimer.cpp \
    Media/System/SystemInitialization.cpp \
    Media/Interface/Screen/IScreenMonitors.cpp \
    Media/Technology/OpenCV/Filters/MyCvFilter1.cpp \
    Media/Technology/OpenCV/Filters/MyCvFilter2.cpp \
    Media/Technology/OpenCV/Filters/MyCvFilter3.cpp \
    Media/Technology/OpenCV/Filters/MyCvFilter4.cpp \
    Media/Technology/OpenCV/Filters/MyCvFilter5.cpp \
    Media/Technology/OpenCV/Filters/MyCvFilter6.cpp \
    Media/Technology/OpenCV/Filters/MyCvFilter7.cpp \
    Media/Technology/OpenCV/Filters/MyCvFilter8.cpp \
    Media/Technology/OpenCV/ImageConverters/MyCvFilterManager.cpp \
    Media/Technology/OpenCV/ImageConverters/MyMatCalibration.cpp \
    Media/Technology/OpenCV/Primitives/ConverterVariablesCV.cpp \
    Media/Technology/OpenCV/Primitives/InfoOpenCV.cpp \
    Media/Technology/OpenCV/Screen/CvScreenMonitors.cpp \
    Media/Technology/OpenCV/Screen/CvScreenShot.cpp \
    Media/Technology/OpenCV/Screen/MyScreenFrame.cpp \
    Media/Technology/OpenCV/Video/IMyVideoReaderWriter.cpp \
    Media/Technology/OpenCV/Video/MyVideoFrame.cpp \
    Media/Technology/OpenCV/Video/MyVideoReaderProxyMulty.cpp \
    Media/Technology/OpenCV/Video/MyVideoReaderWriter.cpp \
    Media/Technology/OpenCV/Video/MyVideoReaderWriterDevice.cpp \
    Media/Technology/OpenCV/Video/MyVideoReaderWriterIpDevice.cpp \
    Media/Technology/OpenCV/Video/MyVideoReaderWriterPlay.cpp \
    Media/Technology/OpenCV/Video/MyVideoWriter.cpp \
    Media/Technology/OpenCV/Filters/BildersCvFilters/BilderCvFilters.cpp \
    Media/Technology/OpenCV/Filters/BildersCvFilters/Interface/IBilderCvFilters.cpp \
    Media/Technology/OpenCV/Filters/Interface/MyCvFilter.cpp

HEADERS += \
    mainwindow.h \
    VideoPlay.h \
    VideoTile.h \
    Dialog/mymessagebox.h \
    helpers/itranslatetext.h \
    helpers/MyException.h \
    helpers/myfileinfo.h \
    helpers/myregexp.h \
    helpers/MyMessage.h \
    helpers/MyElapsedTimer.h \
    Media/Interface/Audio/IMyAudioPlayer.h \
    Media/Interface/Audio/IMyAudioReader.h \
    Media/Interface/Audio/IMyAudioWriter.h \
    Media/Interface/Screen/IMyScreenFrame.h \
    Media/Interface/Screen/IScreenMonitors.h \
    Media/Interface/Video/IFactoryVideo.h \
    Media/Interface/Video/IMyVideoFrame.h \
    Media/Interface/Video/IMyVideoPlayer.h \
    Media/Interface/Video/IMyVideoReader.h \
    Media/Interface/Video/IMyVideoWriter.h \
    Media/System/SystemInitialization.h \
    Media/Technology/OpenCV/Filters/MyCvFilter1.h \
    Media/Technology/OpenCV/Filters/MyCvFilter2.h \
    Media/Technology/OpenCV/Filters/MyCvFilter3.h \
    Media/Technology/OpenCV/Filters/MyCvFilter4.h \
    Media/Technology/OpenCV/Filters/MyCvFilter5.h \
    Media/Technology/OpenCV/Filters/MyCvFilter6.h \
    Media/Technology/OpenCV/Filters/MyCvFilter7.h \
    Media/Technology/OpenCV/Filters/MyCvFilter8.h \
    Media/Technology/OpenCV/ImageConverters/IImageConverter.h \
    Media/Technology/OpenCV/ImageConverters/MyCvFilterManager.h \
    Media/Technology/OpenCV/ImageConverters/MyMatCalibration.h \
    Media/Technology/OpenCV/Primitives/ConverterVariablesCV.h \
    Media/Technology/OpenCV/Primitives/DefinesOpenCV.h \
    Media/Technology/OpenCV/Primitives/InfoOpenCV.h \
    Media/Technology/OpenCV/Screen/CvScreenMonitors.h \
    Media/Technology/OpenCV/Screen/CvScreenShot.h \
    Media/Technology/OpenCV/Screen/ICvScreenMonitors.h \
    Media/Technology/OpenCV/Screen/MyScreenFrame.h \
    Media/Technology/OpenCV/Video/IMyVideoReaderWriter.h \
    Media/Technology/OpenCV/Video/MyVideoFrame.h \
    Media/Technology/OpenCV/Video/MyVideoReaderProxyMulty.h \
    Media/Technology/OpenCV/Video/MyVideoReaderWriter.h \
    Media/Technology/OpenCV/Video/MyVideoReaderWriterDevice.h \
    Media/Technology/OpenCV/Video/MyVideoReaderWriterIpDevice.h \
    Media/Technology/OpenCV/Video/MyVideoReaderWriterPlay.h \
    Media/Technology/OpenCV/Video/MyVideoWriter.h \
    Media/Technology/OpenCV/MyCvEnums.h \
    Media/Technology/OpenCV/Filters/BildersCvFilters/BilderCvFilters.h \
    Media/Technology/OpenCV/Filters/BildersCvFilters/Interface/IBilderCvFilters.h \
    Media/Technology/OpenCV/Filters/Interface/MyCvFilter.h

INCLUDEPATH += /usr/include/opencv4
LIBS += -lX11 -L/usr/lib -lopencv_highgui -lopencv_imgproc -lopencv_core -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_videoio -lopencv_imgcodecs

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
