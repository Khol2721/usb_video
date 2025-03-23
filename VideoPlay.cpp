#include "VideoPlay.h"

//MyListWidgetItem!---------------------------------------------------------------------------

VideoPlay::MyListWidgetItem::MyListWidgetItem(VideoTile* tile, QListWidget* listview, int type) :
    QListWidgetItem(listview, type), videoTile(tile) { }

VideoPlay::MyListWidgetItem::MyListWidgetItem(VideoTile* tile, const QString& text, QListWidget* listview, int type) :
    QListWidgetItem(text, listview, type), videoTile(tile) { }

VideoPlay::MyListWidgetItem::MyListWidgetItem(VideoTile* tile, const QIcon& icon, const QString& text, QListWidget* listview, int type) :
    QListWidgetItem(icon, text, listview, type), videoTile(tile) { }

VideoPlay::MyListWidgetItem::MyListWidgetItem(VideoTile* tile, const QListWidgetItem& other) : QListWidgetItem(other), videoTile(tile)  { }

const VideoTile& VideoPlay::MyListWidgetItem::GetVideoTile() const {
    return *videoTile;
}

bool VideoPlay::MyListWidgetItem::operator < (const QListWidgetItem& item) const {

    // В дате год должен быть ОБЯЗАТЕЛЬНО четырехзначным!
    const auto& thisDateTime = dynamic_cast<const MyListWidgetItem&>(*this).GetVideoTile().GetDateTimeFile();
    const auto& itemDateTime = dynamic_cast<const MyListWidgetItem&>(item).GetVideoTile().GetDateTimeFile();

    if(thisDateTime < itemDateTime) {
        return true;
    }
    else {
        return false;
    }
}

VideoPlay::MyListWidgetItem::~MyListWidgetItem() {
    delete videoTile;
}

//VideoPlay!---------------------------------------------------------------------------

