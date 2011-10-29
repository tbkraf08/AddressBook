#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "contactinfo.h"
#include <QtGui>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Super Awesome Address Book");
    unsavedChanges = false;

    contactInfo = new ContactInfo(this);
    connect(contactInfo,SIGNAL(sendInfo(QString,QString)),this,SLOT(recieveInfo(QString,QString)));
    connect(this,SIGNAL(sendInfo(QString,QString)),contactInfo,SLOT(recieveInfo(QString,QString)));

    //set everything to save in same spot
    currentFilename = "addressBook.txt";

    //load address book
    loadAddressBook(currentFilename);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

//when user clicks Edit->Add Contact
void MainWindow::on_actionAdd_triggered()
{
    contactInfo->setWindowTitle("Add Contact");
    contactInfo->show();
}

//recieves signal from contact info with user input
void MainWindow::recieveInfo(QString name, QString address){
    if(name=="" || address ==""){
        QMessageBox::information(this, tr("Empty Field"),
                                 tr("Please enter a name and address."));
        return;
    }
    if(contactInfo->windowTitle() == "Add Contact"){
        if(!contacts.contains(name)){
            contacts.insert(name, address);
            ui->listWidget->addItem(name);
            QMessageBox::information(this, tr("Add Successful"),
                                     tr("\"%1\" has been added to your address book.").arg(name));
        }else{
            QMessageBox::information(this, tr("Add Unsuccessful"),
                                     tr("Sorry, \"%1\" is already in your address book.").arg(name));
        }
    }else{
        if(oldName != name){
            if(!contacts.contains(name)){
                QMessageBox::information(this, tr("Edit Successful"),
                                         tr("\"%1\" has been edited in your address book.").arg(oldName));
                contacts.remove(oldName);
                contacts.insert(name,address);

                QListWidgetItem *item = ui->listWidget->currentItem();
                delete item;
                ui->listWidget->addItem(name);
                ui->textEdit->setText("Name:\n" + name + "\n\n" + "Address:\n" + address);


            }else{
                QMessageBox::information(this, tr("Edit Unsuccessful"),
                                         tr("Sorry, \"%1\" is already in your address book.").arg(name));
                return;
            }

        }else if (oldAddress != address) {
            QMessageBox::information(this, tr("Edit Successful"),
                                     tr("\"%1\" has been edited in your address book.").arg(name));
            contacts[name] = address;
            ui->textEdit->setText("Name:\n" + name + "\n\n" + "Address:\n" + address);
        }
    }

    unsavedChanges=true;
}


//when user clicks Edit->Edit Contact
void MainWindow::on_actionEdit_Contact_triggered()
{
    QListWidgetItem *item = ui->listWidget->currentItem();

    //if item = null dont do anything
    if(!item){
        return;
    }

    QString name = item->text();
    QString address = contacts.value(name);

    oldName=name;
    oldAddress=address;

    contactInfo->setWindowTitle("Edit Contact");
    contactInfo->show();

    emit sendInfo(name,address);
}

//when user clicks Edit->Remove Contact
void MainWindow::on_actionRemove_Contact_triggered()
{
    QListWidgetItem *item = ui->listWidget->currentItem();

    //if item = null dont do anything
    if(!item){
        return;
    }

    int button = QMessageBox::question(this,
                                       tr("Confirm Remove"),
                                       tr("Are you sure you want to remove \"%1\"?").arg(item->text()),
                                       QMessageBox::Yes | QMessageBox::No);

    if (button == QMessageBox::Yes) {
        QMessageBox::information(this, tr("Remove Successful"),
                                 tr("\"%1\" has been removed from your address book.").arg(item->text()));

        contacts.remove(item->text());
        delete item;
        ui->textEdit->setText("");
        unsavedChanges=true;
    }
}

//when user clicks File->Find Contact
void MainWindow::on_actionFind_Contact_triggered()
{
    QString name = QInputDialog::getText(this,tr("QInputDialog::getText()"),
                                         tr("Find Person:"), QLineEdit::Normal,
                                         QDir::home().dirName());

    if(name.isEmpty()){
        QMessageBox::information(this, tr("Empty Field"),
                                 tr("Please enter a name."));
        return;
    }

    if(!contacts.contains(name)){
        QMessageBox::information(this, tr("Error"),
                                 tr("Contact Not Found."));
        return;
    }
    findContact(name);

}

//finds contact in address book and changes textEdit accordingly
void MainWindow::findContact(QString name){
    QString personAddress = contacts.value(name);
    ui->textEdit->setText("Name:\n" + name + "\n\n" + "Address:\n" + personAddress);
}

//when user double clicks on item in address book
void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem* item)
{
    QString person = item->text();
    findContact(person);
}

//when user clicks File->Save
void MainWindow::on_actionSave_triggered()
{
    saveCurrentFile();
}

//write window contents to a file
//saves in such a way it allows the name and address to be mulitple lines long
void MainWindow::saveCurrentFile(){
    //get a filename if we don't have one
    if(currentFilename.isEmpty()){
        if(!setCurrentFilename(QFileDialog::AnyFile)){
            return;
        }
    }

    //try to open the file
    QFile file(currentFilename);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,"Save Error","Could not be saved!");
        return;
    }

    //write the file
    QTextStream out(&file);
    QMap<QString,QString>::const_iterator i;
    for (i = contacts.constBegin(); i != contacts.constEnd(); i++)
    {

        out << i.key() << endl;
        out << "****" << endl;

        out << i.value() << endl;
        out << "********" << endl;
    }


    //update status
    ui->statusBar->showMessage("File saved.", 2000);
    unsavedChanges = false;
}

//have the user specify a new file name
bool MainWindow::setCurrentFilename(QFileDialog::FileMode mode){
    QFileDialog dialog(this);
    dialog.setFileMode(mode);

    if (!dialog.exec()){
        return false;
    }

    QStringList filenames = dialog.selectedFiles();
    currentFilename = filenames[0];
    return true;
}

//when user clicks File->Load
void MainWindow::on_actionLoad_triggered()
{
    //dont care about saving, we are not deleting old contact book

    //set file name
    if(!setCurrentFilename(QFileDialog::ExistingFile)){
        return;
    }

    loadAddressBook(currentFilename);
}

void MainWindow::loadAddressBook(QString filename){
    //try to open the file
    QFile file(filename);
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,"Save Error","Could not be saved!");
        return;
    }

    //read the file
    QString name = "";
    QString address = "";
    QString temp = "";

    QTextStream in(&file);
    while(!in.atEnd()){
        temp = in.readLine();
        name = "";
        address = "";

        //4 stars always after name

        while (temp != " ****"){
            name = name + temp;
            temp = " " + in.readLine();
        }

        //8 stars always after address

        temp = in.readLine();
        while (temp != "\n********"){
            address = address + temp;
            temp = "\n" + in.readLine();
        }


        if (name != "" && address != "" && !contacts.contains(name)){
            ui->listWidget->addItem(name);
            contacts.insert(name,address);
        }
    }



    //update status
    ui->statusBar->showMessage("File opened.", 2000);
    unsavedChanges = true;
}
