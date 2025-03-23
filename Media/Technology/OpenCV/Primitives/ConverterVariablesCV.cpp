#include "ConverterVariablesCV.h"

QImage ConverterVariablesCV::CvMatToQImage(const Mat& inMat) {

    const auto type = inMat.type();

    switch (type)
    {
       // 8-bit, 4 channel
       case CV_8UC4:
       {
          QImage image( inMat.data,
                        inMat.cols, inMat.rows,
                        static_cast<int>(inMat.step),
                        QImage::Format_ARGB32);

          return image;
       }

       // 8-bit, 3 channel
       case CV_8UC3:
       {
          QImage image( inMat.data,
                        inMat.cols, inMat.rows,
                        static_cast<int>(inMat.step),
                        QImage::Format_RGB888);

          return image.rgbSwapped();
       }

       // 8-bit, 1 channel
       case CV_8UC1:
       {
          QImage image( inMat.data,
                        inMat.cols, inMat.rows,
                        static_cast<int>(inMat.step),
                        QImage::Format_Grayscale8);

          return image;
       }

       default:
          break;
    }

    return QImage();
}

QImage ConverterVariablesCV::CvMatToQImage(const UMat& inUMat) {

    auto inMat = inUMat.getMat(MY_CV_ACCESS_FAST);

    const auto type = inMat.type();

    switch (type)
    {
       // 8-bit, 4 channel
       case CV_8UC4:
       {
          QImage image( inMat.data,
                        inMat.cols, inMat.rows,
                        static_cast<int>(inMat.step),
                        QImage::Format_ARGB32);

          return image;
       }

       // 8-bit, 3 channel
       case CV_8UC3:
       {
          QImage image( inMat.data,
                        inMat.cols, inMat.rows,
                        static_cast<int>(inMat.step),
                        QImage::Format_RGB888);

          return image.rgbSwapped();
       }

       // 8-bit, 1 channel
       case CV_8UC1:
       {
          QImage image( inMat.data,
                        inMat.cols, inMat.rows,
                        static_cast<int>(inMat.step),
                        QImage::Format_Grayscale8);

          return image;
       }

       default:
          break;
    }

    return QImage();
}

Mat ConverterVariablesCV::QImageToCvMat(const QImage& image, bool& isToRGB) {

    Mat mat;

    const auto format = image.format();
    isToRGB = false;

    switch (format)
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), static_cast<size_t>(image.bytesPerLine()));
        break;
    case QImage::Format_RGB888:
        mat = Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), static_cast<size_t>(image.bytesPerLine()));
        cv::cvtColor(mat, mat, MY_CV_BGR2RGB);
        isToRGB = true;
        break;
    case QImage::Format_Grayscale8:
        mat = Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), static_cast<size_t>(image.bytesPerLine()));
        break;
    default:
        //qDebug() << "QImageToCvMat! (#0)";
        break;
    }

    return mat;
}
