#include "WindowRevoke.h"

void WindowRevoke::AddBtnAndTb() {
    this->setWindowTitle("Rewoke");
    QWidget *widget = new QWidget(this);
    widget->setFixedSize(this->width(), this->height());

    btn = new QPushButton("Revoke right", widget);
    btn->setFixedSize(QSize(120, 30));
    connect(btn, &QPushButton::released, this, &WindowRevoke::revokeRight);

    okBtn = new QPushButton("Enter", widget);
    okBtn->setFixedSize(QSize(120, 30));
    okBtn->setEnabled(false);

    QGridLayout *grid = new QGridLayout(widget);

    user = new QTextEdit(widget);
    user->setFixedSize(QSize(120, 30));

    file = new QTextEdit(widget);
    file->setFixedSize(QSize(120, 30));

    right = new QTextEdit(widget);
    right->setFixedSize(QSize(120, 30));

    passwd = new QLineEdit(widget);
    passwd->setEchoMode(QLineEdit::Password);
    passwd->setFixedSize(QSize(120, 30));
    passwd->setReadOnly(true);

    QLabel *labelName = new QLabel("Revoke to user", widget);
    QLabel *labelFileName = new QLabel("File name", widget);
    QLabel *labelRight = new QLabel("Right", widget);
    labelPasswd = new QLabel("", widget);

    labelFileName->setFixedSize(QSize(120,30));
    labelName->setFixedSize(QSize(120,30));
    labelRight->setFixedSize(QSize(120,30));

    grid->addWidget(labelName, 0 ,0);
    grid->addWidget(user, 1, 0);
    grid->addWidget(labelFileName, 2, 0);
    grid->addWidget(file, 3, 0);
    grid->addWidget(labelRight, 4, 0);
    grid->addWidget(right, 5,0);
    grid->addWidget(btn, 6,0);
    grid->addWidget(labelPasswd, 7, 0);
    grid->addWidget(passwd, 8, 0);
    grid->addWidget(okBtn, 9, 0);
}

WindowRevoke::WindowRevoke(QSize fixedSize) {
    this->setFixedSize(fixedSize);
    int width = this->frameGeometry().width();
    int height = this->frameGeometry().height();
    QDesktopWidget wid;
    int screenWidth = wid.width();
    int screenHeight = wid.height();
    this->setGeometry((screenWidth/2)-(width/2),(screenHeight/2)-(height/2),width,height);
    AddBtnAndTb();
}

void WindowRevoke::Revoke(int uid, const char *filename, right_t right) {
    stringstream msgRight;
    QMessageBox msgBox;
    try
    {
        right_t received_r = sec_revoke(uid, filename, right);
        msgRight << received_r;
        msgBox.setText((string("Got result rewoke-right: ") + msgRight.str()).c_str());
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

void WindowRevoke::revokeRight() {
    QMessageBox msgBox;
    stringstream msgRight;
    int uid = std::stoi(user->toPlainText().toStdString());
    string filename = file->toPlainText().toStdString();
    if (passwd_exists(uid, filename.c_str())){
        okBtn->setEnabled(true);
        passwd->setReadOnly(false);
        labelPasswd->setText("Enter password");
        connect(okBtn, &QPushButton::released, this, &WindowRevoke::passwordHandler);
    }
    else{
        right_t r = (right_t)(std::stoi(right->toPlainText().toStdString()));
        Revoke(uid, filename.c_str(), r);
    }
}

void WindowRevoke::passwordHandler(){
    QMessageBox msgBox;
    stringstream right_in_stream;
    int uid = std::stoi(user->toPlainText().toStdString());
    string filename = file->toPlainText().toStdString();
    string password = std::to_string(std::hash<std::string>{}(passwd->text().toStdString()));
    if(authorization_by_passwd(uid, filename.c_str(), password.c_str())){
        right_t r = (right_t)(std::stoi(right->toPlainText().toStdString()));
        Revoke(uid, filename.c_str(), r);
    }
    else{
        msgBox.setText(QString("Error"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }
}






