#include "database.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
Database::Database(QString dbname):dbName(dbname) {
    initDatabase();
}

Database::~Database(){
}

bool Database::initDatabase(){
    if(QSqlDatabase::contains(dbName)){
        db = QSqlDatabase::database(dbName);
        return false;
    }
    db = QSqlDatabase::addDatabase("QSQLITE", dbName);
    db.setDatabaseName("./data/"+dbName+".db");
    db.setUserName("admin");
    db.setPassword("12345678");

    QString initstr = "CREATE TABLE "+ tbNmae +" ("+
        "TaskID INTEGER PRIMARY KEY AUTOINCREMENT,"+
        "TaskName TEXT NOT NULL,"+
        "IsContinuous INTEGER NOT NULL DEFAULT 0,"+
        "StartTime INTEGER NOT NULL,"+
        "StopTime INTEGER DEFAULT NULL,"+
        "Priority INTEGER NOT NULL DEFAULT 1,"+
        "Tags TEXT DEFAULT ''"+
        ");";

    if(!db.open()){
        qDebug()<<"database open failed!" << db.lastError();
    }else{
        QSqlQuery query(db);
        query.exec("SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' AND name = '"+tbNmae+"'");
        query.next();
        if(query.value(0).toInt()){
            db.close();
            return true;
        }
        qDebug()<<dbName<<" database init";
        bool isok = query.exec(initstr);
        if(!isok) {
            qDebug()<<"initfailed!" << query.lastError();
            return false;
        }
        db.close();
    }
    return true;
}

bool Database::insertTask(const Task& t){
    if(!db.open()){
        qDebug()<<"database open failed!" << db.lastError();
        return false;
    }
    QJsonDocument jdcm;
    QJsonArray jarr;
    for(const auto&p:t.tags){
        jarr.push_back(p);
    }
    jdcm.setArray(jarr);
    QString tags = jdcm.toJson();

    QSqlQuery query(db);
    if(t.isContinuous){
        query.prepare("INSERT INTO "+ tbNmae+
                      "(TaskID,TaskName,IsContinuous,StartTime,StopTime,Priority,Tags) "+
                      +"VALUE (?,?,?,?,?,?,?);");
        query.addBindValue(t.taskID);
        query.addBindValue(t.taskName);
        query.addBindValue(t.isContinuous);
        query.addBindValue(t.startTime.toString());
        query.addBindValue(t.stopTime.toString());
        query.addBindValue(t.priority);
        query.addBindValue(tags);
    }else{
        query.prepare("INSERT INTO "+ QString("Tasks")+
                      "(TaskID,TaskName,IsContinuous,StartTime,Priority,Tags) "+
                      +"VALUE (?,?,?,?,?,?);");
        query.addBindValue(t.taskID);
        query.addBindValue(t.taskName);
        query.addBindValue(t.isContinuous);
        query.addBindValue(t.startTime.toString());
        query.addBindValue(t.priority);
        query.addBindValue(tags);
    }
    bool isok = query.exec();
    if(!isok) {
        qDebug()<<"insertfailed!" << query.lastError();
        return false;
    }
    db.close();
    return true;
}

QVector<Task> Database::queryAllTask(){
    QVector<Task> tasks;
    if(!db.open()){
        qDebug()<<"database open failed!" << db.lastError();
        return tasks;
    }
    QSqlQuery query(db);
    query.exec("SELECT * FROM "+ tbNmae );

    while(query.next()){
        const QJsonArray& qarr = query.value(6).toJsonArray();
        QVector<QString> tags;
        for(const auto& i:qarr){
            tags.push_back(i.toString());
        }
        tasks.push_back({
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toBool(),
            QDateTime::fromString(query.value(3).toString(), "yyyy-MM-dd hh:mm:ss"),
            QDateTime::fromString(query.value(4).toString(), "yyyy-MM-dd hh:mm:ss"),
            query.value(5).toInt(),
            tags}
            );
    }
    return tasks;
}

