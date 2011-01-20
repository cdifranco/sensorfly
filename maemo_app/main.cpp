#include <QtGui/QApplication>
#include <QtGui/QSplashScreen>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QSplashScreen *splashScn = new QSplashScreen;
    splashScn->setPixmap(QPixmap(":/images/sensorfly.png"));
    splashScn->show();

    Qt::Alignment topRight = Qt::AlignLeft | Qt::AlignTop;
    splashScn->showMessage(QObject::tr("Welcome to Sensorfly world..."), topRight, Qt::white);
    MainWindow mainWin;

    //sleep(1);

#if defined(Q_WS_S60)
    mainWin.showMaximized();
#else
    mainWin.show();
#endif

    splashScn->finish(&mainWin);
    delete splashScn;

    return app.exec();
}

