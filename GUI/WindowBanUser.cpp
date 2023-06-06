#include "WindowBanUser.h"

void WindowBanUser::AddBtnAndTb() {
    this->setWindowTitle("Ban user");
    QWidget *widget = new QWidget(this);
    widget->setFixedSize(this->width(), this->height());

    btn = new QPushButton("Ban user", widget);
    btn->setFixedSize(QSize(120, 30));
    connect(btn, &QPushButton::released, this, &WindowBanUser::banUser);

    btn2 = new QPushButton("Unban user", widget);
    btn2->setFixedSize(QSize(120, 30));
    connect(btn2, &QPushButton::released, this, &WindowBanUser::unbanUser);

    okBtn = new QPushButton("Enter", widget);
    okBtn->setFixedSize(QSize(120, 30));
    okBtn->setEnabled(false);

    QGridLayout *grid = new QGridLayout(widget);

    user = new QTextEdit(widget);
    user->setFixedSize(QSize(120, 30));

    file = new QTextEdit(widget);
    file->setFixedSize(QSize(120, 30));

    passwd = new QLineEdit(widget);
    passwd->setEchoMode(QLineEdit::Password);
    passwd->setFixedSize(QSize(120, 30));
    passwd->setReadOnly(true);

    QLabel *labelName = new QLabel("User", widget);
    QLabel *labelFileName = new QLabel("File name", widget);
    labelPasswd = new QLabel("", widget);

    labelFileName->setFixedSize(QSize(120,30));
    labelName->setFixedSize(QSize(120,30));

    grid->addWidget(labelName, 0 ,0);
    grid->addWidget(user, 1, 0);
    grid->addWidget(labelFileName, 2, 0);
    grid->addWidget(file, 3, 0);
    grid->addWidget(btn, 5,0);
    grid->addWidget(btn2, 6,0);
    grid->addWidget(labelPasswd, 7, 0);
    grid->addWidget(passwd, 8, 0);
    grid->addWidget(okBtn, 9, 0);
}

WindowBanUser::WindowBanUser(QSize fixedSize) {
    this->setFixedSize(fixedSize);
    int width = this->frameGeometry().width();
    int height = this->frameGeometry().height();
    QDesktopWidget wid;
    int screenWidth = wid.width();
    int screenHeight = wid.height();
    this->setGeometry((screenWidth/2)-(width/2),(screenHeight/2)-(height/2),width,height);
    AddBtnAndTb();
}

void WindowBanUser::banUser() {
    QMessageBox msgBox;
    stringstream msgRight;
    int uid = std::stoi(user->toPlainText().toStdString());
    string filename = file->toPlainText().toStdString();
    if (passwd_exists(uid, filename.c_str())){
        okBtn->setEnabled(true);
        passwd->setReadOnly(false);
        labelPasswd->setText("Enter password");
        connect(okBtn, &QPushButton::released, this, &WindowBanUser::passwordHandler);
    }
    else{
        try
        {
            int res = sec_ban_user(uid, filename.c_str());
            msgBox.setText((string("Got result unban user: ") + to_string(res)).c_str());
            msgBox.exec();
        }
        catch(std::invalid_argument const& ex){
            msgBox.setText(QString("Error  : ") + QString(ex.what()));
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
        }
        okBtn->setEnabled(false);
        passwd->setReadOnly(true);
        labelPasswd->setText("");
    }
}

void WindowBanUser::unbanUser() {
    QMessageBox msgBox;
    stringstream msgRight;
    int uid = std::stoi(user->toPlainText().toStdString());
    string filename = file->toPlainText().toStdString();
    if (passwd_exists(uid, filename.c_str())){
        okBtn->setEnabled(true);
        passwd->setReadOnly(false);
        labelPasswd->setText("Enter password");
        connect(okBtn, &QPushButton::released, this, &WindowBanUser::passwordHandler);
    }
    else{
        try
        {
            int res = sec_unban_user(uid, filename.c_str());
            msgBox.setText((string("Got result unban user: ") + to_string(res)).c_str());
            msgBox.exec();
        }
        catch(std::invalid_argument const& ex){
            msgBox.setText(QString("Error  : ") + QString(ex.what()));
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
        }
        okBtn->setEnabled(false);
        passwd->setReadOnly(true);
        labelPasswd->setText("");
    }
}

void WindowBanUser::passwordHandler(){
    stringstream msgRight;
    QMessageBox msgBox;
    stringstream right_in_stream;
    int uid = std::stoi(user->toPlainText().toStdString());
    string filename = file->toPlainText().toStdString();
    string password = passwd->text().toStdString();

    if(authorization_by_passwd(uid, filename.c_str(), password.c_str())){
        try
        {
            int res = sec_unban_user(uid, filename.c_str());
            msgBox.setText((string("Got result ban user: ") + to_string(res)).c_str());
            msgBox.exec();
        }
        catch(std::invalid_argument const& ex){
            msgBox.setText(QString("Error  : ") + QString(ex.what()));
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
        }
        okBtn->setEnabled(false);
        passwd->setReadOnly(true);
        labelPasswd->setText("");
    }
    else{
        msgBox.setText(QString("Error"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }
}