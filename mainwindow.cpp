#include "mainwindow.h"
#include "QHBoxLayout"
#include "QIcon"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include "ElaMessageBar.h"
#include "logindialog.h"
#include "Pages/p_docker.h"
#include "ElaMenu.h"

MainWindow::MainWindow(QWidget *parent)
    : ElaWindow(parent)
{
    // qDebug() << "Available drivers:" << QSqlDatabase::drivers();
    initWindow();
    initContent();
    if(Login()){
        initDB(usr->name);
    }else{
        initDB();
    }
}

MainWindow::~MainWindow() {}

void MainWindow::initWindow(){
    setWindowIcon(QIcon(":/img/icon.ico"));
    setWindowTitle("智能备忘录助手");
    resize(1200, 740);

    connect(this,&ElaWindow::userInfoCardClicked,this,[=](){
        if(!hasLoggedIn){
            Login();
        }else{
            ElaMenu *userInfoCardMenu = new ElaMenu(this);
            QAction* action = new QAction("登出");
            connect(action,&QAction::toggled,this,&MainWindow::Logout);

            userInfoCardMenu->addAction(action);
        }
    });
}

void MainWindow::initDB(QString dbName){
    if(!db){
        delete db;
    }
    db = new Database(dbName);
    qDebug()<<"open database named " + dbName;
    tasks = db->queryAllTask();
    updateUserInfoCard();
    updateDocker();
}

void MainWindow::initContent(){
    pHome = new P_Home(this);
    pManage = new P_Manage(this);
    pTimeline = new P_Timeline(this);
    pReport = new P_Report(this);
    pSetting = new P_Setting(this);
    QString settingKey;
    pHelp = new P_Help(this);
    QString helpKey;
    pAbout = new P_About(this);
    QString aboutKey;

    this->addPageNode("主页", pHome, ElaIconType::House);
    this->addPageNode("事项管理", pManage, ElaIconType::Book);
    this->addPageNode("时间试图",pTimeline, ElaIconType::Timeline);
    this->addPageNode("近期总结",pReport, ElaIconType::Newspaper);

    this->addFooterNode("关于",pAbout,aboutKey,0,ElaIconType::User);
    this->addFooterNode("帮助",pHelp,helpKey,0,ElaIconType::BlockQuestion);
    this->addFooterNode("设置",pSetting,settingKey,0,ElaIconType::GearComplex);
}


bool MainWindow::initAccountDB(QSqlError& mess){
    if(QSqlDatabase::contains("ACCOUNT")){
        accountDB = QSqlDatabase::database("ACCOUNT");
    }else{
        accountDB = QSqlDatabase::addDatabase("QSQLITE","ACCOUNT");
        accountDB.setDatabaseName("accounts.db");

        if(!accountDB.open()){
            qDebug()<<"accountDB open failed!" << accountDB.lastError();
            mess = accountDB.lastError();
            return false;
        }else{
            QSqlQuery query(accountDB);
            query.exec("SELECT COUNT(*) FROM sqlite_master WHERE type = 'table'AND name = 'users';");
            query.next();
            if(query.value(0).toInt() == 1){
                accountDB.close();
                return true;
            }
            bool isOK = query.exec("CREATE TABLE users("
                "UserID INTEGER PRIMARY KEY AUTOINCREMENT,"
                "UserName TEXT NOT NULL,"
                "UserEmail TEXT DEFAULT '',"
                "UserPsw TEXT DEFAULT '',"
                "UserDBName TEXT NOT NULL);"
            );
            if(!isOK){
                qDebug()<<"init failed!" << query.lastError();
                mess = query.lastError();
                return false;
            }
            accountDB.close();
        }
    }
    return true;
}

bool MainWindow::Login(){
    QSqlError err;
    bool isOk = initAccountDB(err);
    if(!isOk){
        ElaMessageBar::error(ElaMessageBarType::BottomRight,"账户信息数据库初始化失败！",err.text(),3000);
        return false;
    }
    LoginDialog* ld = new LoginDialog(this);
    connect(ld, &LoginDialog::userInfo,this,[=](const QString& name, const QString& pwd){
        bool flag = false;
        if(!accountDB.open()){
            ElaMessageBar::error(ElaMessageBarType::BottomRight,"账户信息数据库加载失败！",err.text(),3000);
            ld->getFeedback(false);
            return;
        }
        QSqlQuery query(accountDB);
        query.exec("SELECT UserPsw FROM users WHERE UserName='"+name+"';");
        while(query.next()){
            if(query.value(0).toString() == pwd){
                flag = true;
                this->hasLoggedIn = true;
            }
        }
        if(!flag){
            ld->getFeedback(false);
            ElaMessageBar::error(ElaMessageBarType::BottomRight,"登录失败!","用户名或密码错误!",3000);
            return;
        }
        query.exec("SELECT UserID,UserEmail,UserDBName FROM users WHERE UserName='"+name+"';");
        while(query.next()){
            this->usr = new User;
            this->usr->id = query.value(0).toInt();
            this->usr->name = name;
            this->usr->email=query.value(1).toString();
            this->usr->dbName = query.value(2).toString();
        }
        ld->getFeedback(true);
        this->initDB(usr->dbName);
        ElaMessageBar::success(ElaMessageBarType::TopRight,"登录成功！","你现在可以使用了",3000);
    });
    ld->show();
    return hasLoggedIn;
}

void MainWindow::Logout(){
    delete usr;
    usr = nullptr;

    initDB();
}

void MainWindow::updateUserInfoCard(){
    if(usr == nullptr){
        setUserInfoCardPixmap(QPixmap(":/img/touxiang.png"));
        setUserInfoCardTitle("请登录");
        setUserInfoCardSubTitle("");
    }else{
        qDebug()<<usr->name << usr->email;
        setUserInfoCardTitle(usr->name);
        setUserInfoCardSubTitle(usr->email);
    }
}

void MainWindow::updateDocker(){
    if(rDocker){
        removeDockWidget(rDocker);
        delete rDocker;
    }
    rDocker = new ElaDockWidget("今日事项", this);
    rDocker->setWidget(new P_Docker(tasks ,this));
    this->addDockWidget(Qt::RightDockWidgetArea, rDocker);
    resizeDocks({rDocker},{300},Qt::Horizontal);
    qDebug()<<tasks.size();
}
