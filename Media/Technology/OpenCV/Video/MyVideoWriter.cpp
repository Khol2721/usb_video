#include "MyVideoWriter.h"

const QString MyVideoWriter::g_customNameFileVideoInfo = "videoInfo.txt";

const QString& MyVideoWriter::G_GetCustomNameFileVideoInfo() {
    return g_customNameFileVideoInfo;
}

MyVideoWriter::MyVideoWriter(TypeWorkWriter typeWorkWriter) :
    typeWorkWriter(typeWorkWriter) {

    pathWriteVideo = "";
    pathWriteScreen = "";
    pathWriteFrameStart = "";
    fpsVideo = 0;

    writer = nullptr;
    isRun = false;
    isDoScrean = false;
}

MyVideoWriter::MyVideoWriter(
        TypeWorkWriter typeWorkWriter,
        const QString& pathWriteVideo,
        const QString& pathWriteScreen,
        const MyCvFourcc& myCvFourcc,
        const QSize& sizeVideo,
        double fpsVideo) : typeWorkWriter(typeWorkWriter) {

    SetPathWriteVideo(pathWriteVideo);
    SetPathWriteScreen(pathWriteScreen);

    this->myCvFourcc = myCvFourcc;
    this->fpsVideo = fpsVideo;
    this->sizeVideo = sizeVideo;

    isRun = false;
    isDoScrean = false;
    startTimeEpochMs = -1;

    writer = nullptr;
}

TypeWorkWriter MyVideoWriter::GetTypeWorkWriter() const {
    return typeWorkWriter;
}

bool MyVideoWriter::Start() {

    const std::lock_guard<std::mutex> lock(mutex);

    if(pathWriteVideo.isEmpty() || fpsVideo == 0) {
        throw new MyException("MyVideoWriter: Не было инициализации параметров!");
    }

    const auto cvFOURCC = MY_CV_FOURCC(
                myCvFourcc.name.c1,
                myCvFourcc.name.c2,
                myCvFourcc.name.c3,
                myCvFourcc.name.c4);

    writer = new cv::VideoWriter();

    writer->open(
                pathWriteVideo.toStdString().data(),
                cvFOURCC,
                fpsVideo,//*MY_CV_COEFF_FPS
                MY_CV_SIZE(sizeVideo.width(), sizeVideo.height()), true);

    if(!writer->isOpened()) {
        qDebug() << "MyVideoWriter: Не удалось создать видео-файл!";
        return false;
    }

    isRun = true;
    isDoScrean = false;
    startTimeEpochMs = QDateTime::currentMSecsSinceEpoch();
    frameStart = nullptr;

    return true;
}

void MyVideoWriter::Stop() {

    const std::lock_guard<std::mutex> lock(mutex);

    isRun = false;

    if(writer != nullptr) {
        writer->release();
        delete writer;
        writer = nullptr;
    }

    //---------------------------------------
    switch (typeWorkWriter) {
    case TypeWorkWriter::Custom: {

        /// Save duration write!
        const auto countTimeEpochMs = QDateTime::currentMSecsSinceEpoch() - startTimeEpochMs;
        auto strPathFolder = MyFileInfo::GetPathDownFolder(pathWriteVideo);

        //NEW!!!!
        const QString strPath = !strPathFolder.isEmpty() ?
                    strPathFolder + MyFileInfo::GetSeparator() + g_customNameFileVideoInfo : g_customNameFileVideoInfo;
        auto fileWriteVideoInfo = new QFile(strPath);

        if(fileWriteVideoInfo->open(QIODevice::Append)) {
            QByteArray bytes;
            qint64 len = fileWriteVideoInfo->write(bytes.insert(0, QString::number(countTimeEpochMs)));

            if(len <= 0)
                throw;

            fileWriteVideoInfo->close();

            qDebug() << "Writer countFrame = " << countFrame;
            countFrame = 0;
        }
        else {
            throw;
        }

        ///Save screen write!

        //У нас выходит битый файл воспроизведения (или первый скрин, что странно)
        if(frameStart == nullptr || frameStart->empty()) {
            frameStart = std::make_shared<Mat>(CreateNullFrame());
        }

        /*bool isToRGB = false;
        auto matStart = ConverterVariablesCV::QImageToCvMat(*frameStart, isToRGB);
        auto isWriteFrameStart = cv::imwrite(pathWriteFrameStart.toStdString(), matStart);
        if(isToRGB) {
            cv::cvtColor(matStart, matStart, MY_CV_RGB2BGR);
        }*/
        qDebug() << "pathWriteFrameStart = " << pathWriteFrameStart;
        auto isWriteFrameStart = cv::imwrite(pathWriteFrameStart.toStdString(), *frameStart);

        if(!isWriteFrameStart) {
            throw;
        }

        break;
    }
    default:
        break;
    }
    //------------------------------

    startTimeEpochMs = -1;
}

