#include <QtGui>
#include <QtDBus/QtDBus>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "mainwindow.h"
#include "packet.h"

#define INTI_BT_ADDR "C8:97:9F:6D:E2:40" //"00:06:66:04:B1:61"

/*****Public Methods*****/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createCentralWidgets();

    setCentralWidget(centralWidgets);

    initBluetoothConn();
}

MainWindow::~MainWindow()
{
    if(btSocket != -1){
        ::close(btSocket);
    }
}


/*****Private Slots*****/

void MainWindow::about(void) const
{
    QMessageBox *aboutSensorfly = new QMessageBox(QMessageBox::Information, tr("Sensorfly"), tr("The SensorFly is a novel low-cost controlled-mobile aerial sensor networking platform. A flock of these 29g autonomous helicopter nodes with communication, ranging and collaborative path determination capabilities, can be useful in sensing survivors after disasters or adversaries in urban combat scenarios.\r\nWebsite : http://sensorfly.sv.cmu.edu/"));
    aboutSensorfly->show();
}

void MainWindow::sendData(void) const
{

}

void MainWindow::btChangeDone(QString btAddr, QString btName, QString btIcon, QString majorClass, QString minorClass, bool trusted, QStringList services)
{
    qDebug() <<btAddr << btName << btIcon << majorClass << minorClass << trusted << services;
    sfbtAddr = &btAddr;
    setupBluetoothConn();
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

    naviSubWidget = new QWidget;
    //TODO: construct the navigate widget

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(naviLabel);
    leftLayout->addWidget(naviMenu);
    leftLayout->addStretch();
    QHBoxLayout *naviLayout = new QHBoxLayout;
    naviLayout->addLayout(leftLayout);

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
    connect(testSendButton, SIGNAL(clicked()), this, SLOT(sendData()));
    ctrlSubWidget = dynamic_cast<QWidget *>(testSendButton);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(ctrlLabel);
    leftLayout->addWidget(ctrlMenu);
    leftLayout->addStretch();
    QHBoxLayout *ctrlLayout = new QHBoxLayout;
    ctrlLayout->addLayout(leftLayout);
    ctrlLayout->addStretch();
    ctrlLayout->addWidget(ctrlSubWidget);

    ctrlWidget->setLayout(ctrlLayout);
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

void MainWindow::sendPkt(Packet *buf) const
{

}

Packet *MainWindow::resvPkt() const
{
    return NULL;
}


/*****Static Elements*****/

const QString* const MainWindow::btDbusService = new QString("com.nokia.bt_ui");

const QString* const MainWindow::btDbusInterface = new QString("com.nokia.bt_ui");

const QString* const MainWindow::btDbusPath = new QString("/com/nokia/bt_ui");

const QString* const MainWindow::btSendFileReq = new QString("show_send_file_dlg");

const QString* const MainWindow::btSendFileSig = new QString("send_file");

const QString* const MainWindow::btSearchReq = new QString("show_search_dlg");

const QString* const MainWindow::btSearchSig = new QString("search_result");
