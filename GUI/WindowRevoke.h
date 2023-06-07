#ifndef TRYGUI_WINDOWREVOKE_H
#define TRYGUI_WINDOWREVOKE_H

#include "UsingLibrarys.h"
#include "StorageLib/sec_library.h"

class WindowRevoke: public QMainWindow {
    Q_OBJECT
private:
    QTextEdit* user;
    QTextEdit* file;
    QTextEdit* right;
    QPushButton* btn;
    QLineEdit* passwd;
    QLabel* labelPasswd;
    QPushButton* okBtn;

public:
    WindowRevoke(QSize fixedSize);
    void AddBtnAndTb();
    void Revoke(int uid, const char* filename, right_t right);

public slots:
    void revokeRight();

private slots:
    void passwordHandler();
};

#endif
