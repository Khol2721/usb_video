#include "MyVideoReaderWriterDevice.h"

std::mutex MyVideoReaderWriterDevice::g_mutexInit { };

MyVideoReaderWriterDevice::MyVideoReaderWriterDevice(
        TypeWorkWriter typeWorkWriter,
        FunConvertMat funConvertMat,
        FunConvertImage funConvertImage,
        uint8_t defFps, QObject* parent) : IMyVideoReaderWriter(sizeFrame, defFps, parent),

    #if CV_MAJOR_VERSION >= 3
    apiPreference(CAP_ANY)//0 = CAP_ANY
    //для воспроизведения видеофайла CAP_FFMPEG!
    //для воспроизведения видеоустройства CAP_V4L2!
    #endif
{
    qDebug() << "MyVideoReaderWriterDevice::MyVideoReaderWriterDevice:" << QThread::currentThread();

    myVideoWriter = new MyVideoWriter(typeWorkWriter);

    isExternalFrames = false;
    ptrExternalFrame = nullptr;

    videoCapture = new VideoCapture;

    iNotFrame = 0;

    this->funConvertMat = funConvertMat;
    this->funConvertImage = funConvertImage;

    auto thread = new QThread;

    timerFrame = new QTimer();
    timerConnect = new QTimer();

    //! В этом случае таймеры и класс родитель выполняются в одном вторичном потоке!
    //! Иначе при moveToThread только this таймер проинициализировался бы в своем (самим собой созданном) вторичном потоке, что НЕВЕРНО!
    timerConnect->moveToThread(thread);
    timerFrame->moveToThread(thread);
    this->moveToThread(thread);

    timerFrame->setTimerType(Qt::TimerType::PreciseTimer);//CoarseTimer еще медленнее на 5-10% (что мало заметно),
    //однако задержка вся на обработке видео в read! //PreciseTimer!

    connect(timerFrame, SIGNAL(timeout()), this, SLOT(TimeoutFrame()));
    connect(thread, SIGNAL(started()), timerFrame, SLOT(start()));
    //timerFrame->start();

    timerConnect->setInterval(DEF_TIMEOUT_CONNECT);
    connect(timerConnect, SIGNAL(timeout()), this, SLOT(TimeoutConnect()));
    //timerConnect->start();

    connect(thread, SIGNAL(finished()), this, SLOT(CloseWork()));

    //Не трогай таймеры (не вызывай start/stop), они уже работают в нужном потоке!
    thread->start(QThread::Priority::TimeCriticalPriority);//бесполезно, т.к. read слишком много грузит!
    //QThread::Priority::TimeCriticalPriority
}

void MyVideoReaderWriterDevice::ReadFrame() {

    //qDebug() << "MyVideoReaderWriterDevice::ReadFrame:" << QThread::currentThread();

    auto ptrImage = GetImage();

    emit NewFrame(ptrImage);
}