VideoPlay::VideoPlay(QWidget* parent) :
    QFrame(parent),
    nameVideoFile(""),
    templateStrDateTimeItem("dd.MM.yyyy   hh:mm:ss"),
    templateStrDateTimeDialog("dd.MM.yyyy   hh:mm:ss"),
    countIconPlayOnLine(4)
{
    const auto nameClass = QString(typeid(this).name());

    //Инициализация текстовых данных (tr тут обязателен, иначе переводчик не видит):
    videoPlayTexts.textLabNameFrame = tr("Выбор файла для воспроизведения");//tr!
    videoPlayTexts.textButPlay = tr("Воспроизведение");//tr!
    videoPlayTexts.textButDelete = tr("Стереть");//tr!
    videoPlayTexts.textMyDialog = tr("Вы действительно хотите удалить запись?");//tr!
    videoPlayTexts.textButSelection = tr("Выделение");//tr!
    videoPlayTexts.textMyDialogSelection = tr("Вы действительно хотите удалить выделенные записи?");//tr!

    //Стили и размеры:
    const QString strColorBackground { "darkgreen" };//darkgreen //green
    const QString styleClass = "#" + nameClass + " { background: " + strColorBackground + "; color: white }"
                                                /*".QListWidget::item:selected {"
                                                    "background-color: blue;"
                                                    "color: yellow;"
                                                "}"*/
                                                ".QListWidget::item {"
                                                    "color: white;"
                                                    //"border-style: solid;"
                                                    //"border-width:1px;"
                                                    //"border-color:black;"
                                                    //"background-color: green;"
                                                "}";

    const QString nameScroll = "frame" + nameClass;
    const QString styleScroll = "#" + nameScroll + " { background: " + strColorBackground + "; }";

    const QString nameListPlay = "listPlay" + nameClass;
    const QString styleListPlay = "#" + nameListPlay + " { background: " + strColorBackground + "; }";

    const QString nameBut = "but" + nameClass;
    const QString styleBut = "#" + nameBut + ":pressed {border: 4px solid #00A000;\
            background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0.1, stop:0 #ebdb9d, stop:1 #a89a63);\
            font-size:21px; background: #c9bc85; border-radius:10px;}\
            #" + nameBut + " {border: 4px solid black; \
            background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0.1, stop:0 #a89a63, stop:1 #ebdb9d);\
            border-radius:10px; font-size:21px;}\
            #" + nameBut + ":disabled {border: 4px solid black; \
            background-color: grey;\
            border-radius:10px; font-size:21px;}\
            #" + nameBut + ":checked {border: 4px solid #00A000;\
            background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0.1, stop:0 #ebdb9d, stop:1 #a89a63);\
            font-size:21px; background: #c9bc85; border-radius:10px;}";

    const QString nameLabName = "labName" + nameClass;
    const QString styleLabName = "#" + nameLabName + "{border: 2px solid white; font: 26px; color: yellow;}";

    const QString strStyleScrollBar = "QScrollBar:vertical {background-color: gray; width: 30px}";

    const QSize sizeBut { 200, 65 };

    ///Инициализация UI:
    this->setObjectName(nameClass);

#ifdef MY_NO_WINDOW_FLAGS
#else
    QWidget::setWindowFlags(Qt::Window | Qt::X11BypassWindowManagerHint); //Устанавливаем окно (Window) без рамок (FramelessWindowHint)
    //X11BypassWindowManagerHint - более универсально, т.к. убирает окна даже на системе ЮП!
#endif

    this->setStyleSheet(styleClass);

    labNameFrame = new QLabel(videoPlayTexts.textLabNameFrame);
    labNameFrame->setObjectName(nameLabName);
    labNameFrame->setStyleSheet(styleLabName);
    labNameFrame->setAlignment(Qt::AlignCenter);

    /*auto framePlay = new QFrame;
    framePlay->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);*/

    //http://itnotesblog.ru/note.php?id=187
    listPlay = new QListWidget;
    listPlay->setObjectName(nameListPlay);
    listPlay->setStyleSheet(styleListPlay);
    //listPlay->setIconSize(sizeIconPlay);//сразу для всех!
    listPlay->setViewMode(QListWidget::IconMode);//тип горизонтального отображения!
    //listPlay->setSelectionMode(QListWidget::MultiSelection);//включить множественное выделение!
    //Благодаря этому можно реализовать выделение объектов для последующего их удаления!
    listPlay->setAcceptDrops(false);//запрещаем перетаскивание элементов! (Drag and Drop)
    listPlay->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listPlay->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    connect(listPlay, &QListWidget::clicked, this, &VideoPlay::SelectItem);

    //shortcutPlay = new QShortcut(QKeySequence(Qt::Key_Delete), listPlay);
    //connect(shortcutPlay, SIGNAL(activated()), listPlay, SLOT(onDeleteItems()));

    auto scrollBar  = new QScrollBar;
    scrollBar->setStyleSheet(strStyleScrollBar);

    listPlay->setVerticalScrollBar(scrollBar);

    butPlay = new QPushButton(videoPlayTexts.textButPlay);
    connect(butPlay, &QPushButton::clicked, this, &VideoPlay::Play);//сигнатура может не совпадать!

    butDelete = new QPushButton(videoPlayTexts.textButDelete);
    connect(butDelete, &QPushButton::clicked, this, &VideoPlay::Delete);//сигнатура может не совпадать!

    butSelection = new QPushButton(videoPlayTexts.textButSelection);
    connect(butSelection, &QPushButton::clicked, this, &VideoPlay::Selection);//сигнатура может не совпадать!
    butSelection->setCheckable(true);

    QList<QPushButton*> listBut { butPlay, butDelete, butSelection };
    for(auto but : listBut) {
        but->setObjectName(nameBut);
        but->setStyleSheet(styleBut);
        but->setMaximumSize(sizeBut);
        but->setMinimumSize(sizeBut);
        but->setEnabled(false);
    }

    auto hBoxButs = new QHBoxLayout;
    hBoxButs->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    hBoxButs->addWidget(butPlay);
    hBoxButs->addWidget(butDelete);
    hBoxButs->addWidget(butSelection);

    auto vBoxBase = new QVBoxLayout(this);
    vBoxBase->addWidget(labNameFrame);
    vBoxBase->addWidget(listPlay);
    vBoxBase->addLayout(hBoxButs);

    myDialog = new MyMessageBox(QMessageBox::Icon::Question, videoPlayTexts.textMyDialog, this);
    myDialog->SetTextAlignment(Qt::AlignCenter);
    myDialog->hide();

    butSelection->setVisible(false);//лишняя кнопка? К тому же нет в документации! // false
}


