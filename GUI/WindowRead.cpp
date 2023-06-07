#include "WindowRead.h"

using namespace std;
WindowRead::WindowRead(QSize fixedSize) {
    this->setWindowTitle("Open file");
    this->setFixedSize(fixedSize);
    int width = this->frameGeometry().width();
    int height = this->frameGeometry().height();
    QDesktopWidget wid;
    int screenWidth = wid.width();
    int screenHeight = wid.height();
    this->setGeometry((screenWidth/2)-(width/2),(screenHeight/2)-(height/2),width,height);
    AddBtnAndTb();
}

void WindowRead::AddBtnAndTb()
{
    QWidget* widget = new QWidget(this);
    widget->setFixedSize(130, this->height());

    QWidget* widget2 = new QWidget(this);
    widget2->setFixedSize(this->width() - 130, this->height());
    widget2->setGeometry(140, 0, widget2->width(), widget2->height());

    btn = new QPushButton("Read file", widget);
    btn->setFixedSize(QSize(120, 30));
    connect(btn, &QPushButton::released, this, &WindowRead::getDataOfFile);

    setText = new QPushButton("Write text", widget);
    setText->setFixedSize(QSize(120, 30));
    connect(setText, &QPushButton::released, this, &WindowRead::setTextHandler);

    okBtn = new QPushButton("Enter", widget);
    okBtn->setFixedSize(QSize(120, 30));
    okBtn->setEnabled(false);

    QGridLayout* grid = new QGridLayout(widget);

    name = new QTextEdit(widget);
    name->setFixedSize(QSize(120, 30));

    file = new QTextEdit(widget);
    file->setFixedSize(QSize(120, 30));

    passwd = new QLineEdit(widget);
    passwd->setEchoMode(QLineEdit::Password);
    passwd->setFixedSize(QSize(120, 30));
    passwd->setReadOnly(true);

    QLabel* labelName = new QLabel("Owner",   widget);
    QLabel* labelFileName = new QLabel("File name", widget);
    labelPasswd = new QLabel("", widget);

    grid->addWidget(labelName, 0, 0);
    grid->addWidget(name, 1, 0);
    grid->addWidget(labelFileName, 2, 0);
    grid->addWidget(file, 3, 0);
    grid->addWidget(btn, 4, 0);
    grid->addWidget(setText, 5, 0);
    grid->addWidget(labelPasswd, 6, 0);
    grid->addWidget(passwd, 7, 0);
    grid->addWidget(okBtn, 8, 0);
    widget->show();
    text = new QTextEdit(widget2);
    text->setFixedSize(widget2->size());
    widget2->show();
}

void WindowRead::Read(int uid, const char *filename, mode_t mode) {
    char* short_text;
    QMessageBox msgBox;
    off_t file_size;
    try
    {
        int receivedFd = sec_openat(uid, filename, mode);
        if (receivedFd < 0)
        {
            text->setText("");
            msgBox.setText("Error open file!");
            msgBox.exec();
            throw std::invalid_argument("Error when open file!");
        }
        else
        {
            file_size = lseek(receivedFd, 0, SEEK_END);
            short_text = new char[(int)file_size+1];
            lseek(receivedFd, 0, SEEK_SET);
            if (read(receivedFd, short_text, (int)file_size) < 0)
                throw std::invalid_argument("No permission to read!");
            text->setText(short_text);
            delete short_text;
        }
    }
    catch (const std::exception& ex)
    {
        text->setText(QString(""));
        msgBox.setText(QString("Error: ") + QString(ex.what()));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }
    okBtn->setEnabled(false);
    passwd->setReadOnly(true);
    labelPasswd->setText("");
}

void WindowRead::getDataOfFile()
{
    int uid = std::stoi(name->toPlainText().toStdString());
    string filename = file->toPlainText().toStdString();
    if (passwd_exists(uid, filename.c_str())){
        okBtn->setEnabled(true);
        passwd->setReadOnly(false);
        labelPasswd->setText("Enter password");
        connect(okBtn, &QPushButton::released, this, &WindowRead::passwordReadHandler);
    }
    else{
        Read(uid, filename.c_str(), 0777);
    }
}

void WindowRead::passwordReadHandler() {
    off_t file_size;
    char* short_text;
    QMessageBox msgBox;
    int uid = std::stoi(name->toPlainText().toStdString());
    string filename = file->toPlainText().toStdString();
    string password = passwd->text().toStdString();

    if(authorization_by_passwd(uid, filename.c_str(), password.c_str())){
        Read(uid, filename.c_str(), 0777);
    }
    else{
        text->setText(QString(""));
        msgBox.setText(QString("Error"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }

}

void WindowRead::Write(int uid, const char *filename, mode_t mode) {
    QMessageBox msgBox;
    try
    {
        int receivedFd = sec_openat(uid, filename, mode);
        if (receivedFd < 0)
        {
            throw  std::invalid_argument("Error when open file");
        }
        else
        {
            string new_text = text->toPlainText().toStdString();
            if( write(receivedFd, new_text.c_str(), new_text.length()+1) < 0)
            {

                throw  std::invalid_argument("No permission to write");
            }
        }
        msgBox.setText("Successfully set text to file");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    }
    catch (const std::exception& ex)
    {
        msgBox.setText(QString("Error: ") + QString(ex.what()));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.addButton(QMessageBox::Ok);
        msgBox.exec();
    }
    okBtn->setEnabled(false);
    passwd->setReadOnly(true);
    labelPasswd->setText("");
}

void WindowRead::setTextHandler() {
    int uid = std::stoi(name->toPlainText().toStdString());
    string filename = file->toPlainText().toStdString();
    QMessageBox msgBox;
    msgBox.setText("Are you sure to write new text?");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.addButton(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);
    if (msgBox.exec() == QMessageBox::No)
        return;
    if (passwd_exists(uid, filename.c_str())){
        okBtn->setEnabled(true);
        passwd->setReadOnly(false);
        labelPasswd->setText("Enter password");
        connect(okBtn, &QPushButton::released, this, &WindowRead::passwordWriteHandler);
    }
    else{
        Write(uid, filename.c_str(), 0777);
    }
}

void WindowRead::passwordWriteHandler() {
    off_t file_size;
    char* short_text;
    QMessageBox msgBox;
    int uid = std::stoi(name->toPlainText().toStdString());
    string filename = file->toPlainText().toStdString();
    string password = passwd->text().toStdString();

    if(authorization_by_passwd(uid, filename.c_str(), password.c_str())){
        msgBox.setText("Are you sure to write new text?");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.addButton(QMessageBox::Yes);
        msgBox.addButton(QMessageBox::No);
        if (msgBox.exec() == QMessageBox::No)
            return;
        if (passwd_exists(uid, filename.c_str())){
            okBtn->setEnabled(true);
            passwd->setReadOnly(false);
            labelPasswd->setText("Enter password");
            connect(okBtn, &QPushButton::released, this, &WindowRead::passwordWriteHandler);
        }
        else{
            Write(uid, filename.c_str(), 0777);
        }
    }
    else{
        text->setText(QString(""));
        msgBox.setText(QString("Error"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }
}