MyPtrImage MyVideoReaderWriterDevice::GetImage() {

    #ifdef IS_SHOW_DEL_TIME_READ_FRAME
    const auto currentEpochMs = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "MyVideoReaderWriterDevice::GetImage: delTimeMs = " << currentEpochMs - previousTimeEpochMs;
    previousTimeEpochMs = currentEpochMs;
    #endif

    #ifndef IS_TEST_VIEW_IMAGE
    if(!GetIsOpened()) {
        //qDebug() << "!GetIsOpened()";
        return nullptr;
    }
    #endif

    const auto startTimeMs = QDateTime::currentMSecsSinceEpoch();

    //! Что read, что grab забирают себе поток пока не получат кадр изображения при работе IP-камер,
    //! Потому, в случае отсутствия обмена с конечной платформой придется делать переинициализацию обмена с камерами!
    auto ptrMat = cv::makePtr<cv::Mat>();
    //cv::Mat matIn;    
    #ifndef IS_TEST_VIEW_IMAGE
    bool isFrame = videoCapture->read(*ptrMat);//*ptrMat //matIn
    #else
    bool isFrame = true;
    auto& inMat = *ptrMat;
    inMat = Mat(sizeFrame.width(), sizeFrame.height(), CV_8UC4, cv::Scalar(255, 255, 255));

    //!Рисуем тестровый объект!
    cv::Size sizeSquare { 20, 20 };
    Point posLeftUp { (inMat.cols - sizeSquare.width)/2, (inMat.rows - sizeSquare.height)/2 };
    Point posRightDown { posLeftUp.x + sizeSquare.width, posLeftUp.y + sizeSquare.height };
    cv::rectangle(inMat, posLeftUp, posRightDown, cv::Scalar(255, 0, 0, 255), FILLED);//2) - толщина линии без заполнения, FILLED - заполнение!
    #endif

    //!(Выравнивание гистограммы) Быстрое, но только для ч/б изображения и возникают шумы!
    /*cv::Mat matInGray;
    cvtColor(matIn, matInGray, cv::COLOR_BGR2GRAY);//matInGray
    cv::equalizeHist(matInGray, *ptrMat);*/

    /*if(ptrMat.get() != nullptr) {//test!
        qDebug() << ptrMat.get()->cols << ptrMat.get()->rows;
    }*/

    //!(Выравнивание гистограммы 3x) Слишком долгое преобразование и возникают шумы!
    /*cv::Mat matInLab;
    cvtColor(matIn, matInLab, cv::COLOR_BGR2Lab);
    std::vector<cv::Mat> labChannels;
    cv::split(matInLab, labChannels);//дробим каналы!
    cv::Mat matOutLab_L;
    cv::equalizeHist(labChannels[0], matOutLab_L);
    labChannels[0] = matOutLab_L;
    cv::Mat labMerge;
    cv::merge(labChannels, labMerge);//объединяем каналы!
    cvtColor(labMerge, *ptrMat, cv::COLOR_Lab2BGR);*/

    //!(Логарифмическое преобразование) для 3x делаем для каждого канала (долгое преобразование даже для 1-го канала,
    //! но эффект есть в пользу черных объектов, но не в пользу светлых, шум в темных областях усилен слабо, что хорошо)!
    /*cv::Mat matInGray;
    cvtColor(matIn, matInGray, cv::COLOR_BGR2GRAY);
    cv::Mat imageNormalized;
    matInGray.convertTo(imageNormalized, CV_32F, 1.0/255.0);
    cv::Mat logTransformed;
    imageNormalized += 1;
    cv::log(imageNormalized, logTransformed);
    cv::normalize(logTransformed, logTransformed, 0, 255, cv::NORM_MINMAX);
    logTransformed.convertTo(*ptrMat, CV_8U);//!convertTo позволяет вызывать себя!*/

    //test!
    /*QString templatePathImitatorImages { ":/icon/%1.png" };
    auto in_im_front = QImage(QString(templatePathImitatorImages).arg("drone290x162"));
    auto _im_front = in_im_front.scaled(in_im_front.size()/2);
    auto im_front = _im_front.convertToFormat(QImage::Format_ARGB32);
    bool isToRGB;
    auto front = ConverterVariablesCV::QImageToCvMat(im_front, isToRGB);
    int posX { (ptrMat->cols-front.cols)/2 }, posY { (ptrMat->rows-front.rows)/2 };
    cv::Mat back = Mat(ptrMat->rows, ptrMat->cols, CV_8UC4, cv::Scalar(255, 255, 255));
    cv::Mat roi = back(cv::Rect(posX, posY, front.cols, front.rows));
    front.copyTo(roi);
    back.convertTo(back, CV_8UC3);
    ptrMat = cv::makePtr<cv::Mat>(back);*/
    auto& mat = *ptrMat;

    SetInfoMat(ptrMat, QDateTime::currentMSecsSinceEpoch());

    //Альтернатива read!
    /*const auto isFrame = videoCapture->grab();//ожидание данных!
    if(isFrame) {
        videoCapture->retrieve(mat);
    }*/
    //else {
    //    return saveImage;
    //}
    const auto readTimeMs = QDateTime::currentMSecsSinceEpoch() - startTimeMs;

    #ifdef IS_TIMER_TEST
    qDebug() << "MyVideoReaderWriterDevice::GetImage: TimeoutReadFrame =" << elTimerTest.elapsed();
    elTimerTest.restart();
    #endif

    #ifndef IS_TEST_VIEW_IMAGE
    if(CheckIsDisconnect()) {
        //qDebug() << "!CheckIsDisconnect()";
        Close();
        return nullptr;
    }
    #endif

    if(!isFrame || mat.empty()) {

        //Неприятное замечание, если отключить устройство и в течение 10 сек подключить,
        //то восстановление не сработает! (поскольку видео-файл получает индекс +1)
        //Освобождение ресурсов "release" не поможет тут!
        iNotFrame++;

        //! По причине того, что API может быть разное, использовать только API ffmpeg не совсем корректно
        //! (где можно установить флаги LIBAVFORMAT_INTERRUPT_OPEN_TIMEOUT_MS и LIBAVFORMAT_INTERRUPT_READ_TIMEOUT_MS)
        //! Проверка наличия файла по пути pathFileDevice не поможет, так как пока videoCapture его использует, он будет висеть!
        //qDebug() << "iNotFrame" << iNotFrame << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
        if(iNotFrame > max_iNotFrame) {
            Close();
        }

        return nullptr;
    }
    else {
        iNotFrame = 0;
    }

    #ifndef IS_TEST_VIEW_IMAGE
    //Автоконтроль частоты кадров, работает при установке частоты кадров по умочанию больше реально установленной в ip-камере!
    if(readTimeMs > timerFrame->interval() || isCheckReadInterval) {

        auto newIntervalMs { timerFrame->interval() };
        if(readTimeMs < DEF_TIMEOUT_CONNECT && !isCheckReadInterval) {
            #ifdef IS_AUTO_INTERVAL_READ_FRAME
            newIntervalMs += 1;
            #endif
        }
        else {
            qint64 delT = 0;
            while(delT < newIntervalMs) {//Очистка буфера после восстановления связи!
                const auto startReadT = QDateTime::currentMSecsSinceEpoch();
                cv::Mat matT;
                videoCapture->read(matT);
                delT = QDateTime::currentMSecsSinceEpoch() - startReadT;
                //qDebug() << delT;
            }
            #ifdef IS_AUTO_INTERVAL_READ_FRAME
            newIntervalMs -= 1;
            #endif
            isCheckReadInterval = false;
        }

        #ifdef IS_AUTO_INTERVAL_READ_FRAME
        timerFrame->setInterval(newIntervalMs);
        //qDebug() << "setInterval" << timerFrame->interval();
        #endif
    }
    #endif

    std::shared_ptr<QImage> baseImage = nullptr;
    if(!isExternalFrames) {
        if(funConvertMat != nullptr) {
            Mat out;
            out = funConvertMat(mat);

            if(!out.empty()) {
                baseImage = std::make_shared<QImage>(ConverterVariablesCV::CvMatToQImage(out));
            }
            else {
                baseImage = std::make_shared<QImage>(ConverterVariablesCV::CvMatToQImage(mat));
            }
        }
        else {
            baseImage = std::make_shared<QImage>(ConverterVariablesCV::CvMatToQImage(mat));
        }
    }
    else {
        if(ptrExternalFrame == nullptr) {
            return nullptr;
        }

        if(ptrExternalFrame->isNull()) {
            return nullptr;
        }

        baseImage = ptrExternalFrame;
    }

    WriteImageBeforeConvert(baseImage);

    qint64 delConvertMs { 0 };
    std::shared_ptr<QImage> image { nullptr };
    if(funConvertImage != nullptr) {
        const auto startConvertMs = QDateTime::currentMSecsSinceEpoch();
        auto newImage = funConvertImage(*baseImage);
        image = std::make_shared<QImage>(newImage);
        delConvertMs = QDateTime::currentMSecsSinceEpoch() - startConvertMs;
    }
    else {
        image = baseImage;
    }

    saveImage = image;
    WriteImageAfterConvert(saveImage);

    /*const auto delTimeMs = QDateTime::currentMSecsSinceEpoch() - startTimeMs;
    if(delTimeMs > timerFrame->interval()) {
        qDebug() << "Error time (ms)" << delTimeMs - timerFrame->interval()
                 << "read time (ms)" << readTimeMs
                 << "interval (ms)" << timerFrame->interval()
                 << "Convert (ms)" << delConvertMs;
    }*/

    return saveImage;
}

