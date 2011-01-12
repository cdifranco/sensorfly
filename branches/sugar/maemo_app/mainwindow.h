#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class Packet;
class QBuffer;
class QString;
class QStackedWidget;
class QWidget;
class QLabel;
class QMenu;
class QAction;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:

private slots:
    void about(void) const;
    void sendData(void) const;

    void btChangeDone(QString btAddr, QString btName, QString btIcon, QString majorClass, QString minorClass, bool trusted, QStringList services);

private:
    /*****Methods*****/
    void createCentralWidgets(void);
    void createActions(void);
    void createMainWidget(void);
    void createNaviWidget(void);
    void createCtrlWidget(void);

    void initBluetoothConn(void);
    void setupBluetoothConn(void);
    void changeBluetoothConn(void);
    void sendPkt(Packet *buf) const;
    Packet *resvPkt() const;

    /******Static Constant*****/
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

    int mainIndex;
    int naviIndex;
    int ctrlIndex;

    QString *sfbtAddr;  //Sensorfly Bluetooth Address
    int btSocket;
};

#endif // MAINWINDOW_H
