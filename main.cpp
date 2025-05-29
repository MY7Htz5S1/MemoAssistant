#include "mainwindow.h"
#include "logindialog.h"
#include "include/ElaApplication.h"
#include "include/ElaTheme.h"  // 添加这个头文件
#include "publicheader.h"
#include "Pages/p_docker.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    eApp->init();

    MainWindow w;
    eTheme->setThemeMode(ElaThemeType::Dark);
    w.show();
    return a.exec();
}
