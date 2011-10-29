#ifndef CONTACTINFO_H
#define CONTACTINFO_H

#include <QDialog>

namespace Ui {
    class ContactInfo;
}

class ContactInfo : public QDialog
{
    Q_OBJECT

public:
    explicit ContactInfo(QWidget *parent=0);
    ~ContactInfo();

private:
    Ui::ContactInfo *ui;

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void recieveInfo(QString,QString);

signals:
    void sendInfo(QString,QString);

};

#endif // CONTACTINFO_H
