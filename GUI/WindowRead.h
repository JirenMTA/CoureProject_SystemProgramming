#ifndef TRYGUI_WINDOWREAD_H
#define TRYGUI_WINDOWREAD_H
#include "UsingLibrarys.h"
#include "StorageLib/sec_library.h"
class WindowRead: public QWidget {
Q_OBJECT
public:
    WindowRead(QSize fixedSize);
    WindowRead(const WindowRead&) = delete;
    void AddBtnAndTb();
private:
    QPushButton* btn;
    QTextEdit* name;
    QTextEdit* file;
    QTextEdit* text;
    QLineEdit* passwd;
    QLabel* labelPasswd;
    QPushButton* okBtn;
    QPushButton* setText;
private slots:
    void getDataOfFile();
    void setTextHandler();
    void passwordReadHandler();
    void passwordWriteHandler();
};


#endif //TRYGUI_WINDOWREAD_H
