#include "mainwindow.h"
#include "logindialog.h"
#include "include/ElaApplication.h"
#include "publicheader.h"
#include "Pages/p_docker.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    eApp->init();
    eApp->setIsEnableMica(true);

    MainWindow w;

    w.show();
    return a.exec();
}
