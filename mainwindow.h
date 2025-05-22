#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Pages/p_about.h"
#include "Pages/p_help.h"
#include "Pages/p_manage.h"
#include "Pages/p_report.h"
#include "Pages/p_setting.h"
#include "Pages/p_timeline.h"
#include "include/ElaWindow.h"
#include "database.h"
#include "Pages/p_home.h"
#include "QVBoxLayout"
#include "ElaDockWidget.h"

class MainWindow : public ElaWindow
{
    Q_OBJECT
private:
    // 数据相关
    Database *db{nullptr};
    QSqlDatabase accountDB;
    bool hasLoggedIn = false;
    User *usr{nullptr};
    QVector<Task> tasks;

    //界面相关
    P_Home *pHome{nullptr};
    P_Manage *pManage{nullptr};
    P_Timeline *pTimeline{nullptr};
    P_Report *pReport{nullptr};
    P_Setting *pSetting{nullptr};
    P_Help *pHelp{nullptr};
    P_About *pAbout{nullptr};

    //Docker
    ElaDockWidget *rDocker{nullptr};
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initWindow();
    void initContent();
    void initDB(QString dbName = "default");
    bool initAccountDB(QSqlError& mess);
    bool Login();
    void Logout();
    void updateUserInfoCard();
    void updateDocker();
};
#endif // MAINWINDOW_H
