#include "WindowGetRight.h"

using namespace std;
WindowGetRight::WindowGetRight(QSize fixedSize) {
    this->setWindowTitle("Get right");
    this->setFixedSize(fixedSize);
    int width = this->frameGeometry().width();
    int height = this->frameGeometry().height();
    QDesktopWidget wid;
    int screenWidth = wid.width();
    int screenHeight = wid.height();
    this->setGeometry((screenWidth/2)-(width/2),(screenHeight/2)-(height/2),width,height);
    AddBtnAndTb();
}


void WindowGetRight::AddBtnAndTb()
{
    QWidget* widget = new QWidget(this);
    widget->setFixedSize(130, this->height());

    QWidget* widget2 = new QWidget(this);
    widget2->setFixedSize(this->width() - 130, this->height());
    widget2->setGeometry(140, 0, widget2->width(), widget2->height());

    btn = new QPushButton("Get right", widget);
    btn->setFixedSize(QSize(120, 30));
    connect(btn, &QPushButton::released, this, &WindowGetRight::handlerGetRight);

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
    grid->addWidget(labelPasswd, 5, 0);
    grid->addWidget(passwd, 6, 0);
    grid->addWidget(okBtn, 7, 0);
    widget->show();
    text = new QTextEdit(widget2);
    text->setFixedSize(widget2->size());
    widget2->show();
}

void WindowGetRight::GetRight(int uid, const char *filename, right_t rights) {
    QMessageBox msgBox;
    stringstream right_in_stream;
    try
    {
        right_t r = sec_check(uid, filename, rights);
        right_in_stream << r;
        text->setText(right_in_stream.str().c_str());
    }
    catch (const std::exception& ex)
    {
        msgBox.setText(QString("Error: ") + QString(ex.what()));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }
    okBtn->setEnabled(false);
    passwd->setReadOnly(true);
    labelPasswd->setText("");
}

void WindowGetRight::handlerGetRight()
{
    int uid = std::stoi(name->toPlainText().toStdString());
    string filename = file->toPlainText().toStdString();
    if (passwd_exists(uid, filename.c_str())){
        okBtn->setEnabled(true);
        passwd->setReadOnly(false);
        labelPasswd->setText("Enter password");
        connect(okBtn, &QPushButton::released, this, &WindowGetRight::passwordHandler);
    }
    else{
        GetRight(uid, filename.c_str(), R_NONE);
    }
}

void WindowGetRight::passwordHandler(){
    QMessageBox msgBox;
    int uid = std::stoi(name->toPlainText().toStdString());
    string filename = file->toPlainText().toStdString();
    string password = std::to_string(std::hash<std::string>{}(passwd->text().toStdString()));
    if(authorization_by_passwd(uid, filename.c_str(), password.c_str())){
        GetRight(uid, filename.c_str(), R_NONE);
    }
    else{
        text->setText(QString(""));
        msgBox.setText(QString("Error"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }
}