#include "MyVideoReaderWriter.h"

MyVideoReaderWriter::MyVideoReaderWriter(
        TypeWorkWriter typeWorkWriter,
        IScreenMonitors* iScreenMonitors,
        FunConvertMat funConvertMat,
        FunConvertImage funConvertImage,
        QSize sizeFrame,
        StateWork stateWork,
        uint8_t defFps) : QObject(),
    startStateWork(stateWork),

    #if CV_MAJOR_VERSION >= 3
    apiPreference(CAP_ANY),//0 = CAP_ANY
    //для воспроизведения видеофайла CAP_FFMPEG!
    //для воспроизведения видеоустройства CAP_V4L2!
    #endif
    typeWorkWriter(typeWorkWriter),
    iScreenMonitors(iScreenMonitors),
    defFps(defFps)
{
    qDebug() << "MyVideoReaderWriter -> MyVideoReaderWriter: " << QThread::currentThread();

    qRegisterMetaType<std::shared_ptr<QImage>>();

    myVideoWriter = new MyVideoWriter(typeWorkWriter);

    isExternalFrames = false;
    ptrExternalFrame = nullptr;

    this->stateWork = stateWork;

    videoCapture = nullptr;

    isInit = false;

    isRun = false;
    iNotFrame = 0;
    isConnect = false;

    pathFilePlay = "";
    countTimePlayMs = 0;
    actualFps = defFps;

    this->funConvertMat = funConvertMat;
    this->funConvertImage = funConvertImage;

    SetSizeFrame(sizeFrame);

    QThread* thread = new QThread;

    timerFrame = new QTimer();
    timerConnect = new QTimer();

    //! В этом случае таймеры и класс родитель выполняются в одном вторичном потоке!
    //! Иначе при moveToThread только this таймер проинициализировался бы в своем (самим собой созданном) вторичном потоке, что НЕВЕРНО!
    timerConnect->moveToThread(thread);
    timerFrame->moveToThread(thread);
    this->moveToThread(thread);

    timerFrame->setTimerType(Qt::TimerType::PreciseTimer);//CoarseTimer еще медленнее на 5-10% (что мало заметно),
    //однако задержка вся на обработке видео в read! //PreciseTimer!

    SetCoeffSpeedPlay();

    switch (stateWork) {
    case StateWork::WorkEmpty:
        InitTimeoutTimerFrame();
        break;
    default:
        break;
    }

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

void MyVideoReaderWriter::ReadFrame() {

    //qDebug() << "MyVideoReader -> ReadFrame: " << QThread::currentThread();

    /*if(!GetIsWork()) {
        return;
    }*/

    auto ptrImage = GetImage();

    //qDebug() << "Width:" << GetWidth();
    //qDebug() << "Height:" << GetHeight();

    switch (stateWork) {
        case StateWork::WorkPlay:
        //if(qRound(videoCapture->get(CAP_PROP_POS_AVI_RATIO)) == 1) {//НЕ работает!!!

        /* save img!
        //ptrImage->save("TEST3.png");
        //throw;
        //qDebug() << ptrImage.get()->isNull();*/

        if(ptrImage == nullptr) {
            ClosePlay();
            emit Sign_ClosePlay();
            return;
        }
        emit NewFrame(ptrImage);
        break;
    case StateWork::WorkVideoDevice:
        emit NewFrame(ptrImage);
        break;
    default:
        break;
    }
}

MyPtrImage MyVideoReaderWriter::GetImage() {

    #ifdef IS_SHOW_DEL_TIME_READ_FRAME
    const auto currentEpochMs = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "delTimeMs = " << currentEpochMs - previousTimeEpochMs;
    previousTimeEpochMs = currentEpochMs;
    #endif

    if(!isExternalFrames || (isExternalFrames && stateWork == StateWork::WorkPlay)) {

        if(!videoCapture->isOpened()) {
            qDebug() << "!!!!!!!!!NOT isOpened!!!!!!!!" << this;
            return nullptr;
        }

        int delFrame = 1;

        if(GetIsPlay() && typeWorkWriter == TypeWorkWriter::Custom) {

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

        bool isFrame = false;

        //const auto preTime = QDateTime::currentMSecsSinceEpoch();

        cv::Mat mat;
        for(int i = 0; i < delFrame; i++) {
            isFrame = videoCapture->read(mat);
            //qDebug() << "videoCapture->read!!!!";
        }
        //qDebug() << "Time = " << QDateTime::currentMSecsSinceEpoch() - preTime;

        // TEST ------------------------------
        /*static bool isTest = true;
        if(isTest) {
            qDebug() << "Type = " << mat.type();
            //mat.convertTo(mat, CV_8UC2);
            //cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
            //cv::imwrite("TEST.png", mat);

            //QFile file("frame.bin");
            //file.write((const char*)mat.data, mat.rows*mat.cols);

            isTest = false;
        }*/
        //--------------------------------------

        //Альтернатива read!
        /*const auto isFrame = videoCapture->grab();
        if(isFrame) {
            videoCapture->retrieve(mat);
        }*/

        #ifdef IS_TIMER_TEST
            qDebug() << "TimeoutReadFrame =" << elTimerTest.elapsed();
            elTimerTest.restart();
        #endif

        if(isCheckFileDevice) {
            //Не работает с цифровой (ждет 10 сек, хотя это можно подправить при компиляции...) (показывает что устройство есть), но работает с аналоговой!
            if(videoId.CheckPath()) {
                MyFileInfo myFileInfo;
                if(myFileInfo.SearchFile(videoId.GetPath()) != FileStatus::TrueFile) {
                    Close();
                    qDebug() << videoId.GetPath() + " not find!";
                    return nullptr;
                }
            }
        }

        if(!isFrame || mat.empty()) {

            /*if(!isFrame) {
                return saveImage;
            }*/

            //Неприятное замечание, если отключить устройство и в течение 10 сек подключить,
            //то восстановление не сработает! (поскольку видео-файл получает индекс +1)
            //Освобождение ресурсов "release" не поможет тут!
            iNotFrame++;

            //qDebug() << "ERROR!!!!!!!!!!!!!!";

            //! По причине того, что API может быть разное, использовать только API ffmpeg не совсем корректно
            //! (где можно установить флаги LIBAVFORMAT_INTERRUPT_OPEN_TIMEOUT_MS и LIBAVFORMAT_INTERRUPT_READ_TIMEOUT_MS)
            //! Проверка наличия файла по пути pathFileDevice не поможет, так как пока videoCapture его использует, он будет висеть!

            if(iNotFrame > MAX_I_NOT_FRAME) {
                Close();
            }

            return nullptr;
        }

        iNotFrame = 0;


        std::shared_ptr<QImage> baseImage = nullptr;

        const bool isPlay = (stateWork == StateWork::WorkPlay);


        if(funConvertMat != nullptr && !isPlay) {
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

            /* save img!
            if(stateWork == StateWork::WorkPlay) {
                //cv::imwrite("TEST.png", mat);

                //baseImage.save("TEST2.png");

                //throw;
            }*/
        }

        const bool isWorkWriteImageBeforeConvert = WriteImageBeforeConvert(baseImage);

        std::shared_ptr<QImage> image = nullptr;

        bool isConvertImage = !(!isWorkWriteImageBeforeConvert && isPlay);

        if(funConvertImage != nullptr && isConvertImage) {
            auto newImage = funConvertImage(*baseImage);
            image = std::make_shared<QImage>(newImage);
        }
        else {
            image = baseImage;
        }

        saveImage = image;
        WriteImageAfterConvert(saveImage);

        return saveImage;
    }
    else {

        if(ptrExternalFrame == nullptr) {
            return nullptr;
        }

        if(ptrExternalFrame->isNull()) {
            return nullptr;
        }

        //Записываем ДО или ПОСЛЕ конвертации, определяет внешний класс!
        WriteImageBeforeConvert(ptrExternalFrame);
        WriteImageAfterConvert(ptrExternalFrame);

        return ptrExternalFrame;
    }
}

void MyVideoReaderWriter::TimeoutFrame() {

    switch(stateWork) {
    case StateWork::WorkVideoDevice:
    case StateWork::WorkPlay: {
        if(!GetIsOpened()) {
            return;
        }

        if(!isRun) {
            return;
        }

        if(!isConnect) {
            return;
        }

        ReadFrame();
        break;
    }
    case StateWork::WorkEmpty: {
        if(myVideoWriter != nullptr && myVideoWriter->GetIsRun()) {

            //static auto timeEpochMs = QDateTime::currentMSecsSinceEpoch();
            //const auto startTimeEpochMs = QDateTime::currentMSecsSinceEpoch();

            ICvScreenMonitors* iCvScreenMonitors = dynamic_cast<ICvScreenMonitors*>(iScreenMonitors);
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

            //const auto currentTimeEpochMs = QDateTime::currentMSecsSinceEpoch();
            //qDebug() << QString("timeoutTest (%2) = %1").arg(currentTimeEpochMs - startTimeEpochMs).arg((long)(QThread::currentThread()));
            //qDebug() << QString("timeoutTest (%2) = %1").arg(currentTimeEpochMs - timeEpochMs).arg(timerFrame->interval());
            //timeEpochMs = currentTimeEpochMs;
            //- startTimeEpochMs //- timeEpochMs!
        }
        break;
    }
    default:
        break;
    }
}

void MyVideoReaderWriter::TimeoutConnect() {

    if(GetIsOpened()) {
        return;
    }

    //При таком поведении горячее подключение не имеет смысла!
    if(stateWork != StateWork::WorkVideoDevice) {
        return;
    }

    if(!isRun) {
        return;
    }

    if(isConnect) {
        return;
    }

    if(videoCapture != nullptr) {
        Close();
    }

    MyFileInfo myFileInfo;

    //qDebug() << "TimeoutConnect!";
    if(videoId.CheckPath()) {
        const auto& path = videoId.GetPath();
        switch (myFileInfo.SearchFile(path)) {
        case FileStatus::TrueFile:
            if(!isConnect) {
                Init();
            }
            break;
        /*case FileStatus::NotPath:
        case FileStatus::NotFile:
            videoCapture.release();
            isConnect = false;
            break;*/
        default:
            break;
        }
    }
    else {
        Init();
    }
}

void MyVideoReaderWriter::Start() {
    isRun = true;
    myElapsedTimer.SetPause(false);

    if(!isInit) {
        switch (stateWork) {
        case StateWork::WorkVideoDevice:
            Init();
            break;
        case StateWork::WorkEmpty:
        if(!timerFrame->isActive()) {//Нам нужен всегда работающий таймер!
            timerFrame->start();//$$$
        }
        return;
        default:
            return;
        }

        isInit = true;
    }
}

void MyVideoReaderWriter::Stop() {
    isRun = false;
    myElapsedTimer.SetPause(true);
}

bool MyVideoReaderWriter::Init() {

    qDebug() << "MyVideoReaderWriter -> Init: " << QThread::currentThread();

    //qDebug() << "Init №1";
    if(videoCapture != nullptr) {
        Close();
    }

    //qDebug() << "Init №1.5";

    switch (stateWork) {
#if CV_MAJOR_VERSION < 3
    case StateWork::WorkVideoDevice: {
        InitSystemConfigVideoDevice();
        if(videoId.GetIsInitNum()) {
            videoCapture = new VideoCapture(videoId.GetNum());
        }
        else {
            videoCapture = new VideoCapture(videoId.GetPath().toStdString());
        }
        break;
        }
    case StateWork::WorkPlay: {
        if(!(pathFilePlay.isNull() || pathFilePlay.isEmpty())) {
            videoCapture = new VideoCapture(pathFilePlay.toStdString());
        }
        else {
            return false;
        }
        break;
        }

#elif 3 <= CV_MAJOR_VERSION && CV_MAJOR_VERSION < 4
    case StateWork::WorkVideoDevice: {
        InitSystemConfigVideoDevice();
        if(videoId.GetIsInitNum()) {
            videoCapture = new VideoCapture(videoId.GetNum());
        }
        else {
            qDebug() << videoId.GetPath();
            videoCapture = new VideoCapture(videoId.GetPath().toStdString(), apiPreference);
        }
        break;
        }
    case StateWork::WorkPlay: {
        if(!(pathFilePlay.isNull() || pathFilePlay.isEmpty())) {
            videoCapture = new VideoCapture(pathFilePlay.toStdString(), CAP_FFMPEG);
        }
        else {
            return false;
        }
        break;
        }
#else
    case StateWork::WorkVideoDevice: {
        InitSystemConfigVideoDevice();
        if(videoId.GetIsInitNum()) {
            videoCapture = new VideoCapture(videoId.GetNum(), apiPreference);
            /*qDebug() << "CAP_PROP_BUFFERSIZE = " << videoCapture->get(cv::CAP_PROP_BUFFERSIZE);//4 по умолчанию!
            const auto isSet_CAP_PROP_BUFFERSIZE = videoCapture->set(cv::CAP_PROP_BUFFERSIZE, 1);
            qDebug() << "isSet_CAP_PROP_BUFFERSIZE = " << isSet_CAP_PROP_BUFFERSIZE;

            const auto fourcc = videoCapture->get(cv::CAP_PROP_FOURCC);//Y (YUYV) по умолчанию!
            qDebug() << "FOURCC = " << (char)((&fourcc)[0]);
            const auto isSet_FOURCC = videoCapture->set(CAP_PROP_FOURCC, MY_CV_FOURCC('U', 'Y', 'V', 'Y'));
            //('R', 'G', 'B', 'P') -> сложный формат 565, невозможно стандартными способами конвертировать в 888!
            //('U', 'Y', 'V', 'Y') -> неплох, т.к. ~ меньше требует времени на чтение!
            qDebug() << "isSet_FOURCC = " << isSet_FOURCC;
            const auto fourccNew = videoCapture->get(cv::CAP_PROP_FOURCC);
            qDebug() << "FOURCC NEW = " << (char)((&fourccNew)[0]);*/
        }
        else {
            videoCapture = new VideoCapture(videoId.GetPath().toStdString(), apiPreference);
        }
        break;
        }
    case StateWork::WorkPlay: {
        if(!(pathFilePlay.isNull() || pathFilePlay.isEmpty())) {
            videoCapture = new VideoCapture(pathFilePlay.toStdString(), CAP_FFMPEG);
        }
        else {
            return false;
        }
        break;
        }
#endif
    case StateWork::WorkEmpty:
        return false;
    default:
        return false;
    }

    //qDebug() << "Init №2";

    //Поздняя инициализация SizeFrame!
    MyVideoReaderWriter::SetSizeFrame(sizeFrame);
    //!Важно! Если не указывать SetSizeFrame::, то произойдет обратный вызов (вызов метода наследника)!

    //qDebug() << "Init №3";

    //! Этого делать не нужно, т.к. при инициализации videoCapture устройство открывается автоматически!
    //if(!GetIsOpened()) {
        //videoCapture->open(path.toStdString());//, cv::CAP_XIAPI) cv::CAP_FFMPEG) ->v
    //-> устанавливаем API в качестве бекенда! (для веб-камеры используется cv::CAP_VFW) По умолчанию стоит автоопределение API!
    //https://vk.com/away.php?to=https%3A%2F%2Fvovkos.github.io%2Fdoxyrest-showcase%2Fopencv%2Fsphinx_rtd_theme%2Fenum_cv_VideoCaptureAPIs.html
    //Выдаст сообщения (если нет такого устройства): VIDEOIO(cvCreateFileCapture_Images (filename)): raised C++ exception:
    //basic_string::substr: __pos (which is 140) > this->size() (which is 0)
    //InitFps(DEF_FPS);
    //}

    //Должно запускаться независимо от IsOpenedVideoFile = false!
    if(!timerConnect->isActive()) {
        timerConnect->start();
    }

    //qDebug() << "Init №4";
    if(GetIsOpened()) {
        isConnect = true;
        //Start();// Запуск по умолчанию (при успешной инициализации)!

        SetCoeffSpeedPlay();
        InitTimeoutTimerFrame();

        #ifdef IS_SHOW_DEL_TIME_READ_FRAME
        previousTimeEpochMs = QDateTime::currentMSecsSinceEpoch();
        #endif

        if(!timerFrame->isActive()) {
            timerFrame->start();
        }

        if(stateWork == StateWork::WorkPlay) {
            InitCountTimePlayMs();
        }

        //qDebug() << "Init №5";
        return true;
    }
    else {
        return false;
    }
}

int MyVideoReaderWriter::GetWidth() const {
    return static_cast<int>(videoCapture->get(MY_CV_CAP_PROP_FRAME_WIDTH));
}

int MyVideoReaderWriter::GetHeight() const {
    return static_cast<int>(videoCapture->get(MY_CV_CAP_PROP_FRAME_HEIGHT));
}

bool MyVideoReaderWriter::SetSizeFrame(QSize size) {

    if(!size.isNull() && !size.isEmpty()) {

        //Установка разрешения, если камера его поддерживает! (флаги возврата videoCapture->set НЕ РАБОТАЮТ!!!!)
        sizeFrame = size;

        if(!GetIsOpened()) {//сработает поздняя инициализация!
            return true;
        }

        const auto videoW = GetWidth();
        if(videoW != sizeFrame.width()) {
            videoCapture->set(MY_CV_CAP_PROP_FRAME_WIDTH, sizeFrame.width());//3
        }

        //Замечание! Если увидешь вопросы (?????) при выводе qDebug(), то это кирилица, которая не читается на удаленной машине!

        #ifdef IS_WORK_RESIZE_DEVICE
        QThread::msleep(TIME_SLEEP_INIT_SIZE_MS);//увы, но ему и 5 сек мало... (в общем, не работает)
        //! Нужно время на установку! (не успевает обновить разрешение)
        const auto newVideoW = GetWidth();
        if(newVideoW != sizeFrame.width()) {
            qDebug() << "Не удалось установить разрешение изображения по ширине!";
            throw;//return false;//бред!
        }
        #endif

        const auto videoH = GetHeight();
        if(videoH != sizeFrame.height()) {
            videoCapture->set(MY_CV_CAP_PROP_FRAME_HEIGHT, sizeFrame.height());//4
        }

        #ifdef IS_WORK_RESIZE_DEVICE
        QThread::msleep(TIME_SLEEP_INIT_SIZE_MS);//увы, но ему и 5 сек мало... (в общем, не работает)
        //! Нужно время на установку! (не успевает обновить разрешение)
        const auto newVideoH = GetHeight();
        if(newVideoH != sizeFrame.height()) {
            qDebug() << "Не удалось установить разрешение изображения по высоте!";
            throw;//return false;//бред!
        }
        #endif

        qDebug() << "Resize device is completed!";

        if(!myVideoWriter->GetIsRun()) {
            myVideoWriter->SetSizeVideo(size);
        }
        return true;
    }
    else {
        return false;
    }
}

bool MyVideoReaderWriter::GetIsOpened() const {

    if(videoCapture == nullptr) {
        return false;
    }

    const bool isOpened = videoCapture->isOpened();

    /*if (!isOpened) {
        //qDebug() << "videoCapture: Не удалось открыть устройство!";
        qDebug() << "videoCapture: No open video file/device!";
    }*/

    return isOpened;
}

bool MyVideoReaderWriter::GetIsRun() const {
    return isRun;
}

int MyVideoReaderWriter::GetTimeoutFrame() {

    actualFps = GetFps();
    const auto fps = coeffSpeed*static_cast<float>(actualFps);

    //! Как показала практика, для устранения лишнего (возрастающего) ожидания при чтении кадра (->read) уровня OpenCV,
    //! необходимо ввести искусственную задержку на обработку кадра ( >= 1мс)!
    auto addDalay = 0;
    switch (stateWork) {
    case StateWork::WorkVideoDevice:
        addDalay = ADD_DELAY_TIMEOUT_READ_FRAME;
        break;
    default:
        break;
    }

    const auto timeoutFrame = qCeil((double)(1000.0f/fps)) + addDalay;

    return timeoutFrame;
}

ushort MyVideoReaderWriter::GetFps() const {

    if(!GetIsOpened()) {
        return defFps;
    }

    auto fps = static_cast<ushort>(videoCapture->get(MY_CV_CAP_PROP_FPS))*2;//IP камера выдает 25 фпс, но фактическая установлена 50, потому домножаем на 2! -> КОСТЫЛЬ!
    qDebug() << fps << defFps << "!!!!!!!!!";
    #if CV_MAJOR_VERSION < 4//КОСТЫЛЬ!
    if(stateWork == StateWork::WorkPlay) {
        return defFps;
    }
    #endif//! set(MY_CV_CAP_PROP_POS_FRAMES... работает при этом исправно!

    if(fps == 0) {
        //qDebug() << "SetFps: FPS не был установлен!";
        return defFps;
    }

    return fps;
}

uint MyVideoReaderWriter::GetCountFramePlay() const {

    auto count = static_cast<uint>(videoCapture->get(MY_CV_CAP_PROP_FRAME_COUNT));

    #if CV_MAJOR_VERSION < 4//КОСТЫЛЬ!
    if(stateWork == StateWork::WorkPlay) {
        auto fps = static_cast<float>(videoCapture->get(MY_CV_CAP_PROP_FPS));
        return static_cast<float>(count)/fps*static_cast<float>(GetFps());
    }
    #endif//! set(MY_CV_CAP_PROP_POS_FRAMES... работает при этом исправно!

    return count;
}

uint MyVideoReaderWriter::GetCurrentFramePlay() const {
    auto current = static_cast<uint>(videoCapture->get(MY_CV_CAP_PROP_POS_FRAMES));

    #if CV_MAJOR_VERSION < 4//КОСТЫЛЬ!
    if(stateWork == StateWork::WorkPlay) {
        auto fps = static_cast<float>(videoCapture->get(MY_CV_CAP_PROP_FPS));
        return static_cast<float>(current)/fps*static_cast<float>(GetFps());
    }
    #endif//! set(MY_CV_CAP_PROP_POS_FRAMES... работает при этом исправно!

    return current;
}

bool MyVideoReaderWriter::SetCurrentFramePlay(uint posFrame) {

    if(!GetIsOpened()) {
        return false;
    }

    switch (typeWorkWriter) {
    case TypeWorkWriter::Normal:
        return videoCapture->set(MY_CV_CAP_PROP_POS_FRAMES, posFrame);//в Custom реализации нельзя использовать напрямую!
    case TypeWorkWriter::Custom: { //TODO: не проверено!

        //Пропорция!
        const auto posTime = (int64)((float)currentTimePlayMs*(float)posFrame/(float)tikFrame);

        const auto delTimeMs = (int64)posTime - myElapsedTimer.GetElapsedMs();
        //qDebug() << "posTime = " <<  posTime << " delTimeMs = " << delTimeMs;
        myElapsedTimer.AddElapsedMs(delTimeMs);


        tikFrame = posFrame;
        currentTimePlayMs = posTime;
        startSpeedTimePlayMs = currentTimePlayMs;

        #if CV_MAJOR_VERSION < 4//КОСТЫЛЬ!
        auto fps = static_cast<double>(videoCapture->get(MY_CV_CAP_PROP_FPS));
        bool isComplete = videoCapture->set(MY_CV_CAP_PROP_POS_FRAMES, static_cast<double>(tikFrame)*fps/static_cast<double>(GetFps()));
        #else
        bool isComplete = videoCapture->set(MY_CV_CAP_PROP_POS_FRAMES, tikFrame);
        #endif

        return isComplete;
    }
    default:
        throw;
    }
}


void MyVideoReaderWriter::SetPathFileDevice(const QString& pathFile) {

    videoId = VideoId(pathFile);

    if(!GetIsRun()) {
        return;
    }

    switch (stateWork) {
    case StateWork::WorkVideoDevice:
        Init();
        break;
    default:
        return;
    }
}

void MyVideoReaderWriter::SetNumDevice(qint8 num) {

    videoId = VideoId(num);

    if(!GetIsRun()) {
        return;
    }

    switch (stateWork) {
    case StateWork::WorkVideoDevice:
        Init();
        break;
    default:
        return;
    }
}

void MyVideoReaderWriter::SetVideoIdDevice(const QString& path, qint8 num) {

    videoId = VideoId(path, num);

    if(!GetIsRun()) {
        return;
    }

    switch (stateWork) {
    case StateWork::WorkVideoDevice:
        Init();
        break;
    default:
        return;
    }
}

void MyVideoReaderWriter::SetIsCheckFileDevice(bool in) {
    isCheckFileDevice = in;
}

quint64 MyVideoReaderWriter::GetCountTimePlayMs() const {
    return countTimePlayMs;
}

quint64 MyVideoReaderWriter::GetCurrentTimePlayMs() const {

    if(!GetIsOpened()) {
        return 0;
    }

    return currentTimePlayMs;
    //return static_cast<quint64>(videoCapture->get(MY_CV_CAP_PROP_POS_MSEC));
}

bool MyVideoReaderWriter::SetCurrentTimePlayMs(quint64 posTime) {

    if(!GetIsOpened()) {
        return false;
    }

    switch (typeWorkWriter) {
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
        throw;
    }
}

void MyVideoReaderWriter::RestartPlay() {
    StartPlay(pathFilePlay);
}

void MyVideoReaderWriter::StartPlay(QString path) {

    qDebug() << "MyVideoReader -> StartPlay: " << QThread::currentThread();

    if(path.isNull() || path.isEmpty()) {
        return;
    }

    //Проверять по адрессам не корректно, потому проверяем посимвольно:
    //https://ravesli.com/urok-2-stroki-v-qt5/#toc-7
    if(GetIsWork() && stateWork == StateWork::WorkPlay) {
        if(QString::compare(pathFilePlay, path) == STR_EQUAL) {
            videoCapture->set(MY_CV_CAP_PROP_POS_AVI_RATIO, 0);//вернет в начало!
            //Если переинициализировать не тужно то все верно!
        }
    }

    pathFilePlay = path;
    stateWork = StateWork::WorkPlay;
    Init();

    myElapsedTimer.Stop();
    Start();
}

void MyVideoReaderWriter::ClosePlay() {

    //Выключаем воспроизведение видео-файла и переключаемся на показ видео с устройства!

    if(!GetIsPlay()) {
        return;
    }

    if(GetIsOpened()) {
        Close();
    }

    //stateWork = StateWork::WorkVideoDevice;
    stateWork = startStateWork;

    Init();
    SetIsRun(true);

    tikFrame = 0;
    currentTimePlayMs = 0;
    startSpeedTimePlayMs = 0;
}

void MyVideoReaderWriter::SetIsRun(bool tf) {
    tf ? Start() : Stop();
}

void MyVideoReaderWriter::Close() {

    if(timerFrame->isActive()) {
        timerFrame->stop();
    }

    if(!GetIsOpened()) {
        return;
    }

    qDebug() << "MyVideoReader -> Close: " << QThread::currentThread();

    //Полагаю, что освобождение занимает время, за которое может сработать таймер на чтение изображения!
    auto ptrCaptureToRelease = videoCapture;
    videoCapture = nullptr;
    ptrCaptureToRelease->release();
    delete ptrCaptureToRelease;
    isConnect = false;
    iNotFrame = 0;
}

bool MyVideoReaderWriter::GetIsPlay() const {

    if(stateWork == StateWork::WorkPlay) {
        return true;
    }
    else {
        return false;
    }
}

void MyVideoReaderWriter::SetCoeffSpeedPlay(float coeff) {

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

void MyVideoReaderWriter::InitTimeoutTimerFrame() {
    //qDebug() << "MyVideoReader -> timerFrame: " << timerFrame->thread();

    //timerFrame->stop();
    //timerFrame->moveToThread(QThread::currentThread());
    const auto interval = GetTimeoutFrame();
    timerFrame->setInterval(interval);
    //timerFrame->start();

    qDebug() << "TimerFrame interval = " << interval;
}

float MyVideoReaderWriter::GetCoeffSpeedPlay() const {
    return coeffSpeed;
}

bool MyVideoReaderWriter::GetIsWork() const {

    if(!GetIsOpened() || !isConnect) {
        return false;
    }
    else {
        return true;
    }
}

void MyVideoReaderWriter::InitCountTimePlayMs() {

    switch (typeWorkWriter) {
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

int64 MyVideoReaderWriter::CreateCurrentTimeCustomPlayMs() {

    /*if(myElapsedTimer.GetElapsedMs() < startSpeedTimePlayMs) {
        qDebug() << "Stop!";
    }*/

    return startSpeedTimePlayMs + qRound64(coeffSpeed*static_cast<float>(myElapsedTimer.GetElapsedMs() - startSpeedTimePlayMs));
}

//Multy:
bool MyVideoReaderWriter::GetIsExternalFrames() const {
    return isExternalFrames;
}

void MyVideoReaderWriter::SetIsExternalFrames(bool tf) {
    isExternalFrames = tf;
}

void MyVideoReaderWriter::SetExternalFrame(std::shared_ptr<QImage> img) {
    ptrExternalFrame = img;
}

//---------------
void MyVideoReaderWriter::SetSystemConfigVideoDevice(SystemConfigVideoDevice type, bool isWork) {
    mapSystemConfigVideoDevice[type] = isWork;
}

bool MyVideoReaderWriter::GetSystemConfigVideoDevice(SystemConfigVideoDevice type) const {
    return mapSystemConfigVideoDevice[type];
}

void MyVideoReaderWriter::InitSystemConfigVideoDevice() {
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
//---------------

void MyVideoReaderWriter::CloseWork() {

    qDebug() << "CloseWork!!!!";
    if(timerConnect->isActive()) {
        timerConnect->stop();
    }

    if(timerFrame->isActive()) {
        timerFrame->stop();
    }

    Close();
}

bool MyVideoReaderWriter::WriteImageBeforeConvert(std::shared_ptr<QImage> ptrImage) {

    if(myVideoWriter == nullptr) {
        return false;
    }

    if(!myVideoWriter->GetIsRun()) {
        return false;
    }

    if(isWriteAfterConvert) {
        return false;
    }

    WriteFrame(ptrImage);

    return true;
}

void MyVideoReaderWriter::WriteImageAfterConvert(std::shared_ptr<QImage> ptrImage) {

    if(myVideoWriter == nullptr) {
        return;
    }

    if(!myVideoWriter->GetIsRun()) {
        return;
    }

    if(!isWriteAfterConvert) {
        return;
    }

    WriteFrame(ptrImage);
}

template<class T>
void MyVideoReaderWriter::WriteFrame(std::shared_ptr<T> ptrFrame) {

    //const auto startTimeEpochMs = QDateTime::currentMSecsSinceEpoch();
    myVideoWriter->NewFrame(ptrFrame);
    /*const auto currentTimeEpochMs = QDateTime::currentMSecsSinceEpoch();
    qDebug() << QString("timeoutTest (%2) = %1").arg(currentTimeEpochMs - startTimeEpochMs).arg((long)(QThread::currentThread()));*/
}
template void MyVideoReaderWriter::WriteFrame<QImage>(std::shared_ptr<QImage> value);
template void MyVideoReaderWriter::WriteFrame<cv::Mat>(std::shared_ptr<cv::Mat> value);

MyVideoWriter& MyVideoReaderWriter::GetMyVideoWriter() {
    return *myVideoWriter;
}

void MyVideoReaderWriter::SetIsWriteAfterConvert(bool in) {
    isWriteAfterConvert = in;
}

MyVideoReaderWriter::~MyVideoReaderWriter() {

    qDebug() << "MyVideoReaderWriter::~MyVideoReaderWriter!";

    auto threadReader = this->thread();
    if(QThread::currentThread() != threadReader) {
        threadReader->quit();//даем команду на остановку!
        threadReader->wait();//ждем завершения!
    }

    //Ответственность на освобождение данного объекта возлагаем на себя!
    delete iScreenMonitors;
}
