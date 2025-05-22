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

    MainWindow w;
    //LoginDialog w;
    // QVector<Task> tsk = {Task{0,
    //                           "complish",
    //                           0,
    //                           QDateTime::fromString("2025-01-01 00:00:00","yyyy-MM-dd hh:mm:ss"),
    //                           QDateTime(),
    //                           5,
    //                           {"math"}
    //                         },
    //                      Task{
    //                           0,
    //                           "finish",
    //                           0,
    //                           QDateTime::fromString("2025-01-03 00:00:00","yyyy-MM-dd hh:mm:ss"),
    //                           QDateTime(),
    //                           8,
    //                           {"physics"}
    //                         }
    //                      };
    // P_Docker w(tsk);
    w.show();
    return a.exec();
}
