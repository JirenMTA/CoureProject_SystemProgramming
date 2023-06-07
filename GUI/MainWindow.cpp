#include "MainWindow.h"

MainWindow::MainWindow(const QSize& maxBackgroundSize) {
    this->setFixedSize(maxBackgroundSize);
    int width = this->frameGeometry().width();
    int height = this->frameGeometry().height();
    QDesktopWidget wid;
    int screenWidth = wid.width();
    int screenHeight = wid.height();
    this->setGeometry((screenWidth/2)-(width/2),(screenHeight/2)-(height/2),width,height);

    QWidget* widget = new QWidget(this);
    widget->setFixedSize(this->size());

    labelID = new QLabel(widget);
    setuid(getuid());
    string uid_instr = to_string((int)(getuid()));
    labelID->setText(QString("USER ID: ") + QString(uid_instr.c_str()));
    labelID->setFixedSize(200, 40);

    btnOpen = new QPushButton("Open file", widget);
    btnOpen->setFixedSize(QSize(200, 40));
    connect(btnOpen, &QPushButton::released, this, &MainWindow::handlerButton);

    btnGetRight = new QPushButton("Get right", widget);
    btnGetRight->setFixedSize(QSize(200, 40));
    connect(btnGetRight, &QPushButton::released, this, &MainWindow::handlerGetRight);

    btnGrant = new QPushButton("Grant right", widget);
    btnGrant->setFixedSize(QSize(200, 40));
    connect(btnGrant, &QPushButton::released, this, &MainWindow::handlerGrant);

    btnListStorage = new QPushButton("List storage");
    btnListStorage->setFixedSize(QSize(200, 40));
    connect(btnListStorage, &QPushButton::released, this, &MainWindow::handlerListStorage);

    btnRevoke = new QPushButton("Revoke right");
    btnRevoke->setFixedSize(QSize(200, 40));
    connect(btnRevoke, &QPushButton::released, this, &MainWindow::handlerRevoke);

    btnDelete= new QPushButton("Delete");
    btnDelete->setFixedSize(QSize(200, 40));
    connect(btnDelete, &QPushButton::released, this, &MainWindow::handlerDelete);

    btnBanUser= new QPushButton("Ban/Unban user");
    btnBanUser->setFixedSize(QSize(200, 40));
    connect(btnBanUser, &QPushButton::released, this, &MainWindow::handlerBanUser);

    btnPasswdFile= new QPushButton("Passwd File");
    btnPasswdFile->setFixedSize(QSize(200, 40));
    connect(btnPasswdFile, &QPushButton::released, this, &MainWindow::handlerPasswdFile);

    QGridLayout* grid = new QGridLayout(widget);

    grid->addWidget(labelID, 0, 0);
    grid->addWidget(btnOpen, 1, 0);
    grid->addWidget(btnGetRight, 2, 0);
    grid->addWidget(btnGrant, 3,0);
    grid->addWidget(btnListStorage, 1, 1);
    grid->addWidget(btnRevoke, 2, 1);
    grid->addWidget(btnDelete,3, 1);
    grid->addWidget(btnBanUser,4, 0);
    grid->addWidget(btnPasswdFile,4, 1);
    widget->show();
    sec_init();
}

 MainWindow::~MainWindow() noexcept {
    delete btnOpen;
    delete btnGetRight;
    delete this;
}

void MainWindow::handlerButton() {
    WindowRead* windowRead = new WindowRead (QSize(400, 300));
    windowRead->show();
}

void MainWindow::handlerGetRight() {
    windowGetRight= new WindowGetRight(QSize(400, 300));
    windowGetRight->show();
}

void MainWindow::handlerGrant() {
    windowGrant = new WindowGrant(QSize(400,300));
    windowGrant->show();;
}

void MainWindow::handlerListStorage() {
    windowListStorage = new WindowListStorage(QSize(400,300));
    windowListStorage->show();
}

void MainWindow::handlerRevoke() {
    windowRevoke = new WindowRevoke(QSize(400, 300));
    windowRevoke->show();
}

void MainWindow::handlerDelete() {
    windowDelete = new WindowDelete(QSize(400, 300));
    windowDelete->show();
}

void MainWindow::handlerBanUser() {
    windowBanUser = new WindowBanUser(QSize(400, 300));
    windowBanUser->show();
}

void MainWindow::handlerPasswdFile() {
    windowPasswdFile = new WindowPasswdFile(QSize(400, 300));
    windowPasswdFile->show();
}















