#include "InfoOpenCV.h"

string InfoOpenCV::GetOpenCV_Version() {
    return std::to_string(CV_MAJOR_VERSION) + "." +
            std::to_string(CV_MINOR_VERSION) + "." +
            std::to_string(CV_SUBMINOR_VERSION);
}

vector<string> InfoOpenCV::GetVideoCapture_ListApiPreference() {

    vector<string> listStrApi;

#if CV_MAJOR_VERSION >= 4
    auto listApi = cv::videoio_registry::getBackends();
    for(auto api : listApi) {
        listStrApi.insert(listStrApi.end(), cv::videoio_registry::getBackendName(api));
    }

#endif

    //Поскольку нет другого решения! (не известно)
    return listStrApi;
}
