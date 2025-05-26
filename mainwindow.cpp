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
    pManage = new P_Manage(tasks,this);
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
    this->addPageNode("时间线视图",pTimeline, ElaIconType::Timeline);
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
        accountDB.setDatabaseName("./data/accounts.db");

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
        if(name.isEmpty() || pwd.isEmpty()){
            ld->getFeedback(false);
            ElaMessageBar::error(ElaMessageBarType::BottomRight,"登录失败!","用户名和密码不能为空!",3000);
            return;
        }

        if(!accountDB.open()){
            ElaMessageBar::error(ElaMessageBarType::BottomRight,"账户信息数据库加载失败！",accountDB.lastError().text(),3000);
            ld->getFeedback(false);
            return;
        }

        QSqlQuery query(accountDB);

        // 检查用户是否存在
        query.prepare("SELECT UserID, UserPsw, UserEmail, UserDBName FROM users WHERE UserName = ?");
        query.addBindValue(name);

        if(!query.exec()){
            accountDB.close();
            ld->getFeedback(false);
            ElaMessageBar::error(ElaMessageBarType::BottomRight,"查询失败!",query.lastError().text(),3000);
            return;
        }

        bool userExists = query.next();
        bool loginSuccess = false;

        if(userExists){
            // 用户存在，验证密码
            QString storedPassword = query.value(1).toString();
            if(storedPassword == pwd){
                loginSuccess = true;
                this->hasLoggedIn = true;

                // 获取用户信息
                this->usr = new User;
                this->usr->id = query.value(0).toInt();
                this->usr->name = name;
                this->usr->email = query.value(2).toString();
                this->usr->dbName = query.value(3).toString();

                ElaMessageBar::success(ElaMessageBarType::TopRight,"登录成功！","欢迎回来！",3000);
            } else {
                ElaMessageBar::error(ElaMessageBarType::BottomRight,"登录失败!","密码错误!",3000);
            }
        } else {
            // 用户不存在，创建新用户
            loginSuccess = createNewUser(name, pwd, query);
            if(loginSuccess){
                this->hasLoggedIn = true;
                ElaMessageBar::success(ElaMessageBarType::TopRight,"注册成功！","新用户创建完成，欢迎使用！",3000);
            }
        }

        accountDB.close();
        ld->getFeedback(loginSuccess);

        if(loginSuccess){
            this->initDB(usr->dbName);
        }
    });

    ld->show();
    return hasLoggedIn;
}

bool MainWindow::createNewUser(const QString& name, const QString& pwd, QSqlQuery& query){
    // 生成用户数据库名称（使用用户名）
    QString userDBName = name;

    // 插入新用户到accounts.db
    query.prepare("INSERT INTO users (UserName, UserEmail, UserPsw, UserDBName) VALUES (?, ?, ?, ?)");
    query.addBindValue(name);
    query.addBindValue(""); // 默认空邮箱
    query.addBindValue(pwd);
    query.addBindValue(userDBName);

    if(!query.exec()){
        ElaMessageBar::error(ElaMessageBarType::BottomRight,"用户创建失败!",query.lastError().text(),3000);
        return false;
    }

    // 获取新创建用户的ID
    int newUserID = query.lastInsertId().toInt();

    // 创建用户对象
    this->usr = new User;
    this->usr->id = newUserID;
    this->usr->name = name;
    this->usr->email = "";
    this->usr->dbName = userDBName;

    // 创建用户专属数据库
    if(!createUserDatabase(userDBName)){
        // 如果数据库创建失败，删除刚才插入的用户记录
        query.prepare("DELETE FROM users WHERE UserID = ?");
        query.addBindValue(newUserID);
        query.exec();

        delete this->usr;
        this->usr = nullptr;

        ElaMessageBar::error(ElaMessageBarType::BottomRight,"用户数据库创建失败!","请重试",3000);
        return false;
    }

    return true;
}

bool MainWindow::createUserDatabase(const QString& dbName){
    // 创建用户专属的数据库
    Database* userDB = nullptr;
    try {
        userDB = new Database(dbName);
        delete userDB; // 只是为了初始化数据库，然后删除对象
        return true;
    } catch (...) {
        if(userDB) delete userDB;
        return false;
    }
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
