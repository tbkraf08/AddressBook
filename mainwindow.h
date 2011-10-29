#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtGui>
#include <QMainWindow>
#include "contactinfo.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);  

private:
    Ui::MainWindow *ui;
    ContactInfo *contactInfo;
    QMap<QString, QString> contacts;
    QString currentFilename;
    void saveCurrentFile();
    bool setCurrentFilename(QFileDialog::FileMode);
    bool unsavedChanges;
    void addContacts();
    void findContact(QString);
    QString oldName;
    QString oldAddress;
    void loadAddressBook(QString);


private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem* item);
    void on_actionFind_Contact_triggered();
    void on_actionLoad_triggered();
    void on_actionSave_triggered();
    void on_actionRemove_Contact_triggered();
    void on_actionEdit_Contact_triggered();
    void on_actionAdd_triggered();
    void recieveInfo(QString,QString);

signals:
    void sendInfo(QString,QString);

};

#endif // MAINWINDOW_H
