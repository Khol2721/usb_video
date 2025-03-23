#ifndef VIDEOPLAY_H
#define VIDEOPLAY_H

#include <QDebug>
#include <QLabel>
#include <QFrame>
#include <QEvent>
#include <QScrollBar>
#include <QScrollArea>
#include <QPushButton>
#include <QStringList>
#include <QListWidget>
#include <QResizeEvent>
//#include <QShortcut>// предназначен для привязки
//нажатия сочетаний клавиш с клавиатуры к слотам!
//(очень удобно, можно было его сделать в Сервале)

#include "Dialog/mymessagebox.h"
#include "VideoTile.h"
#include "helpers/myfileinfo.h"
#include "helpers/itranslatetext.h"

class VideoPlay : public QFrame
{
    Q_OBJECT
public:
    VideoPlay(QWidget* parent = nullptr);
    virtual ~VideoPlay();

    void UpdateListPlay();

    void SetNameFileVideo(const QString& name);//new!

    void changeEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    void setVisible(bool in) override;

private:

    class MyListWidgetItem : public QListWidgetItem
    {
    public:
        MyListWidgetItem(VideoTile*, QListWidget* listview = nullptr, int type = Type);
        MyListWidgetItem(VideoTile*, const QString& text, QListWidget* listview = nullptr, int type = Type);
        MyListWidgetItem(VideoTile*, const QIcon& icon, const QString& text, QListWidget* listview = nullptr, int type = Type);
        MyListWidgetItem(VideoTile*, const QListWidgetItem& other);
        virtual ~MyListWidgetItem();//тут контролируем очистку videoTile!

        const VideoTile& GetVideoTile() const;

        ///Переопределяем для сортировки по дате по УБЫВАНИЮ! (для удобства)
        ///Возвращает true если текст этого элемента меньше текста другого элемента, в противном случае возвращается false!
        bool operator < (const QListWidgetItem&) const override;

    private:
        VideoTile* videoTile;
    };

    QLabel* labNameFrame;//name
    QPushButton* butDelete;//delete_B
    QPushButton* butPlay;//play_B

    QPushButton* butSelection;//play_B

    MyMessageBox* myDialog;//mydialog

    QListWidget* listPlay;//QWidgetList НЕ QListWidget

    //QStringList listPathVideoFile;//file_list

    QString nameVideoFile;
    const QString templateStrDateTimeItem;
    const QString templateStrDateTimeDialog;

    struct VideoPlayTexts : public ITranslateText
    {
        QString textLabNameFrame;
        QString textButDelete;
        QString textButPlay;
        QString textMyDialog;

        QString textButSelection;
        QString textMyDialogSelection;

    } videoPlayTexts;


    ///Количество тайлов, вмещающихся в горизонтальную линию!
    const ushort countIconPlayOnLine;
    QSize sizeIconPlay;

    //QShortcut* shortcutPlay;

    void ClearListPlay();

    QListWidgetItem* CreateItemPlay(const QString& pathPlay);
    void CheckEnablePlayButtons();

    QString GetTextDialog() const;

signals:
    void Sign_InitVideoPlay(QString pathVideoFile, QDateTime dateTime);

private slots:
    void Delete();
    void Play();
    void SelectItem();
    void Selection();
};

#endif // VIDEOPLAY_H