void MyVideoWriter::NewFrame(std::shared_ptr<QImage> ptrImage) {

    if(ptrImage == nullptr || ptrImage->isNull()) {
        return;
    }

    const std::lock_guard<std::mutex> lock(mutex);

    Mat mat;
    bool isToRGB = false;

    if(isDoScrean || isRun) {
        mat = ConverterVariablesCV::QImageToCvMat(*ptrImage, isToRGB);
    }
    else {
        return;
    }

    if(mat.empty()) {
        throw new MyException("MyVideoWriter: Not convert image! (no valid format)");
    }

    if(isDoScrean) {
        const auto frameScreen = CreateScreen(mat);
        cv::imwrite(pathWriteScreen.toStdString(), *frameScreen);
        isDoScrean = false;
    }

    if(isRun) {

        //ПОТОКИ... вылет!
        if(!writer->isOpened()) {
            return;
        }

        if(frameStart == nullptr) {
            if(!mat.empty()) {
                frameStart = CreateScreen(mat);
            }
        }

        writer->write(mat);
        countFrame++;

        //Возвращаем преобразование! (без него появляются мерцания цветов изображения)
        if(isToRGB) {
            cv::cvtColor(mat, mat, MY_CV_RGB2BGR);
        }
    }
}

template<class T>
void MyVideoWriter::NewFrame(std::shared_ptr<T> ptrCvFrame) {

    const std::lock_guard<std::mutex> lock(mutex);

    if(ptrCvFrame->empty()) {
        throw new MyException("MyVideoWriter: Not convert image! (no valid format)");
    }

    if(isDoScrean) {
        const auto frameScreen = CreateScreen(*ptrCvFrame);
        cv::imwrite(pathWriteScreen.toStdString(), *frameScreen);
        isDoScrean = false;
    }

    if(isRun) {

        if(!writer->isOpened()) {
            return;
        }

        if(frameStart == nullptr) {
            if(!ptrCvFrame->empty()) {
                frameStart = CreateScreen(*ptrCvFrame);
            }
        }

        //resize(*ptrMat, *ptrMat, cv::Size(), 0.5, 0.5, cv::INTER_AREA);
        writer->write(*ptrCvFrame);
        countFrame++;
    }
}
template void MyVideoWriter::NewFrame<cv::Mat>(std::shared_ptr<cv::Mat> ptrCvFrame);
#if CV_MAJOR_VERSION >= 4
template void MyVideoWriter::NewFrame<cv::UMat>(std::shared_ptr<cv::UMat> ptrCvFrame);
#endif

bool MyVideoWriter::GetIsRun() const {
    return isRun;
}

void MyVideoWriter::SetCodec(const MyCvFourcc newMyCvFourcc) {

    if(writer != nullptr) {
        return;
    }

    myCvFourcc = newMyCvFourcc;
}

void MyVideoWriter::SetPathWriteVideo(const QString& newPath, FormatFileSaveVideo formatFileSaveVideo) {

    if(writer != nullptr) {
        return;
    }

    const auto strFormatVideo = G_GetStrFormatVideo(formatFileSaveVideo);

    if(strFormatVideo.isNull() || strFormatVideo.isEmpty()) {
        throw new MyException("MyVideoWriter: Неопределенный формат файла записи видео!");
    }

    pathWriteVideo = QString("%1.%2").arg(newPath).arg(strFormatVideo);

    const auto strFormatScreen = G_GetStrFormatScreen(FormatFileSaveScreen::PNG);
    pathWriteFrameStart = QString("%1.%2").arg(newPath).arg(strFormatScreen);
}