void MyVideoReaderWriterDevice::TimeoutFrame() {

    #ifndef IS_TEST_VIEW_IMAGE
    if(!GetIsOpened()) {
        return;
    }
    #endif

    if(!isRun) {
        return;
    }

    ReadFrame();
}

void MyVideoReaderWriterDevice::TimeoutConnect() {

    //qDebug() << "TimeoutConnect" << 1;
    if(GetIsOpened()) {

        #ifdef IS_AUTO_INTERVAL_READ_FRAME
        if(i_currentTimeoutConnect < 10) {
            i_currentTimeoutConnect++;
        }
        else {
            i_currentTimeoutConnect = 0;
            timerFrame->setInterval(timerFrame->interval() - 1);
            //qDebug() << "setInterval" << timerFrame->interval();
        }
        #endif

        return;
    }

    //qDebug() << "TimeoutConnect" << 2;
    if(!isRun) {
        return;
    }

    //qDebug() << "TimeoutConnect" << 3;
    Init();

    //qDebug() << Q_FUNC_INFO;
}

void MyVideoReaderWriterDevice::Start() {
    isRun = true;

    Init();
}

void MyVideoReaderWriterDevice::Stop() {
    isRun = false;
}

bool MyVideoReaderWriterDevice::Init() {

    qDebug() << "MyVideoReaderWriterDevice::Init: " << QThread::currentThread();

    /*if(sizeFrame.height() > 1000) {//test!
        return false;
    }*/
    //return false;//test!

    if(GetIsOpened()) {
        Close();
    }

    InitSystemConfigVideoDevice();

    bool isOpen { false };

    g_mutexInit.lock();
    if(videoId.GetIsInitNum()) {
        #if CV_MAJOR_VERSION < 3
        isOpen = videoCapture->open(videoId.GetNum());
        #else
        isOpen = videoCapture->open(videoId.GetNum(), apiPreference);
        #endif
    }
    else {
        #if CV_MAJOR_VERSION < 3
        isOpen = videoCapture->open(videoId.GetPath().toStdString());
        #else
        isOpen = videoCapture->open(videoId.GetPath().toStdString(), apiPreference);
        #endif
    }
    //!При работе IP камер выявлено, что OpenCV сервер не успевает проинициализировать 1-ю камеру перед инициализацией 2-й
    //!(что приводит к Segm. fault в videoCapture->open), задержки в 500мс достаточно!*/
    QThread::msleep(500);
    g_mutexInit.unlock();
    isCheckReadInterval = true;

    if(!timerConnect->isActive()) {
        timerConnect->start();
    }

    if(isOpen) {

        InitTimeoutTimerFrame();

        #ifdef IS_SHOW_DEL_TIME_READ_FRAME
        previousTimeEpochMs = QDateTime::currentMSecsSinceEpoch();
        #endif

        if(!timerFrame->isActive()) {
            timerFrame->start();
        }
    }

    return isOpen;
}

