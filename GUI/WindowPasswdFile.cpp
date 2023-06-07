#include "WindowPasswdFile.h"

void WindowPasswdFile::AddBtnAndTb() {
    this->setWindowTitle("Passwd file");
    QWidget *widget = new QWidget(this);
    widget->setFixedSize(this->width(), this->height());

    btn = new QPushButton("Passwd", widget);
    btn->setFixedSize(QSize(120, 30));
    connect(btn, &QPushButton::released, this, &WindowPasswdFile::passwdFile);

    QGridLayout *grid = new QGridLayout(widget);

    user = new QTextEdit(widget);
    user->setFixedSize(QSize(120, 30));

    file = new QLineEdit(widget);
    file->setFixedSize(QSize(120, 30));

    pwd = new QLineEdit(widget);
    pwd->setEchoMode(QLineEdit::Password);
    pwd->setFixedSize(QSize(120, 30));

    QLabel *labelName = new QLabel("User", widget);
    QLabel *labelFileName = new QLabel("File name", widget);
    QLabel *labelPassword = new QLabel("Enter password", widget);

    labelFileName->setFixedSize(QSize(120,30));
    labelName->setFixedSize(QSize(120,30));
    labelPassword->setFixedSize(QSize(120,30));

    grid->addWidget(labelName, 0 ,0);
    grid->addWidget(user, 1, 0);
    grid->addWidget(labelFileName, 2, 0);
    grid->addWidget(file, 3, 0);
    grid->addWidget(labelPassword, 4, 0);
    grid->addWidget(pwd, 5, 0);
    grid->addWidget(btn, 7,0);
}

WindowPasswdFile::WindowPasswdFile(QSize fixedSize){
    this->setFixedSize(fixedSize);
    int width = this->frameGeometry().width();
    int height = this->frameGeometry().height();
    QDesktopWidget wid;
    int screenWidth = wid.width();
    int screenHeight = wid.height();
    this->setGeometry((screenWidth/2)-(width/2),(screenHeight/2)-(height/2),width,height);
    AddBtnAndTb();
}

void WindowPasswdFile::passwdFile() {
    QMessageBox msgBox;
    stringstream msgRight;
    try
    {
        int uid = std::stoi(user->toPlainText().toStdString());
        string filename = file->text().toStdString();
        string passwd = pwd->text().toStdString();

        int res = sec_passwd_by_file(uid, filename.c_str(), passwd.c_str());
        msgBox.setText((string("Got result passwd file: ") + to_string(res)).c_str());
        msgBox.exec();
    }
    catch(std::invalid_argument const& ex){
        msgBox.setText(QString("Error: ") + QString(ex.what()));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }
}