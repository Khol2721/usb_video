#include "MyScreenFrame.h"

#include <QFile>
MyPtrImage MyScreenFrame::GetScreenFrame(const QString& path) {

    if(QFile(path).size() == 0) {
        return std::make_shared<QImage>();
    }

    auto screen = cv::imread(path.toStdString());//IMREAD_COLOR!

    MyPtrImage image = std::make_shared<QImage>(ConverterVariablesCV::CvMatToQImage(screen));

    return image;
}