int MyVideoReaderWriterDevice::GetWidth() const {
    return static_cast<int>(videoCapture->get(MY_CV_CAP_PROP_FRAME_WIDTH));
}

int MyVideoReaderWriterDevice::GetHeight() const {
    return static_cast<int>(videoCapture->get(MY_CV_CAP_PROP_FRAME_HEIGHT));
}

bool MyVideoReaderWriterDevice::SetSizeFrame(QSize size) {

    if(!size.isNull() && !size.isEmpty()) {

        //Установка разрешения, если камера его поддерживает! (флаги возврата videoCapture->set НЕ РАБОТАЮТ!!!!)
        sizeFrame = size;

        if(!GetIsOpened()) {
            return true;
        }

        if(isSetResize) {
            const auto videoW = GetWidth();
            if(videoW != sizeFrame.width()) {
                videoCapture->set(MY_CV_CAP_PROP_FRAME_WIDTH, sizeFrame.width());
            }

            const auto videoH = GetHeight();
            if(videoH != sizeFrame.height()) {
                videoCapture->set(MY_CV_CAP_PROP_FRAME_HEIGHT, sizeFrame.height());//4
            }
            //QThread::msleep(TIME_SLEEP_INIT_SIZE_MS);//увы, но ему и 5 сек мало... (в общем, не работает)

            const auto newVideoW = GetWidth();
            if(newVideoW != sizeFrame.width()) {
                throw MyException("MyVideoReaderWriterDevice::SetSizeFrame: error set width!");
            }

            const auto newVideoH = GetHeight();
            if(newVideoH != sizeFrame.height()) {
                throw MyException("MyVideoReaderWriterDevice::SetSizeFrame: error set height!");
            }
        }

        if(!myVideoWriter->GetIsRun()) {
            myVideoWriter->SetSizeVideo(size);
        }
        return true;
    }
    else {
        return false;
    }
}

