#ifndef ICVSCREENMONITORS_H
#define ICVSCREENMONITORS_H

#include <memory>

#include <opencv2/opencv.hpp>

#include "../../../Interface/Screen/IScreenMonitors.h"

typedef cv::Mat MatScreen; //cv::UMat //cv::Mat
typedef std::shared_ptr<MatScreen> PtrMatScreen;

class ICvScreenMonitors : public IScreenMonitors
{
public:
    virtual PtrMatScreen GetPtrMatScreen() = 0;
};

#endif // ICVSCREENMONITORS_H
