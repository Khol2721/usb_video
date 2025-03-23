#ifndef MYMASSAGEBOX_H
#define MYMASSAGEBOX_H

#include <QDialog>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEvent>

#define INT_TRUE 1

class MyMessageBox : public QDialog
{
Q_OBJECT

public:
    MyMessageBox(QMessageBox::Icon icon, const QString& text, QWidget *parent = nullptr);
    virtual ~MyMessageBox();

    void SetText(const QString& text);
    QString GetText() const;

    void SetTextAlignment(Qt::AlignmentFlag alignment);

    void changeEvent(QEvent* event) override;
    //virtual void resizeEvent(QResizeEvent* event);

private:
    QPushButton* pButtonYes;
    QPushButton* pButtonNo;
    QLabel* p_LabText;
    QLabel* p_LabIcon;
    static QString strStyleBut;

private slots:
    void onOKButtonClicked() { this->setResult(QDialog::Accepted); }
    void onCancelButtonClicked() { this->setResult(QDialog::Rejected); }
};

#endif // MYMASSAGEBOX_H
