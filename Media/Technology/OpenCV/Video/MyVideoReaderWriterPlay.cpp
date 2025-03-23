#include "MyVideoReaderWriterPlay.h"

MyVideoReaderWriterPlay::MyVideoReaderWriterPlay(
        TypeWorkWriter typeWorkWriter,
        IScreenMonitors* iScreenMonitors,
        uint8_t defFps, QObject* parent) : IMyVideoReaderWriter(QSize(), defFps, parent),

    #if CV_MAJOR_VERSION >= 3
    apiPreference(CAP_ANY),//0 = CAP_ANY
    //для воспроизведения видеофайла CAP_FFMPEG!
    //для воспроизведения видеоустройства CAP_V4L2!
    #endif
    iScreenMonitors(iScreenMonitors)
{
    qDebug() << "MyVideoReaderWriterPlay -> MyVideoReaderWriterPlay: " << QThread::currentThread();

    myVideoWriter = new MyVideoWriter(typeWorkWriter);

    videoCapture = new VideoCapture;

    iNotFrame = 0;

    pathFilePlay = "";
    countTimePlayMs = 0;

    auto thread = new QThread;

    timerFrame = new QTimer();

    //! В этом случае таймеры и класс родитель выполняются в одном вторичном потоке!
    //! Иначе при moveToThread только this таймер проинициализировался бы в своем (самим собой созданном) вторичном потоке, что НЕВЕРНО!
    timerFrame->moveToThread(thread);
    this->moveToThread(thread);

    timerFrame->setTimerType(Qt::TimerType::PreciseTimer);//CoarseTimer еще медленнее на 5-10% (что мало заметно),
    //однако задержка вся на обработке видео в read! //PreciseTimer!

    SetCoeffSpeedPlay();

    InitTimeoutTimerFrame();//play!

    connect(timerFrame, SIGNAL(timeout()), this, SLOT(TimeoutFrame()));
    connect(thread, SIGNAL(started()), timerFrame, SLOT(start()));
    //timerFrame->start();

    connect(thread, SIGNAL(finished()), this, SLOT(CloseWork()));

    //Не трогай таймеры (не вызывай start/stop), они уже работают в нужном потоке!
    thread->start(QThread::Priority::TimeCriticalPriority);//бесполезно, т.к. read слишком много грузит!
    //QThread::Priority::TimeCriticalPriority
}

void MyVideoReaderWriterPlay::ReadFrame() {

    //qDebug() << "MyVideoReaderWriterPlay::ReadFrame:" << QThread::currentThread();

    if(!isPlay) {
        return;
    }

    auto ptrImage = GetImage();

    if(ptrImage == nullptr) {//play!
        ClosePlay();
        emit Sign_ClosePlay();
        return;
    }
    emit NewFrame(ptrImage);
}

MyPtrImage MyVideoReaderWriterPlay::GetImage() {

    #ifdef IS_SHOW_DEL_TIME_READ_FRAME
    const auto currentEpochMs = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "MyVideoReaderWriterPlay::GetImage: delTimeMs = " << currentEpochMs - previousTimeEpochMs;
    previousTimeEpochMs = currentEpochMs;
    #endif

    if(!isPlay) {
        return nullptr;
    }

    if(!videoCapture->isOpened()) {
        return nullptr;
    }

    //play!
    int delFrame { 1 };
    if(myVideoWriter->GetTypeWorkWriter() == TypeWorkWriter::Custom) {

        if(myElapsedTimer.GetIsStop()) {
            myElapsedTimer.Start();
        }

        auto countTimePlayMs = (quint64)GetCountTimePlayMs();
        auto countFramePlay = GetCountFramePlay();

        currentTimePlayMs = CreateCurrentTimeCustomPlayMs();

        auto currentFrame = qFloor((float)countFramePlay*(float)currentTimePlayMs/(float)countTimePlayMs);


        delFrame = currentFrame - tikFrame;

        if(saveImage != nullptr) {
            if(delFrame < 1) {
                return saveImage;
            }

            tikFrame += delFrame;
        }
        else {
            delFrame = 1;
            tikFrame++;
        }
    }

    bool isFrame { false };
    cv::Mat mat;
    for(int i = 0; i < delFrame; i++) {
        isFrame = videoCapture->read(mat);
    }

    #ifdef IS_TIMER_TEST
        qDebug() << "TimeoutReadFrame =" << elTimerTest.elapsed();
        elTimerTest.restart();
    #endif

    if(!isFrame || mat.empty()) {

        //Неприятное замечание, если отключить устройство и в течение 10 сек подключить,
        //то восстановление не сработает! (поскольку видео-файл получает индекс +1)
        //Освобождение ресурсов "release" не поможет тут!
        iNotFrame++;

        //! По причине того, что API может быть разное, использовать только API ffmpeg не совсем корректно
        //! (где можно установить флаги LIBAVFORMAT_INTERRUPT_OPEN_TIMEOUT_MS и LIBAVFORMAT_INTERRUPT_READ_TIMEOUT_MS)
        //! Проверка наличия файла по пути pathFileDevice не поможет, так как пока videoCapture его использует, он будет висеть!

        if(iNotFrame > MAX_I_NOT_FRAME) {
            Close();
        }

        return nullptr;
    }

    iNotFrame = 0;

    //play!
    auto baseImage = std::make_shared<QImage>(ConverterVariablesCV::CvMatToQImage(mat));

    saveImage = baseImage;

    if(myVideoWriter->GetIsRun()) {
        WriteFrame(baseImage);
    }

    return saveImage;
}

