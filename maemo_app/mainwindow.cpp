#include <QtGui>
#include <QtDBus/QtDBus>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "mainwindow.h"
//#include "packet.h"

//#define ENABLE_BLUETOOTH
#define INTI_BT_ADDR "C8:97:9F:6D:E2:40" //"00:06:66:04:B1:61"
#define INTI_NET_ADDR "10.0.16.193"
#define INIT_NET_PORT 8964
#define MAX_DEST_NUM 27

/*****Navigation Thread*****/
naviThread::naviThread(QLineEdit *msgEdit,
                       int socket,
                       QStackedWidget *centralWidgets,
                       int naviIndex,
                       int forwardIndex,
                       int backIndex,
                       int leftIndex,
                       int rightIndex)
                       :
                       msgEdit(msgEdit),
                       socket(socket),
                       centralWidgets(centralWidgets),
                       naviIndex(naviIndex),
                       forwardIndex(forwardIndex),
                       backIndex(backIndex),
                       leftIndex(leftIndex),
                       rightIndex(rightIndex)
{
    isNavi = false;
}

void naviThread::stop()
{
    isNavi = false;
}

void naviThread::run()
{
    isNavi = true;
    QString dest = msgEdit->text();

    ushort outBuf;
    outBuf = dest.toUShort();
    qDebug() << dest.toUShort() <<"before";
    while(isNavi && send(socket,&outBuf,1,0) < 0);
    qDebug() << dest.toUShort();

    char inBuf;
    bool manualStop = true;
    while(isNavi && centralWidgets->currentIndex() == naviIndex){
        inBuf = ushort(0);
        while(isNavi && recv(socket, &inBuf, 1, 0) < 0);
        qDebug() << inBuf;
        switch(inBuf){
            case 'f':
                emit directRefreshed(forwardIndex);
                //sleep(2);
                //outBuf = ushort(1);
                //while(isNavi && send(socket,&outBuf,1,0) < 0);
                break;
            case 'b':
                emit directRefreshed(backIndex);
                //sleep(2);
                //outBuf = ushort(1);
                //while(isNavi && send(socket,&outBuf,1,0) < 0);
                break;
            case 'l':
                emit directRefreshed(leftIndex);
                //sleep(2);
                //outBuf = ushort(1);
                //while(isNavi && send(socket,&outBuf,1,0) < 0);
                break;
            case 'r':
                emit directRefreshed(rightIndex);
                //sleep(2);
                //outBuf = ushort(1);
                //while(isNavi && send(socket,&outBuf,1,0) < 0);
                break;
            case 's':
                emit arriSignal();
                manualStop = false;
                break;
            case 'e':
                emit cancelSignal();
                manualStop = false;
                break;
            default:
                break;
        }
    }
    if(manualStop){
        qDebug() << "stop";
        outBuf = ushort(0);
        while(send(socket,&outBuf,1,0) < 0);
    }
}


/*****Public Methods*****/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createCentralWidgets();

    setCentralWidget(centralWidgets);

#ifdef ENABLE_BLUETOOTH
    initBluetoothConn();
#else
    initNetConn();
#endif
    initNaviThread();
}

MainWindow::~MainWindow()
{
    if(btSocket != -1){
        ::shutdown(btSocket, 2);
        ::close(btSocket);
    }
    if(netSocket != -1){
        ::shutdown(netSocket, 2);
        ::close(netSocket);
    }
}


/*****Private Slots*****/

void MainWindow::about(void) const
{
    QMessageBox *aboutSensorfly = new QMessageBox(QMessageBox::Information, tr("Sensorfly"), tr("The SensorFly is a novel low-cost controlled-mobile aerial sensor networking platform. A flock of these 29g autonomous helicopter nodes with communication, ranging and collaborative path determination capabilities, can be useful in sensing survivors after disasters or adversaries in urban combat scenarios.\r\nWebsite : http://sensorfly.sv.cmu.edu/"));
    aboutSensorfly->show();
}

void MainWindow::btChangeDone(QString btAddr, QString btName, QString btIcon, QString majorClass, QString minorClass, bool trusted, QStringList services)
{
    qDebug() << btAddr << btName << btIcon << majorClass << minorClass << trusted << services;
    sfbtAddr = &btAddr;
    setupBluetoothConn();
}

void MainWindow::enableGoButton(const QString &text) const
{
    qDebug() << text.toShort();
    if(msgLabel->text() == QString(tr("Dest:")) && 0 < text.toShort() && text.toShort() <= MAX_DEST_NUM){
        goButton->setEnabled(true);
    }else{
        goButton->setEnabled(false);
    }
}

void MainWindow::msgEditSltChg(void)
{
    if(msgLabel->text() == QString(tr("Msg:"))){
        msgLabel->setText(tr("Dest:"));
        msgEdit->clear();
    }
}

