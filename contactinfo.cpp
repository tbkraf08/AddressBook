#include "contactinfo.h"
#include "ui_contactinfo.h"
#include "mainwindow.h"

ContactInfo::ContactInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactInfo)
{
    ui->setupUi(this);
}

ContactInfo::~ContactInfo()
{
    delete ui;
}

//when you click on the ok button
void ContactInfo::on_buttonBox_accepted()
{
    QString name = ui->lineEdit->text();
    QString address = ui->plainTextEdit->toPlainText();

    ui->lineEdit->setText("");
    ui->plainTextEdit->setPlainText("");

    emit sendInfo(name,address);
}

void ContactInfo::recieveInfo(QString name, QString address){
    ui->lineEdit->setText(name);
    ui->plainTextEdit->setPlainText(address);
}

void ContactInfo::on_buttonBox_rejected()
{
    ui->lineEdit->setText("");
    ui->plainTextEdit->setPlainText("");
}
