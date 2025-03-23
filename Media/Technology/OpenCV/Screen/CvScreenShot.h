#ifndef CVSCREENSHOT_H
#define CVSCREENSHOT_H

#include "../Primitives/DefinesOpenCV.h"
//#include <opencv2/opencv.hpp>
//! https://github.com/opencv/opencv/issues/7113

//test!
#include <QDebug>
#include <QDateTime>

//! Должны быть подключены последними! (из-за устаревшей реализации и неприемлемых переопределений)
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class CvScreenShot
{
public:
    CvScreenShot(int posX, int posY, int width, int height);
   virtual ~CvScreenShot();

    void CreateScreenShot(cv::UMat& cvImg);
    void CreateScreenShot(cv::Mat& cvImg);

    cv::Mat GetScreenShot();

private:
    const int posX, posY;
    const int width, height;

    Display* display;
    Window root;
    XImage* img {nullptr};
};

//-----------------------------------------

#endif // CVSCREENSHOT_H