//Clicked goButton
void MainWindow::naviBegin(void)
{
    goButton->setEnabled(false);
    cancelButton->setEnabled(true);
    arriButton->setEnabled(true);
    sendAreaButton->setEnabled(true);
    msgLabel->setText(tr("Area:"));
    qDebug() << "Navi begins";
    naviThd->start();
}

//Clicked cancelButton
void MainWindow::naviCanceled(void)
{
    naviThd->stop();
    msgEdit->clear();
    goButton->setEnabled(false);
    cancelButton->setEnabled(false);
    arriButton->setEnabled(false);
    sendAreaButton->setEnabled((false));
    msgLabel->setText(tr("Msg:"));
    msgEdit->setText(tr("Navi Canceled"));
    qDebug() << "Navi canceled";
}

//Clicked arriButton or get arriMsg
void MainWindow::naviArrived(void)
{
    naviThd->stop();
    msgEdit->clear();
    goButton->setEnabled(false);
    cancelButton->setEnabled(false);
    arriButton->setEnabled(false);
    sendAreaButton->setEnabled(false);
    msgLabel->setText(tr("Msg:"));
    msgEdit->setText(tr("Navi Arrived"));
    qDebug() << "Navi arrived";
}

//Clicked arriButton or get arriMsg
void MainWindow::sendArea(void)
{
    QString dest = msgEdit->text();
    ushort outBuf;
    outBuf = dest.toUShort();
#ifdef ENABLE_BLUETOOTH
    while(send(btSocket,&outBuf,1,0) < 0);
#else
    while(send(netSocket,&outBuf,1,0) < 0);
#endif
}

/*****Private Methods*****/

void MainWindow::createCentralWidgets(void)
{
    centralWidgets = new QStackedWidget;

    mainWidget = new QWidget;
    naviWidget = new QWidget;
    ctrlWidget = new QWidget;

    mainIndex = centralWidgets->addWidget(mainWidget);
    naviIndex = centralWidgets->addWidget(naviWidget);
    ctrlIndex = centralWidgets->addWidget(ctrlWidget);

    createActions();
    createMainWidget();
    createNaviWidget();
    createCtrlWidget();
}