void MyVideoWriter::SetPathWriteScreen(const QString& newPath, FormatFileSaveScreen formatFileSaveScreen) {

    const auto strFormatScreen = G_GetStrFormatScreen(formatFileSaveScreen);

    if(strFormatScreen.isNull() || strFormatScreen.isEmpty()) {
        throw new MyException("MyVideoWriter: Неопределенный формат файла записи изображения!");
    }

    pathWriteScreen = QString("%1.%2").arg(newPath).arg(strFormatScreen);
}

void MyVideoWriter::SetSizeVideo(const QSize& size) {

    if(writer != nullptr) {
        return;
    }

    sizeVideo = size;
}

void MyVideoWriter::SetRectScreen(const QRect& rect) {

    const std::lock_guard<std::mutex> lock(mutex);

    rectScreen = rect;
}

void MyVideoWriter::SetFps(double fps) {

    if(writer != nullptr) {
        return;
    }

    fpsVideo = fps;
}

void MyVideoWriter::DoScreen() {
    isDoScrean = true;
}

QString MyVideoWriter::GetNameFileVideo() const {
    return MyFileInfo::GetNameFile(pathWriteVideo);
}

QString MyVideoWriter::GetNameFileScreen() const {
    return MyFileInfo::GetNameFile(pathWriteScreen);
}

QString MyVideoWriter::G_GetDefStrFormatVideo() {
    return G_GetStrFormatVideo(FormatFileSaveVideo::Def);
}

QString MyVideoWriter::G_GetDefStrFormatScreen() {
    return G_GetStrFormatScreen(FormatFileSaveScreen::Def);
}

QString MyVideoWriter::G_GetStrFormatVideo(FormatFileSaveVideo format) {

    switch (format) {
        case FormatFileSaveVideo::AVI:
        return "avi";
    default:
        return nullptr;
    }
}

QString MyVideoWriter::G_GetStrFormatScreen(FormatFileSaveScreen format) {
    switch (format) {
        case FormatFileSaveScreen::JPG:
        return "jpg";
        case FormatFileSaveScreen::PNG:
        return "png";
    default:
        return nullptr;
    }
}

qint64 MyVideoWriter::GetStartTimeEpochMs() const {
    return startTimeEpochMs;
}

qint64 MyVideoWriter::GetCurrentTimeMs() const {
    if(isRun) {
        return QDateTime::currentMSecsSinceEpoch() - startTimeEpochMs;
    }
    else {
        return -1;
    }
}

const QString& MyVideoWriter::GetNameFileVideoInfo() const {
    return g_customNameFileVideoInfo;
}

std::shared_ptr<Mat> MyVideoWriter::CreateScreen(const Mat& mat) const {

    if(!rectScreen.size().isEmpty()) {
        cv::Rect rect { rectScreen.x(), rectScreen.y(), rectScreen.width(), rectScreen.height() };
        const auto matScreen = mat(rect);
        return std::make_shared<Mat>(matScreen);
    }
    else {
        return std::make_shared<Mat>(mat);
    }
}

std::shared_ptr<Mat> MyVideoWriter::CreateScreen(const UMat& umat) const {

    auto mat = umat.getMat(MY_CV_ACCESS_FAST);

    if(!rectScreen.size().isEmpty()) {
        cv::Rect rect { rectScreen.x(), rectScreen.y(), rectScreen.width(), rectScreen.height() };
        const auto matScreen = mat(rect);
        return std::make_shared<Mat>(matScreen);
    }
    else {
        return std::make_shared<Mat>(mat);
    }
}

Mat MyVideoWriter::CreateNullFrame() const {
    cv::Mat mat(sizeVideo.height(), sizeVideo.width(), CV_8UC3, cv::Scalar(255, 255, 255));
    return mat;
}

MyVideoWriter::~MyVideoWriter() {
    Stop();
}
