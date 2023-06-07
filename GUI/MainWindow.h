#include "UsingLibrarys.h"
#include "WindowRead.h"
#include "WindowGetRight.h"
#include "WindowGrant.h"
#include "WindowListStorage.h"
#include "WindowRevoke.h"
#include "WindowDelete.h"
#include "WindowBanUser.h"
#include "WindowPasswdFile.h"

class MainWindow : public QMainWindow{
Q_OBJECT

private:
    QPushButton *btnOpen;
    QPushButton *btnGrant;
    WindowRead* windowRead;
    WindowGetRight* windowGetRight;
    QPushButton *btnGetRight;
    WindowGrant *windowGrant;
    WindowListStorage* windowListStorage;
    QPushButton *btnListStorage;
    WindowRevoke* windowRevoke;
    QPushButton* btnRevoke;
    QLabel* labelID;
    QPushButton* btnDelete;
    WindowDelete* windowDelete;
    WindowBanUser* windowBanUser;
    QPushButton* btnBanUser;
    WindowPasswdFile* windowPasswdFile;
    QPushButton* btnPasswdFile;

private slots:
    void handlerButton();
    void handlerGetRight();
    void handlerGrant();
    void handlerListStorage();
    void handlerRevoke();
    void handlerDelete();
    void handlerBanUser();
    void handlerPasswdFile();

public:
    MainWindow(const QSize& maxBackgroundSize);
    ~MainWindow();
};