bool MyVideoReaderWriterDevice::GetIsOpened() const {

    const bool isOpened = videoCapture->isOpened();
    return isOpened;
}

bool MyVideoReaderWriterDevice::GetIsRun() const {
    return isRun;
}

int MyVideoReaderWriterDevice::GetTimeoutFrame() {

    auto fps = GetFps();

    //! Как показала практика, для устранения лишнего (возрастающего) ожидания при чтении кадра (->read) уровня OpenCV,
    //! необходимо ввести искусственную задержку на обработку кадра ( >= 1мс)!
    const auto addDalay = ADD_DELAY_TIMEOUT_READ_FRAME;

    const auto timeoutFrame = qCeil((double)(1000.0f/fps)) + addDalay;

    return timeoutFrame;
}

ushort MyVideoReaderWriterDevice::GetFps() const {

    if(!GetIsOpened()) {
        return defFps;
    }

    auto fps = static_cast<ushort>(videoCapture->get(MY_CV_CAP_PROP_FPS));

    if(fps > 0) {
        return fps;
    }
    else {
        return defFps;
    }
}

uint MyVideoReaderWriterDevice::GetCountFramePlay() const {
    return 0;
}

uint MyVideoReaderWriterDevice::GetCurrentFramePlay() const {
    return 0;
}

bool MyVideoReaderWriterDevice::SetCurrentFramePlay(uint) { return false; }


void MyVideoReaderWriterDevice::SetPathFileDevice(const QString& pathFile) {

    videoId = VideoId(pathFile);

    if(!GetIsRun()) {
        return;
    }

    Init();
}

void MyVideoReaderWriterDevice::SetNumDevice(qint8 num) {

    videoId = VideoId(num);

    if(!GetIsRun()) {
        return;
    }

    Init();
}

void MyVideoReaderWriterDevice::SetVideoIdDevice(const QString& path, qint8 num) {

    videoId = VideoId(path, num);

    if(!GetIsRun()) {
        return;
    }

    Init();
}

quint64 MyVideoReaderWriterDevice::GetCountTimePlayMs() const { return 0; }
quint64 MyVideoReaderWriterDevice::GetCurrentTimePlayMs() const { return 0; }
bool MyVideoReaderWriterDevice::SetCurrentTimePlayMs(quint64) { return false; }

void MyVideoReaderWriterDevice::RestartPlay() { }
void MyVideoReaderWriterDevice::StartPlay(QString) { }
void MyVideoReaderWriterDevice::ClosePlay() { }

bool MyVideoReaderWriterDevice::GetIsPlay() const { return false; }
void MyVideoReaderWriterDevice::SetCoeffSpeedPlay(float) { }

void MyVideoReaderWriterDevice::Close() {
    if(timerFrame->isActive()) {
        timerFrame->stop();
    }

    if(!GetIsOpened()) {
        return;
    }

    qDebug() << "MyVideoReaderWriterDevice::Close: " << QThread::currentThread();

    //Полагаю, что освобождение занимает время, за которое может сработать таймер на чтение изображения!
    videoCapture->release();
    iNotFrame = 0;
}

void MyVideoReaderWriterDevice::InitTimeoutTimerFrame() {

    //qDebug() << "MyVideoReaderWriterDevice::InitTimeoutTimerFrame:" << timerFrame->thread();

    const auto interval = GetTimeoutFrame();
    timerFrame->setInterval(interval);

    qDebug() << "MyVideoReaderWriterDevice::InitTimeoutTimerFrame: TimerFrame interval = " << interval;
}