void VideoPlay::changeEvent(QEvent* event)
{
    //Событие изменения языка текста!
    if (event->type() == QEvent::LanguageChange) {

        //Работает, но нужно файл перевода кропотливо редактировать!
        //Обязательно требуется в Инструменты/Внешние/Linguist/Создать переводы, затем пересобрать проект!
        //!Используем tr как при инициализации (в данном примере при инициализации videoPlayTexts), так и в событии changeEvent!
        //tr!
        labNameFrame->setText(tr(videoPlayTexts.GetConstCharPtr(videoPlayTexts.textLabNameFrame)));
        butPlay->setText(tr(videoPlayTexts.GetConstCharPtr(videoPlayTexts.textButPlay)));
        butDelete->setText(tr(videoPlayTexts.GetConstCharPtr(videoPlayTexts.textButDelete)));
        butSelection->setText(tr(videoPlayTexts.GetConstCharPtr(videoPlayTexts.textButSelection)));

//        labNameFrame->setText(videoPlayTexts.textLabNameFrame);
//        butPlay->setText(videoPlayTexts.textButPlay);
//        butDelete->setText(videoPlayTexts.textButDelete);
//        butSelection->setText(videoPlayTexts.textButSelection);

        myDialog->SetText(GetTextDialog());
    }
}

void VideoPlay::UpdateListPlay() {

    ClearListPlay();

    auto listPathPlay = MyFileInfo::GetListDirFolders(VideoTile::G_GetPathFiles());//"/mnt/video/"

    for(auto path : listPathPlay) {

        auto itemPlay = CreateItemPlay(path);

        if(itemPlay != nullptr) {
            listPlay->addItem(itemPlay);
        }
    }

    listPlay->sortItems(Qt::SortOrder::DescendingOrder);
    //DescendingOrder/AscendingOrder - убывание/возрастние!

    butSelection->setEnabled(listPathPlay.count());
}

void VideoPlay::ClearListPlay() {
    listPlay->clear();
}

void VideoPlay::SetNameFileVideo(const QString& name) {
    nameVideoFile = name;
}

void VideoPlay::Delete() {

    auto listSelectedItems = listPlay->selectedItems();

    const auto countVideo = listPlay->count();

    const auto startTextDialog = GetTextDialog();

    if(!butSelection->isChecked()) {
        for(auto item : listSelectedItems) {
            listPlay->removeItemWidget(item);

            auto textDialog = startTextDialog;

            auto myItem = dynamic_cast<MyListWidgetItem*>(item);
            const auto& videoTile = myItem->GetVideoTile();

            const auto& dateTimeFile = videoTile.GetDateTimeFile();
            textDialog += ("\n" + dateTimeFile.toString(templateStrDateTimeDialog));

            myDialog->SetText(textDialog);
            myDialog->show();

            int result = 0;
            result = myDialog->exec();

            if(result != INT_TRUE) {
                return;
            }

            const auto pathFolder = MyFileInfo::GetPathDownFolder(videoTile.GetPath());
            MyFileInfo::DeleteAllFilesAndFolders(pathFolder);

            delete item; // Qt documentation warnings you to destroy item to effectively remove it from QListWidget.
        }
    }
    else {
        myDialog->SetText(startTextDialog);
        myDialog->show();

        int result = 0;
        result = myDialog->exec();

        if(result != INT_TRUE) {
            return;
        }

        for(auto item : listSelectedItems) {
            listPlay->removeItemWidget(item);

            auto myItem = dynamic_cast<MyListWidgetItem*>(item);
            const auto& videoTile = myItem->GetVideoTile();

            const auto pathFolder = MyFileInfo::GetPathDownFolder(videoTile.GetPath());
            MyFileInfo::DeleteAllFilesAndFolders(pathFolder);

            delete item; // Qt documentation warnings you to destroy item to effectively remove it from QListWidget.
        }
    }

    if(countVideo == 1) {
        butDelete->setEnabled(false);
        butPlay->setEnabled(false);
    }
}

