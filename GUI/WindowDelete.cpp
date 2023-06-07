#include "WindowDelete.h"

void WindowDelete::AddBtnAndTb() {
    this->setWindowTitle("Delete");
    QWidget *widget = new QWidget(this);
    widget->setFixedSize(this->width(), this->height());

    btn = new QPushButton("Delete file", widget);
    btn->setFixedSize(QSize(120, 30));
    connect(btn, &QPushButton::released, this, &WindowDelete::deleteFile);

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

    QLabel *labelName = new QLabel("Owner", widget);
    QLabel *labelFileName = new QLabel("File name", widget);

    labelFileName->setFixedSize(QSize(120,30));
    labelName->setFixedSize(QSize(120,30));
    labelPasswd = new QLabel("", widget);

    grid->addWidget(labelName, 0 ,0);
    grid->addWidget(user, 1, 0);
    grid->addWidget(labelFileName, 2, 0);
    grid->addWidget(file, 3, 0);
    grid->addWidget(btn, 5,0);
    grid->addWidget(labelPasswd, 6, 0);
    grid->addWidget(passwd, 7, 0);
    grid->addWidget(okBtn, 8, 0);
}

WindowDelete::WindowDelete(QSize fixedSize) {
    this->setFixedSize(fixedSize);
    int width = this->frameGeometry().width();
    int height = this->frameGeometry().height();
    QDesktopWidget wid;
    int screenWidth = wid.width();
    int screenHeight = wid.height();
    this->setGeometry((screenWidth/2)-(width/2),(screenHeight/2)-(height/2),width,height);
    AddBtnAndTb();
}

void WindowDelete::Delete(int uid, const char* filename){
    QMessageBox msgBox;
    try
    {
        int res = sec_unlink_at(uid, filename);
        msgBox.setText((string("Got result delete: ") + to_string(res)).c_str());
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

void WindowDelete::deleteFile() {
    int uid = std::stoi(user->toPlainText().toStdString());
    string filename = file->toPlainText().toStdString();

    if (passwd_exists(uid, filename.c_str())){
        okBtn->setEnabled(true);
        passwd->setReadOnly(false);
        labelPasswd->setText("Enter password");
        connect(okBtn, &QPushButton::released, this, &WindowDelete::passwordHandler);
    }
    else{
        Delete(uid, filename.c_str());
    }
}

void WindowDelete::passwordHandler(){
    QMessageBox msgBox;
    int uid = std::stoi(user->toPlainText().toStdString());
    string filename = file->toPlainText().toStdString();
    string password = std::to_string(std::hash<std::string>{}(passwd->text().toStdString()));
    if(authorization_by_passwd(uid, filename.c_str(), password.c_str())){
        Delete(uid, filename.c_str());
    }
    else{
        msgBox.setText(QString("Error"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }
}




