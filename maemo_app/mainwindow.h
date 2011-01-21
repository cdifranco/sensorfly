#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtCore/QThread>

//class Packet;
class QBuffer;
class QString;
class QStackedWidget;
class QWidget;
class QLabel;
class QMenu;
class QAction;
class QLineEdit;
class QPushButton;

class naviThread : public QThread
{
    Q_OBJECT

public:
    naviThread(QLineEdit *destEdit,
               int socket,
               QStackedWidget *centralWidgets,
               int naviIndex,
               int forwardIndex,
               int backIndex,
               int leftIndex,
               int rightIndex);

    void stop();

signals:
    void directRefreshed(int directIndex);
    void cancelSignal();
    void arriSignal();

protected:
    void run();

private:
    QLineEdit *destEdit;
    int socket;
    QStackedWidget *centralWidgets;
    int naviIndex;
    int forwardIndex;
    int backIndex;
    int leftIndex;
    int rightIndex;

    volatile bool isNavi;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:

private slots:
    void about(void) const;
    void btChangeDone(QString btAddr, QString btName, QString btIcon, QString majorClass, QString minorClass, bool trusted, QStringList services);
    void enableGoButton(const QString &text) const;
    void naviBegin(void);
    void naviCanceled(void);
    void naviArrived(void);

private:
    /*****Methods*****/
    void createCentralWidgets(void);
    void createActions(void);
    void createMainWidget(void);
    void createNaviWidget(void);
    void createCtrlWidget(void);

    void createNaviDirect(void);
    void createNaviDest(void);
    void createNaviSubWidget(void);

    void initBluetoothConn(void);
    void setupBluetoothConn(void);
    void changeBluetoothConn(void);
    void initNetConn(void);
    void setupNetConn(void);
    void changeNetConn(void);
    //void sendPkt(Packet *buf) const;
    //Packet *resvPkt() const;
    void initNaviThread(void);

    /*****Static Constant*****/
    static const QString* const btDbusService;
    static const QString* const btDbusInterface;
    static const QString* const btDbusPath;
    static const QString* const btSendFileReq;
    static const QString* const btSendFileSig;
    static const QString* const btSearchReq;
    static const QString* const btSearchSig;

    /*****Elements*****/
    QStackedWidget *centralWidgets;

    QWidget *mainWidget;
    QWidget *naviWidget;
    QWidget *ctrlWidget;

    QLabel *mainLabel;
    QLabel *naviLabel;
    QLabel *ctrlLabel;

    QMenu *mainMenu;
    QMenu *naviMenu;
    QMenu *ctrlMenu;

    QAction *naviAction;
    QAction *ctrlAction;
    QAction *aboutAction;
    QAction *exitAction;
    QAction *backAction;

    QWidget *mainSubWidget;
    QWidget *naviSubWidget;
    QWidget *ctrlSubWidget;

    QStackedWidget *naviDirect;
    QWidget *naviDest;
    QLineEdit *destEdit;
    QPushButton *goButton;
    QPushButton *cancelButton;
    QPushButton *arriButton;

    int mainIndex;
    int naviIndex;
    int ctrlIndex;

    int forwardIndex;
    int backIndex;
    int leftIndex;
    int rightIndex;

    QString *sfbtAddr;  //Sensorfly Bluetooth Address
    int btSocket;
    QString *sfnetAddr;  //Sensorfly Network Address
    int netSocket;

    naviThread *naviThd;
};



#endif // MAINWINDOW_H
