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
    void Ban(int uid, const char* filename);
    void Unban(int uid, const char* filename);

private slots:
    void banUser();
    void unbanUser();
    void banPasswordHandler();
    void unbanPasswordHandler();
};
#endif
