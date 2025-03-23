#ifndef INFOOPENCV_H
#define INFOOPENCV_H

#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

#if CV_MAJOR_VERSION >= 4
#include "opencv2/videoio/registry.hpp"//появился в новой версии!
#endif

using std::string;
using std::vector;

class InfoOpenCV
{
public:
    InfoOpenCV() = delete;

    static string GetOpenCV_Version();

    static vector<string> GetVideoCapture_ListApiPreference();
};

#endif // INFOOPENCV_H
