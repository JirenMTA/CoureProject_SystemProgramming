#ifndef TRYGUI_WINDOWPASSWDFILE_H
#define TRYGUI_WINDOWPASSWDFILE_H

#include "UsingLibrarys.h"
#include "StorageLib/sec_library.h"

class WindowPasswdFile :public QMainWindow {
Q_OBJECT
private:
    QTextEdit *user;
    QLineEdit *file;
    QLineEdit *pwd;
    QPushButton *btn;
public:
    WindowPasswdFile(QSize fixedSize);

    void AddBtnAndTb();

public slots:

    void passwdFile();
};
#endif