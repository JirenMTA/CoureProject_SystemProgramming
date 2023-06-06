#ifndef TRYGUI_WINDOWBANUSER_H
#define TRYGUI_WINDOWBANUSER_H

#include "UsingLibrarys.h"
#include "StorageLib/sec_library.h"

class WindowBanUser :public QMainWindow {
Q_OBJECT
private:
    QTextEdit *user;
    QTextEdit *file;
    QPushButton *btn;
    QPushButton *btn2;
    QLineEdit* passwd;
    QLabel* labelPasswd;
    QPushButton* okBtn;
public:
    WindowBanUser(QSize fixedSize);

    void AddBtnAndTb();

public slots:
    void banUser();
    void unbanUser();
private slots:
    void passwordHandler();
};
#endif