void MyVideoReaderWriterPlay::TimeoutFrame() {

    if(isPlay) {
        if(!GetIsOpened()) {
            return;
        }

        if(!isRun) {
            return;
        }

        ReadFrame();
        return;
    }

    if(myVideoWriter->GetIsRun()) {

        auto iCvScreenMonitors = dynamic_cast<ICvScreenMonitors*>(iScreenMonitors);
        if(iCvScreenMonitors != nullptr) {

            auto ptrMat = iCvScreenMonitors->GetPtrMatScreen();
            WriteFrame(ptrMat);

            if(ptrMat != nullptr && !ptrMat->empty()) {
                auto image = std::make_shared<QImage>(ConverterVariablesCV::CvMatToQImage(*ptrMat));
                emit NewFrame(image);
            }
        }
        else {
            auto ptrImage = iScreenMonitors->GetPtrImageScreen();
            WriteFrame(ptrImage);

            emit NewFrame(ptrImage);
        }
    }
}

void MyVideoReaderWriterPlay::Start() {
    isRun = true;
    myElapsedTimer.SetPause(false);

    if(!isPlay) {
        //Нам нужен всегда работающий таймер!
        if(!timerFrame->isActive()) {
            timerFrame->start();
        }
    }
}

void MyVideoReaderWriterPlay::Stop() {
    isRun = false;
    myElapsedTimer.SetPause(true);
}

bool MyVideoReaderWriterPlay::Init() {

    qDebug() << "MyVideoReaderWriterPlay::Init: " << QThread::currentThread();

    if(!isPlay) {
        return false;
    }

    if(GetIsOpened()) {
        Close();
    }

    bool isOpen { false };
    if(!pathFilePlay.isNull() && !pathFilePlay.isEmpty()) {
        #if CV_MAJOR_VERSION < 3
        isOpen = videoCapture->open(pathFilePlay.toStdString());
        #else
        isOpen = videoCapture->open(pathFilePlay.toStdString(), CAP_FFMPEG);
        #endif
    }
    else {
        return false;
    }

    if(isOpen) {

        SetCoeffSpeedPlay();
        InitTimeoutTimerFrame();

        #ifdef IS_SHOW_DEL_TIME_READ_FRAME
        previousTimeEpochMs = QDateTime::currentMSecsSinceEpoch();
        #endif

        if(!timerFrame->isActive()) {
            timerFrame->start();
        }

        InitCountTimePlayMs();
    }

    return isOpen;
}

int MyVideoReaderWriterPlay::GetWidth() const {
    return static_cast<int>(videoCapture->get(MY_CV_CAP_PROP_FRAME_WIDTH));
}

int MyVideoReaderWriterPlay::GetHeight() const {
    return static_cast<int>(videoCapture->get(MY_CV_CAP_PROP_FRAME_HEIGHT));
}

