//
// Created by hung on 08.03.23.
//

#ifndef TRYGUI_WINDOWGRANT_H
#define TRYGUI_WINDOWGRANT_H

#include "UsingLibrarys.h"
#include "StorageLib/sec_library.h"
class WindowGrant: public QMainWindow {
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
    WindowGrant(QSize fixedSize);
    void AddBtnAndTb();
public slots:
    void setRight();
private slots:
    void passwordHandler();
};


#endif //TRYGUI_WINDOWGRANT_H
