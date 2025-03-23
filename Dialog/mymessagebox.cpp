#include "mymessagebox.h"

QString MyMessageBox::strStyleBut = nullptr;


MyMessageBox::MyMessageBox(QMessageBox::Icon icon, const QString& text, QWidget* parent) : QDialog(parent)
{

#ifdef MY_NO_WINDOW_FLAGS
#else
    QWidget::setWindowFlags(Qt::Window | Qt::X11BypassWindowManagerHint); //Устанавливаем окно (Window) без рамок (FramelessWindowHint)
    //X11BypassWindowManagerHint - более универсально, т.к. убирает окна даже на системе ЮП!
#endif

    ushort pixFont = 17;
    QString strPixFont = QString::number(pixFont);

    strStyleBut =  QString("#tech:pressed {border: 3px solid black; font-size:" + strPixFont + "px; border-radius:10px; background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0.1, stop:0 #000080, stop:1 #000040);color: white;}"
                           "#tech         {border: 3px solid black;border-radius:10px; font-size:" + strPixFont + "px;  background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0.1, stop:0 #00A0FF, stop:1 #0000E0);color: white;}"
                           "#tech:disabled {border: 3px solid black; font-size:" + strPixFont + "px; border-radius:10px; background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0.1, stop:0 #808080, stop:1 #404040);color: white;}"
                           "#tech:checked {border: 3px solid black; font-size:" + strPixFont + "px; border-radius:10px; background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:0, y2:0.1, stop:0 #000080, stop:1 #000040);color: white;}");

    this->setObjectName("MyMessageBox");
    this->setStyleSheet("#MyMessageBox {background-color: #00F0F0;}");

    p_LabText = new QLabel(text);
    p_LabText->setObjectName("LabText");
    p_LabText->setStyleSheet("#LabText {font: 20px; color: black;}");
    //{font: 24px; color: black;}//{font: " + strPixFont + "px; color: black;}

    p_LabIcon = new QLabel();

    pButtonYes = new QPushButton(tr("ДА"));
    pButtonYes->setMinimumHeight(60);
    pButtonYes->setObjectName("tech");
    pButtonYes->setStyleSheet(strStyleBut);
    pButtonYes->setMaximumWidth(150);
    pButtonYes->setMinimumWidth(125);

    pButtonNo = new QPushButton(tr("НЕТ"));
    pButtonNo->setMinimumHeight(60);
    pButtonNo->setObjectName("tech");
    pButtonNo->setStyleSheet(strStyleBut);
    pButtonNo->setMaximumWidth(150);
    pButtonNo->setMinimumWidth(125);

    QVBoxLayout* VBox = new QVBoxLayout;//(this)
    VBox->setAlignment(Qt::AlignCenter);

    QHBoxLayout* HBox1 = new QHBoxLayout;
    HBox1->setAlignment(Qt::AlignCenter);

    QHBoxLayout* HBox2 = new QHBoxLayout;
    HBox2->setAlignment(Qt::AlignCenter);
    HBox2->setSpacing(p_LabText->size().width()/5);

    QIcon qicon;
    QPixmap pix;

    //:/IMAGE/req60.png
    //:/IMAGE/ask55_1.png
    //:/IMAGE/ask55_2.png

    switch (icon) {
    case QMessageBox::Icon::Question:
        qicon = QIcon(":/icon/ask55_1.png");
        pix = qicon.pixmap(QSize(60,60));
        p_LabIcon->setPixmap(pix);
        HBox1->addWidget(p_LabIcon);
        HBox1->addWidget(p_LabText);
        break;
    case QMessageBox::Icon::NoIcon:
        HBox1->addWidget(p_LabText);
        break;
    default:
        break;
    }

    HBox2->addWidget(pButtonYes);
    HBox2->addWidget(pButtonNo);

    VBox->addLayout(HBox1);
    VBox->addLayout(HBox2);

    QFrame* frameBoard = new QFrame(this);
    /*this->setObjectName("frameBoard");//не работает!
    this->setStyleSheet("#frameBoard { border: 2px solid white; background: #00F0F0}");*/
    frameBoard->setFrameStyle(QFrame::Box | QFrame::Plain);
    frameBoard->setLineWidth(2);
    frameBoard->setLayout(VBox);

    QVBoxLayout* VBoxBase = new QVBoxLayout(this);
    VBoxBase->setMargin(0);
    VBoxBase->setAlignment(Qt::AlignCenter);
    VBoxBase->addWidget(frameBoard);

    QObject::connect(pButtonYes, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(pButtonNo, SIGNAL(clicked()), this, SLOT(reject()));
}

void MyMessageBox::SetText(const QString& text) {
    p_LabText->setText(text);
}

QString MyMessageBox::GetText() const {
    return p_LabText->text();
}

void MyMessageBox::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        pButtonNo->setText(tr("НЕТ"));
        pButtonYes->setText(tr("ДА"));
    }
}

void MyMessageBox::SetTextAlignment(Qt::AlignmentFlag alignment) {
    p_LabText->setAlignment(alignment);
}

MyMessageBox::~MyMessageBox()
{
    delete pButtonYes;
    delete pButtonNo;
    delete p_LabText;
    delete p_LabIcon;
}