bool MyVideoReaderWriterPlay::SetSizeFrame(QSize size) {

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

bool MyVideoReaderWriterPlay::GetIsOpened() const {

    const bool isOpened = videoCapture->isOpened();
    return isOpened;
}

bool MyVideoReaderWriterPlay::GetIsRun() const {
    return isRun;
}

int MyVideoReaderWriterPlay::GetTimeoutFrame() {

    auto fps = GetFps();

    const auto timeoutFrame = qCeil((double)(1000.0f/fps));

    return timeoutFrame;
}

ushort MyVideoReaderWriterPlay::GetFps() const {

    if(!GetIsOpened()) {
        return defFps;
    }
    #if CV_MAJOR_VERSION < 4
    //! set(MY_CV_CAP_PROP_POS_FRAMES... работает при этом исправно!
    return defFps;
    #else
    auto fps = static_cast<ushort>(videoCapture->get(MY_CV_CAP_PROP_FPS));

    if(fps == 0) {
        return defFps;
    }
    return fps;
    #endif
}

uint MyVideoReaderWriterPlay::GetCountFramePlay() const {

    auto count = static_cast<uint>(videoCapture->get(MY_CV_CAP_PROP_FRAME_COUNT));

    #if CV_MAJOR_VERSION < 4
    //! set(MY_CV_CAP_PROP_POS_FRAMES... работает при этом исправно!
    auto fps = static_cast<float>(videoCapture->get(MY_CV_CAP_PROP_FPS));
    return static_cast<float>(count)/fps*static_cast<float>(GetFps());
    #endif

    return count;
}

uint MyVideoReaderWriterPlay::GetCurrentFramePlay() const {
    auto current = static_cast<uint>(videoCapture->get(MY_CV_CAP_PROP_POS_FRAMES));

    #if CV_MAJOR_VERSION < 4
    //! set(MY_CV_CAP_PROP_POS_FRAMES... работает при этом исправно!
    auto fps = static_cast<float>(videoCapture->get(MY_CV_CAP_PROP_FPS));
    return static_cast<float>(current)/fps*static_cast<float>(GetFps());
    #endif

    return current;
}

bool MyVideoReaderWriterPlay::SetCurrentFramePlay(uint posFrame) {

    if(!GetIsOpened()) {
        return false;
    }

    switch (myVideoWriter->GetTypeWorkWriter()) {
    case TypeWorkWriter::Normal:
        return videoCapture->set(MY_CV_CAP_PROP_POS_FRAMES, posFrame);//в Custom реализации нельзя использовать напрямую!
    case TypeWorkWriter::Custom: {

        //Пропорция!
        const auto posTime = (int64)((float)currentTimePlayMs*(float)posFrame/(float)tikFrame);

        const auto delTimeMs = (int64)posTime - myElapsedTimer.GetElapsedMs();
        //qDebug() << "posTime = " <<  posTime << " delTimeMs = " << delTimeMs;
        myElapsedTimer.AddElapsedMs(delTimeMs);


        tikFrame = posFrame;
        currentTimePlayMs = posTime;
        startSpeedTimePlayMs = currentTimePlayMs;

        #if CV_MAJOR_VERSION < 4
        auto fps = static_cast<double>(videoCapture->get(MY_CV_CAP_PROP_FPS));
        bool isComplete = videoCapture->set(MY_CV_CAP_PROP_POS_FRAMES, static_cast<double>(tikFrame)*fps/static_cast<double>(GetFps()));
        #else
        bool isComplete = videoCapture->set(MY_CV_CAP_PROP_POS_FRAMES, tikFrame);
        #endif

        return isComplete;
    }
    default:
        throw MyException("MyVideoReaderWriterPlay::SetCurrentFramePlay: error myVideoWriter->GetTypeWorkWriter()");
    }
}


void MyVideoReaderWriterPlay::SetPathFileDevice(const QString&) { }

void MyVideoReaderWriterPlay::SetNumDevice(qint8) { }

void MyVideoReaderWriterPlay::SetVideoIdDevice(const QString&, qint8) { }

quint64 MyVideoReaderWriterPlay::GetCountTimePlayMs() const {
    return countTimePlayMs;
}

quint64 MyVideoReaderWriterPlay::GetCurrentTimePlayMs() const {

    if(!GetIsOpened()) {
        return 0;
    }

    return currentTimePlayMs;
}

bool MyVideoReaderWriterPlay::SetCurrentTimePlayMs(quint64 posTime) {

    if(!GetIsOpened()) {
        return false;
    }

    switch (myVideoWriter->GetTypeWorkWriter()) {
    case TypeWorkWriter::Normal:
        return videoCapture->set(MY_CV_CAP_PROP_POS_MSEC, posTime);//в Custom реализации нельзя использовать напрямую!
    case TypeWorkWriter::Custom: {

        bool isComplete = false;
        const auto delTimeMs = (int64)posTime - myElapsedTimer.GetElapsedMs();
        //qDebug() << "posTime = " <<  posTime << " delTimeMs = " << delTimeMs;
        myElapsedTimer.AddElapsedMs(delTimeMs);


        //Пропорция!
        const auto newTikFrame = (int64)((float)tikFrame*(float)posTime/(float)currentTimePlayMs);
        //qDebug() << "newTikFrame = " << newTikFrame << "; tikFrame = " << tikFrame;

        tikFrame = newTikFrame;
        currentTimePlayMs = posTime;
        startSpeedTimePlayMs = currentTimePlayMs;

        //Это повтор предыдущего вызова AddElapsedMs!!!!
        //myElapsedTimer.AddElapsedMs(currentTimePlayMs - myElapsedTimer.GetElapsedMs());

        #if CV_MAJOR_VERSION < 4//КОСТЫЛЬ!
        auto fps = static_cast<double>(videoCapture->get(MY_CV_CAP_PROP_FPS));
        isComplete = videoCapture->set(MY_CV_CAP_PROP_POS_FRAMES, static_cast<double>(tikFrame)*fps/static_cast<double>(GetFps()));
        #else
        isComplete = videoCapture->set(MY_CV_CAP_PROP_POS_FRAMES, tikFrame);
        #endif

        //! Not work!
        //isComplete = videoCapture->set(MY_CV_CAP_PROP_POS_AVI_RATIO, static_cast<double>(currentTimePlayMs)/static_cast<double>(GetCountTimePlayMs()));
        return isComplete;
    }
    default:
        throw MyException("MyVideoReaderWriterPlay::SetCurrentTimePlayMs: error myVideoWriter->GetTypeWorkWriter()");
    }
}

void MyVideoReaderWriterPlay::RestartPlay() {
    StartPlay(pathFilePlay);
}

void MyVideoReaderWriterPlay::StartPlay(QString path) {

    qDebug() << "MyVideoReader -> StartPlay: " << QThread::currentThread();

    if(path.isNull() || path.isEmpty()) {
        return;
    }

    //Проверять по адрессам не корректно, потому проверяем посимвольно:
    //https://ravesli.com/urok-2-stroki-v-qt5/#toc-7
    if(GetIsWork() && isPlay) {
        if(QString::compare(pathFilePlay, path) == STR_EQUAL) {
            videoCapture->set(MY_CV_CAP_PROP_POS_AVI_RATIO, 0);//вернет в начало!
            //Если переинициализировать не тужно то все верно!
        }
    }

    pathFilePlay = path;
    isPlay = true;

    Init();

    myElapsedTimer.Stop();
    Start();
}

void MyVideoReaderWriterPlay::ClosePlay() {

    //Выключаем воспроизведение видео-файла и переключаемся на показ видео с устройства!

    if(!GetIsPlay()) {
        return;
    }

    if(GetIsOpened()) {
        Close();
    }

    isPlay = false;

    Init();
    SetIsRun(true);

    tikFrame = 0;
    currentTimePlayMs = 0;
    startSpeedTimePlayMs = 0;
}

void MyVideoReaderWriterPlay::SetIsRun(bool in) {
    in ? Start() : Stop();
}

void MyVideoReaderWriterPlay::Close() {

    if(timerFrame->isActive()) {
        timerFrame->stop();
    }

    if(!GetIsOpened()) {
        return;
    }

    qDebug() << "MyVideoReaderWriterPlay::Close: " << QThread::currentThread();

    //Полагаю, что освобождение занимает время, за которое может сработать таймер на чтение изображения!
    videoCapture->release();
    iNotFrame = 0;
}

bool MyVideoReaderWriterPlay::GetIsPlay() const {
    return isPlay;//play!
}

void MyVideoReaderWriterPlay::SetCoeffSpeedPlay(float coeff) {

    if(coeff < 0.f) {
        throw;//TODO: Механизм обратной перемотки не реализован! (уже не уверен)
    }

    //Заглушка!
    if(qAbs(coeffSpeed - coeff) < 0.001f) {
        return;
    }

    coeffSpeed = coeff;

    //qDebug() << "MyVideoReader -> SetCoeffSpeedPlay: " << QThread::currentThread();

    startSpeedTimePlayMs = currentTimePlayMs;
    myElapsedTimer.AddElapsedMs(currentTimePlayMs - myElapsedTimer.GetElapsedMs());
}

void MyVideoReaderWriterPlay::InitTimeoutTimerFrame() {
    //qDebug() << "MyVideoReader -> timerFrame: " << timerFrame->thread();

    const auto interval = GetTimeoutFrame();
    timerFrame->setInterval(interval);

    qDebug() << "TimerFrame interval = " << interval;
}

float MyVideoReaderWriterPlay::GetCoeffSpeedPlay() const {
    return coeffSpeed;
}

bool MyVideoReaderWriterPlay::GetIsWork() const { return GetIsOpened(); }

void MyVideoReaderWriterPlay::InitCountTimePlayMs() {

    switch (myVideoWriter->GetTypeWorkWriter()) {
    case TypeWorkWriter::Normal: {
        //! В версии 4.4.0 не работает! (на 15 сек видео выдает 33мс..., может дело в отсутствии libv4l-dev)
        #if CV_MAJOR_VERSION < 4
        //Поскольку такого параметра в API не предусмотрено, то поступим иначе:
        //https://stackoverflow.com/questions/49048111/how-to-get-the-duration-of-video-using-cv2

        //Cправка: https://docs.opencv.org/3.4/d4/d15/group__videoio__flags__base.html

        const auto currentFrame = GetCurrentFramePlay();

        videoCapture->set(MY_CV_CAP_PROP_POS_AVI_RATIO, 1.0);//1 - устанавливаем позицию в конец файла!

        //SetCurrentFramePlay(GetCountFramePlay());//альтернатива!
        countTimePlayMs = GetCurrentTimePlayMs();

        //Возвращаем!
        //videoCapture.set(CAP_PROP_POS_AVI_RATIO, 0);//вернет в начало! (пойдет только при старте воспроизведения)
        SetCurrentFramePlay(currentFrame);
        #else
        //! ЭТО НЕ ВЕРНО!------------------
        //Не корректно, так как недостает 2 сек для 20 сек видео! (для 60 сек видео время 6 сек -> линейная зависимость!)
        //const auto tempCountTimePlayMs = GetCountFramePlay()*GetFps();
        //countTimePlayMs = static_cast<quint64>(qCeil(static_cast<double>(tempCountTimePlayMs)*(1.081)));//qRound - порог 0.5! qCeil -> 1.1 - 2!
        //1.081 подобран для 1м камеры для 20 мин!
        //-----------------------------------

        const auto tempCountTimePlayMs = GetCountFramePlay()*GetFps();
        countTimePlayMs = static_cast<quint64>(tempCountTimePlayMs);
        //!----------------------------
        #endif
        break;
    }
    case TypeWorkWriter::Custom: {
        //Костыль!
        const QString strPath = MyFileInfo::GetPathDownFolder(pathFilePlay) + MyFileInfo::GetSeparator() + myVideoWriter->GetNameFileVideoInfo();
        auto fileReadVideoInfo = new QFile(strPath);
        if(fileReadVideoInfo->open(QFile::OpenModeFlag::ReadOnly)) {

            const auto arrayInfo = fileReadVideoInfo->readLine();
            const QString strCountTimePlayMs = QString(arrayInfo);
            countTimePlayMs = strCountTimePlayMs.toULong();
            fileReadVideoInfo->close();
        }
        else {
            throw;
        }
        //countTimePlayMs = myVideoWriter->lastPlay_countTimeEpochMs;
        break;
    }
    default:
        throw;
    }
}

int64 MyVideoReaderWriterPlay::CreateCurrentTimeCustomPlayMs() {

    return startSpeedTimePlayMs + qRound64(coeffSpeed*static_cast<float>(myElapsedTimer.GetElapsedMs() - startSpeedTimePlayMs));
}

//Multy:
bool MyVideoReaderWriterPlay::GetIsExternalFrames() const {
    return false;
}

void MyVideoReaderWriterPlay::SetIsExternalFrames(bool) { }

void MyVideoReaderWriterPlay::SetExternalFrame(std::shared_ptr<QImage>) { }

//---------------
void MyVideoReaderWriterPlay::SetSystemConfigVideoDevice(SystemConfigVideoDevice, bool) { }

bool MyVideoReaderWriterPlay::GetSystemConfigVideoDevice(SystemConfigVideoDevice) const { return false; }

void MyVideoReaderWriterPlay::InitSystemConfigVideoDevice() { }
//---------------

void MyVideoReaderWriterPlay::CloseWork() {

    qDebug() << "MyVideoReaderWriterPlay::CloseWork!";

    if(timerFrame->isActive()) {
        timerFrame->stop();
    }

    Close();
}

template<class T>
void MyVideoReaderWriterPlay::WriteFrame(std::shared_ptr<T> ptrFrame) {
    myVideoWriter->NewFrame(ptrFrame);
}
template void MyVideoReaderWriterPlay::WriteFrame<QImage>(std::shared_ptr<QImage> value);
template void MyVideoReaderWriterPlay::WriteFrame<cv::Mat>(std::shared_ptr<cv::Mat> value);

MyVideoWriter& MyVideoReaderWriterPlay::GetMyVideoWriter() {
    return *myVideoWriter;
}

MyVideoReaderWriterPlay::~MyVideoReaderWriterPlay() {

    qDebug() << "MyVideoReaderWriterPlay::~MyVideoReaderWriter!";

    auto threadReader = this->thread();
    if(QThread::currentThread() != threadReader) {
        threadReader->quit();//даем команду на остановку!
        threadReader->wait();//ждем завершения!
    }
}