void MyVideoReaderWriterDevice::SetIsRun(bool in) {
    in ? Start() : Stop();
    qDebug() << Q_FUNC_INFO << in;
}

float MyVideoReaderWriterDevice::GetCoeffSpeedPlay() const { return 1.f; }

bool MyVideoReaderWriterDevice::GetIsWork() const { return GetIsOpened(); }

//Multy:
bool MyVideoReaderWriterDevice::GetIsExternalFrames() const {
    return isExternalFrames;
}

void MyVideoReaderWriterDevice::SetIsExternalFrames(bool in) {
    isExternalFrames = in;
}

void MyVideoReaderWriterDevice::SetExternalFrame(std::shared_ptr<QImage> img) {
    ptrExternalFrame = img;
}

//---------------
void MyVideoReaderWriterDevice::SetSystemConfigVideoDevice(SystemConfigVideoDevice type, bool isWork) {
    mapSystemConfigVideoDevice[type] = isWork;
}

bool MyVideoReaderWriterDevice::GetSystemConfigVideoDevice(SystemConfigVideoDevice type) const {
    return mapSystemConfigVideoDevice[type];
}

void MyVideoReaderWriterDevice::InitSystemConfigVideoDevice() {
    QMapIterator<SystemConfigVideoDevice, bool> iter(mapSystemConfigVideoDevice);
    while (iter.hasNext()) {
        iter.next();
        if(iter.value()) {
            switch (iter.key()) {
                case SystemConfigVideoDevice::AnalogDevice:
                if(videoId.CheckPath()) {
                    auto stdStr = videoId.GetPath().toStdString();
                    SystemInitialization::InitAnalogVideoDevice(stdStr.data());
                }
                break;
            default:
                break;
            }
        }
    }
}

void MyVideoReaderWriterDevice::CloseWork() {

    qDebug() << "MyVideoReaderWriterDevice::CloseWork!";
    if(timerConnect->isActive()) {
        timerConnect->stop();
    }

    if(timerFrame->isActive()) {
        timerFrame->stop();
    }

    Close();
}

bool MyVideoReaderWriterDevice::WriteImageBeforeConvert(std::shared_ptr<QImage> ptrImage) {

    if(!myVideoWriter->GetIsRun()) {
        return false;
    }

    if(isWriteAfterConvert) {
        return false;
    }

    WriteFrame(ptrImage);

    return true;
}

bool MyVideoReaderWriterDevice::WriteImageAfterConvert(std::shared_ptr<QImage> ptrImage) {

    if(!myVideoWriter->GetIsRun()) {
        return false;
    }

    if(!isWriteAfterConvert) {
        return false;
    }

    WriteFrame(ptrImage);

    return true;
}

bool MyVideoReaderWriterDevice::CheckIsDisconnect() {

    MyFileInfo myFileInfo;
    if(videoId.CheckPath()) {
        if(myFileInfo.SearchFile(videoId.GetPath()) != FileStatus::TrueFile) {
            Close();
            qDebug() << "MyVideoReaderWriterDevice::CheckIsDisconnect:" << videoId.GetPath() + " not find!";
            return true;
        }
    }

    return false;
}

template<class T>
void MyVideoReaderWriterDevice::WriteFrame(std::shared_ptr<T> ptrFrame) {
    myVideoWriter->NewFrame(ptrFrame);
}
template void MyVideoReaderWriterDevice::WriteFrame<QImage>(std::shared_ptr<QImage> value);
template void MyVideoReaderWriterDevice::WriteFrame<cv::Mat>(std::shared_ptr<cv::Mat> value);

MyVideoWriter& MyVideoReaderWriterDevice::GetMyVideoWriter() {
    return *myVideoWriter;
}

void MyVideoReaderWriterDevice::SetIsWriteAfterConvert(bool in) {
    isWriteAfterConvert = in;
}

MyVideoReaderWriterDevice::~MyVideoReaderWriterDevice() {

    qDebug() << "MyVideoReaderWriterDevice::~MyVideoReaderWriter!";

    auto threadReader = this->thread();
    if(QThread::currentThread() != threadReader) {
        threadReader->quit();//даем команду на остановку!
        threadReader->wait();//ждем завершения!
    }
}