void MainWindow::createActions(void)
{
    QSignalMapper *signalMapper = new QSignalMapper(centralWidgets);

    naviAction = new QAction(tr("Navigate"), this);
    connect(naviAction, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(naviAction, naviIndex);
    connect(signalMapper, SIGNAL(mapped(int)), centralWidgets, SLOT(setCurrentIndex(int)));

    ctrlAction = new QAction(tr("Control"), this);
    connect(ctrlAction, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(ctrlAction, ctrlIndex);
    connect(signalMapper, SIGNAL(mapped(int)), centralWidgets, SLOT(setCurrentIndex(int)));

    aboutAction = new QAction(tr("About"), this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    exitAction = new QAction(tr("Exit"), this);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    backAction = new QAction(tr("Back"), this);
    connect(backAction, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(backAction, mainIndex);
    connect(signalMapper, SIGNAL(mapped(int)), centralWidgets, SLOT(setCurrentIndex(int)));
}

void MainWindow::createMainWidget(void)
{
    mainLabel = new QLabel(tr("Main:"));

    mainMenu = new QMenu;
    mainMenu->addAction(naviAction);
    mainMenu->addAction(ctrlAction);
    mainMenu->addAction(aboutAction);
    mainMenu->addAction(exitAction);

    //mainSubWidget = new QWidget;
    QLabel *mainSubLabel = new QLabel();
    QImage *mainImg = new QImage(":/images/mainImg.png");
    mainSubLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainSubLabel->setAlignment(Qt::AlignCenter);
    mainSubLabel->setPixmap(QPixmap::fromImage(*mainImg, Qt::AutoColor));
    mainSubWidget = dynamic_cast<QWidget *>(mainSubLabel);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(mainLabel);
    leftLayout->addWidget(mainMenu);
    leftLayout->addStretch();
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(leftLayout);
    mainLayout->addStretch();
    mainLayout->addWidget(mainSubWidget);

    mainWidget->setLayout(mainLayout);
}

void MainWindow::createNaviWidget(void)
{
    naviLabel = new QLabel(tr("Navigate:"));

    naviMenu = new QMenu;
    naviMenu->addAction(backAction);

    createNaviSubWidget();

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(naviLabel);
    leftLayout->addWidget(naviMenu);
    leftLayout->addStretch();
    QHBoxLayout *naviLayout = new QHBoxLayout;
    naviLayout->addLayout(leftLayout);
    naviLayout->addStretch();
    naviLayout->addWidget(naviSubWidget);
    naviLayout->addStretch();

    naviWidget->setLayout(naviLayout);
}

void MainWindow::createCtrlWidget(void)
{
    ctrlLabel = new QLabel(tr("Control:"));

    ctrlMenu = new QMenu;
    ctrlMenu->addAction(backAction);

    //ctrlSubWidget = new QWidget;
    //TODO: construct the control widget
    QPushButton *testSendButton = new QPushButton(tr("send"));
    //connect(testSendButton, SIGNAL(clicked()), this, SLOT(sendData()));
    ctrlSubWidget = dynamic_cast<QWidget *>(testSendButton);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(ctrlLabel);
    leftLayout->addWidget(ctrlMenu);
    leftLayout->addStretch();
    QHBoxLayout *ctrlLayout = new QHBoxLayout;
    ctrlLayout->addLayout(leftLayout);
    ctrlLayout->addStretch();
    ctrlLayout->addWidget(ctrlSubWidget);
    ctrlLayout->addStretch();

    ctrlWidget->setLayout(ctrlLayout);
}

void MainWindow::createNaviDirectWidget(void)
{
    naviDirect = new QStackedWidget;
    QWidget *forwardWidget = new QWidget;
    QWidget *backWidget = new QWidget;
    QWidget *leftWidget = new QWidget;
    QWidget *rightWidget = new QWidget;
    QLabel *naviDirectLabel;
    QImage *naviDirectImg;
    //forward
    naviDirectLabel = new QLabel();
    naviDirectImg = new QImage(":/images/forward_arrow.png");
    naviDirectLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    naviDirectLabel->setAlignment(Qt::AlignCenter);
    naviDirectLabel->setPixmap(QPixmap::fromImage(*naviDirectImg, Qt::AutoColor));
    forwardWidget = dynamic_cast<QWidget *>(naviDirectLabel);
    forwardIndex = naviDirect->addWidget(forwardWidget);
    //back
    naviDirectLabel = new QLabel();
    naviDirectImg = new QImage(":/images/back_arrow.png");
    naviDirectLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    naviDirectLabel->setAlignment(Qt::AlignCenter);
    naviDirectLabel->setPixmap(QPixmap::fromImage(*naviDirectImg, Qt::AutoColor));
    backWidget = dynamic_cast<QWidget *>(naviDirectLabel);
    backIndex = naviDirect->addWidget(backWidget);
    //left
    naviDirectLabel = new QLabel();
    naviDirectImg = new QImage(":/images/left_arrow.png");
    naviDirectLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    naviDirectLabel->setAlignment(Qt::AlignCenter);
    naviDirectLabel->setPixmap(QPixmap::fromImage(*naviDirectImg, Qt::AutoColor));
    leftWidget = dynamic_cast<QWidget *>(naviDirectLabel);
    leftIndex = naviDirect->addWidget(leftWidget);
    //right
    naviDirectLabel = new QLabel();
    naviDirectImg = new QImage(":/images/right_arrow.png");
    naviDirectLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    naviDirectLabel->setAlignment(Qt::AlignCenter);
    naviDirectLabel->setPixmap(QPixmap::fromImage(*naviDirectImg, Qt::AutoColor));
    rightWidget = dynamic_cast<QWidget *>(naviDirectLabel);
    rightIndex = naviDirect->addWidget(rightWidget);
}

void MainWindow::createNaviDestWidget(void)
{
    naviDest = new QWidget;

    msgLabel = new QLabel(tr("Dest:"));
    msgEdit = new QLineEdit;
    msgLabel->setBuddy(msgEdit);
    QValidator *editValidator = new QIntValidator(1, MAX_DEST_NUM, msgEdit);
    msgEdit->setValidator(editValidator);

    goButton = new QPushButton(tr("Go!"));
    goButton->setEnabled(false);

    cancelButton = new QPushButton(tr("Cancel"));
    cancelButton->setEnabled(false);

    arriButton = new QPushButton(tr("I Arrive"));
    arriButton->setEnabled(false);

    sendAreaButton = new QPushButton(tr("Send Area"));
    sendAreaButton->setEnabled(false);

    QHBoxLayout *navimsgEditLayout = new QHBoxLayout;
    navimsgEditLayout->addWidget(msgLabel);
    navimsgEditLayout->addWidget(msgEdit);
    QVBoxLayout *naviDestLayout = new QVBoxLayout;
    naviDestLayout->addLayout(navimsgEditLayout);
    naviDestLayout->addWidget(goButton);
    naviDestLayout->addWidget(cancelButton);
    naviDestLayout->addWidget(arriButton);
    naviDestLayout->addWidget(sendAreaButton);
    naviDestLayout->addStretch();

    naviDest->setLayout(naviDestLayout);

    connect(msgEdit, SIGNAL(textChanged(const QString &)), this, SLOT(enableGoButton(const QString &)));
    connect(msgEdit, SIGNAL(selectionChanged()), this, SLOT(msgEditSltChg()));
    connect(goButton, SIGNAL(clicked()), this, SLOT(naviBegin()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(naviCanceled()));
    connect(arriButton, SIGNAL(clicked()), this, SLOT(naviArrived()));
    connect(sendAreaButton, SIGNAL(clicked()), this, SLOT(sendArea()));
}

void MainWindow::createNaviSubWidget(void)
{
    createNaviDirectWidget();
    createNaviDestWidget();

    naviSubWidget = new QWidget;

    QHBoxLayout *naviSubWidgetLayout = new QHBoxLayout;
    naviSubWidgetLayout->addWidget(naviDirect);
    naviSubWidgetLayout->addWidget(naviDest);

    naviSubWidget->setLayout(naviSubWidgetLayout);

}

void MainWindow::initBluetoothConn(void)
{
    btSocket = -1;
    sfbtAddr = new QString(INTI_BT_ADDR);

    setupBluetoothConn();

    QDBusConnection::systemBus().connect(*btDbusService, *btDbusPath, *btDbusInterface, *btSearchSig, this, SLOT(btChangeDone(QString, QString, QString, QString, QString, bool, QStringList)));
}

void MainWindow::setupBluetoothConn(void)
{
    if(btSocket != -1){
        ::shutdown(btSocket, 2);
        ::close(btSocket);
    }

    //allocate a socket
    btSocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    //set the connection parameters (who to connect to)
    struct sockaddr_rc addr;
    QByteArray ba = sfbtAddr->toLatin1();
    char *dest = ba.data();
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba(dest, &addr.rc_bdaddr);

    //connect to server
    int status = ::connect(btSocket, (struct sockaddr *)&addr, sizeof(addr));

    if(status != 0){
        //TODO:handle exception
        qDebug() << "Fail to connect bluetooth.";
    }
}

void MainWindow::changeBluetoothConn(void)
{
    QDBusInterface *interface = new QDBusInterface(*btDbusService, *btDbusPath, *btDbusInterface, QDBusConnection::systemBus(), this);
    if(!interface->isValid()){
        qDebug() << qPrintable(QDBusConnection::systemBus().lastError().message());
        //TODO: handle exception
        exit(1);
    }
    interface->call(QDBus::AutoDetect, *btSearchReq, QString(""), QString(""), QStringList(""), QString("require"));
}

void MainWindow::initNetConn(void)
{
    netSocket = -1;
    sfnetAddr = new QString(INTI_NET_ADDR);

    setupNetConn();
}

void MainWindow::setupNetConn(void)
{
    if(netSocket != -1){
        ::shutdown(netSocket, 2);
        ::close(netSocket);
    }

    //allocate a socket
    netSocket = socket(AF_INET, SOCK_STREAM, 0);

    //set the connection parameters (who to connect to)
    sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    QByteArray na = sfnetAddr->toLatin1();
    char *dest = na.data();
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, dest, &addr.sin_addr);
    addr.sin_port = htons(INIT_NET_PORT);

    //connect to server
    int status;
    //do{
        status = ::connect(netSocket, (struct sockaddr *)&addr, sizeof(addr));
    //}while(status != 0);

    if(status != 0){
        //TODO:handle exception
        qDebug() << "Fail to connect network.";
    }
}

void MainWindow::changeNetConn(void)
{

}

/*void MainWindow::sendPkt(Packet *buf) const
{

}

Packet *MainWindow::resvPkt() const
{
    return NULL;
}*/

void MainWindow::initNaviThread(void)
{
    naviThd = new naviThread(msgEdit,
                             netSocket,
                             centralWidgets,
                             naviIndex,
                             forwardIndex,
                             backIndex,
                             leftIndex,
                             rightIndex);
    connect(naviThd, SIGNAL(directRefreshed(int)), naviDirect, SLOT(setCurrentIndex(int)));
    connect(naviThd, SIGNAL(cancelSignal()), this, SLOT(naviCanceled()));
    connect(naviThd, SIGNAL(arriSignal()), this, SLOT(naviArrived()));
}

/*****Static Elements*****/

const QString* const MainWindow::btDbusService = new QString("com.nokia.bt_ui");

const QString* const MainWindow::btDbusInterface = new QString("com.nokia.bt_ui");

const QString* const MainWindow::btDbusPath = new QString("/com/nokia/bt_ui");

const QString* const MainWindow::btSendFileReq = new QString("show_send_file_dlg");

const QString* const MainWindow::btSendFileSig = new QString("send_file");

const QString* const MainWindow::btSearchReq = new QString("show_search_dlg");

const QString* const MainWindow::btSearchSig = new QString("search_result");