void VideoPlay::Play() {

    auto listSelectedItems = listPlay->selectedItems();

    if(listSelectedItems.count() == 0) {//необходимо запрещать нажатие зарание!
        //throw;
        if(listPlay->count() > 0) {//устраняем вылет при "слабой" фокусировке!
            listPlay->setCurrentItem(listPlay->currentItem());
            listSelectedItems = listPlay->selectedItems();
        }
        else {
            throw;
        }
    }

    else if(listSelectedItems.count() > 1) {//не должно быть больше 1!
        throw;
    }

    for(auto item : listSelectedItems) {
        listPlay->removeItemWidget(item);
    }

    const auto& videoTile = dynamic_cast<const MyListWidgetItem*>(listSelectedItems.first())->GetVideoTile();

    emit Sign_InitVideoPlay(videoTile.GetPath(), videoTile.GetDateTimeFile());
}

QListWidgetItem* VideoPlay::CreateItemPlay(const QString& pathPlay) {

    //nameVideoFile(old) = 3M2_OEPU.avi
    const QString pathPlayVideo = QString("%1%2%3").arg(pathPlay).arg(MyFileInfo::GetSeparator()).arg(nameVideoFile);

    auto videoTile = new VideoTile(pathPlayVideo, sizeIconPlay);

    if(!videoTile->GetIsValid()) {
        const auto pathFolder = MyFileInfo::GetPathDownFolder(videoTile->GetPath());
        MyFileInfo::DeleteAllFilesAndFolders(pathFolder);//Автоматически удаляем неисправную запись! (хотя она могла бы пригодиться...)
        delete videoTile;
        return nullptr;
    }

    const auto& dateTimeFile = videoTile->GetDateTimeFile();

    //Duration!!!!!!!!--------------
    QTime timeDuration(0, 0, 0, 0);
    timeDuration = timeDuration.addMSecs(videoTile->GetDurationMs());
    const QString strDuration = QString("\n%2: %1").arg(timeDuration.toString()).arg(tr("длительность"));
    //-------------

    const auto textItemPlay = dateTimeFile.toString(templateStrDateTimeItem) + strDuration;

    auto itemPlay = new VideoPlay::MyListWidgetItem(videoTile, textItemPlay);
    itemPlay->setIcon(videoTile->GetIcon());
    //itemPlay->setFlags(Qt::ItemIsEditable);//выключаем возможность выделения!

    return itemPlay;
}

void VideoPlay::CheckEnablePlayButtons() {

    const bool isEnable = listPlay->selectedItems().count() != 0;//listPlay->count()

    butDelete->setEnabled(isEnable);
    butPlay->setEnabled(isEnable);
}

//https://issue.life/questions/52297273
void VideoPlay::resizeEvent(QResizeEvent* event) {

    QWidget::resizeEvent(event);

    /*const auto oldSize = event->oldSize();
    if(oldSize.width() >= 0 && oldSize.height() >= 0) { }*/

    //Размер тайлов меняется только сдесь!
    const auto newSize = event->size();
    sizeIconPlay = (newSize - QSize(20, 20) - (QSize(15, 15)*countIconPlayOnLine))/countIconPlayOnLine;
    listPlay->setIconSize(sizeIconPlay);
}

void VideoPlay::SelectItem() {
    CheckEnablePlayButtons();
}

void VideoPlay::Selection() {

    const auto but = (QPushButton*)sender();

    if(but->isChecked()) {
        listPlay->setSelectionMode(QListWidget::MultiSelection);
    }
    else {
        listPlay->setSelectionMode(QListWidget::SingleSelection);
    }
}

QString VideoPlay::GetTextDialog() const {
    //tr!
    return !butSelection->isChecked() ? tr(videoPlayTexts.GetConstCharPtr(videoPlayTexts.textMyDialog)) :
                                        tr(videoPlayTexts.GetConstCharPtr(videoPlayTexts.textMyDialogSelection));

//    return !butSelection->isChecked() ? videoPlayTexts.textMyDialog :
//                                            videoPlayTexts.textMyDialogSelection;
}

void VideoPlay::setVisible(bool in) {
    if(!in) {
        butDelete->setEnabled(false);
        butPlay->setEnabled(false);
    }

    QFrame::setVisible(in);
}

VideoPlay::~VideoPlay() {

    delete labNameFrame;
    delete butDelete;
    delete butPlay;
    delete myDialog;

    ClearListPlay();
    delete listPlay;
}
