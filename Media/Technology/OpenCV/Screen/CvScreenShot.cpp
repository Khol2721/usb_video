#include "CvScreenShot.h"

CvScreenShot::CvScreenShot(int posX, int posY, int width, int height) : posX(posX), posY(posY), width(width), height(height)
{
    display = XOpenDisplay(nullptr);
    root = XDefaultRootWindow(display);
}

void CvScreenShot::CreateScreenShot(cv::Mat& cvImg) {

    //const auto startTimeEpochMs = QDateTime::currentMSecsSinceEpoch();
    if(img != nullptr) {
        XDestroyImage(img);//~5mc
    }

    img = XGetImage(display, root, posX, posY, width, height, AllPlanes, ZPixmap);//~70mc + max random to 230!

    cvImg = cv::Mat(height, width, CV_8UC4, img->data);//CV_8UC4*/

    //cv::UMat umat = cvImg.getUMat(MY_CV_ACCESS_FAST);

    cvtColor(cvImg, cvImg, MY_CV_BGRA2BGR);//convert CV_8UC4 (RGB32) -> CV_8UC3 (RGB888) //~50mc
    //qDebug() << QString("timeoutTest = %1").arg(QDateTime::currentMSecsSinceEpoch() - startTimeEpochMs);
}

void CvScreenShot::CreateScreenShot(cv::UMat& uMatImg) {

    if(img != nullptr) {
        XDestroyImage(img);//~5mc
    }

    img = XGetImage(display, root, posX, posY, width, height, AllPlanes, ZPixmap);//~70mc + max random to 230!
    auto matImg = cv::Mat(height, width, CV_8UC4, img->data);//CV_8UC4*/

    //const auto startTimeEpochMs = QDateTime::currentMSecsSinceEpoch();
    uMatImg = matImg.getUMat(MY_CV_ACCESS_FAST);

    cvtColor(uMatImg, uMatImg, MY_CV_BGRA2BGR);//convert CV_8UC4 (RGB32) -> CV_8UC3 (RGB888) //~50mc (так же как Mat)
    //qDebug() << QString("timeoutTest = %1").arg(QDateTime::currentMSecsSinceEpoch() - startTimeEpochMs);
}


cv::Mat CvScreenShot::GetScreenShot() {
    cv::Mat cvImg;
    CreateScreenShot(cvImg);
    return cvImg;
}

CvScreenShot::~CvScreenShot() {
    if(img != nullptr) {
        XDestroyImage(img);
    }
    XCloseDisplay(display);
}

//----------------------------------------------
