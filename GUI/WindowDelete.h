#ifndef TRYGUI_WINDOWDELETE_H
#define TRYGUI_WINDOWDELETE_H

#include "UsingLibrarys.h"
#include "StorageLib/sec_library.h"

class WindowDelete :public QMainWindow {
Q_OBJECT
private:
    QTextEdit *user;
    QTextEdit *file;
    QPushButton *btn;
    QLineEdit* passwd;
    QLabel* labelPasswd;
    QPushButton* okBtn;
public:
    WindowDelete(QSize fixedSize);
    void AddBtnAndTb();
    void Delete(int uid, const char* filename);

public slots:
    void deleteFile();

private slots:
    void passwordHandler();
};
#endif