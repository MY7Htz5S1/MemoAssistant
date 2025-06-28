#include "database.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDir>
#include "TaskManageEventBus.h"
Database::Database(QString dbname):dbName(dbname) {
    initDatabase();
}

Database::~Database(){
    if(db.isOpen()){
        db.close();
    }
}

bool Database::initDatabase(){
    if(QSqlDatabase::contains(dbName)){
        db = QSqlDatabase::database(dbName);
        return true;
    }

    // 确保data目录存在
    QDir dataDir;
    if(!dataDir.exists("./data")){
        dataDir.mkpath("./data");
    }

    db = QSqlDatabase::addDatabase("QSQLITE", dbName);
    db.setDatabaseName("./data/"+dbName+".db");
    QString initstr = "CREATE TABLE "+ tbName +" ("+
                      "TaskID INTEGER PRIMARY KEY AUTOINCREMENT,"+
                      "TaskName TEXT NOT NULL,"+
                      "IsContinuous INTEGER NOT NULL DEFAULT 0,"+
                      "StartTime TEXT NOT NULL,"+
                      "StopTime TEXT DEFAULT NULL,"+
                      "Priority INTEGER NOT NULL DEFAULT 1,"+
                      "Tags TEXT DEFAULT '',"+
                      "Finished BOOL DEFAULT 0"+
                      ");";

    if(!db.open()){
        qDebug()<<"database open failed!" << db.lastError();
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' AND name = ?");
    query.addBindValue(tbName);

    if(!query.exec()){
        qDebug()<<"Query failed!" << query.lastError();
        db.close();
        return false;
    }

    query.next();
    if(query.value(0).toInt() == 0){
        qDebug()<<dbName<<" database init - creating table";
        bool isok = query.exec(initstr);
        if(!isok) {
            qDebug()<<"Table creation failed!" << query.lastError();
            db.close();
            return false;
        }
    }

    db.close();
    return true;
}

bool Database::insertTask(const Task& t, bool& ok){
    if(!db.open()){
        qDebug()<<"database open failed!" << db.lastError();
        ok = false;
        return false;
    }

    QJsonDocument jdcm;
    QJsonArray jarr;
    for(const auto& tag : t.tags){
        jarr.append(tag);
    }
    jdcm.setArray(jarr);
    QString tags = QString::fromUtf8(jdcm.toJson(QJsonDocument::Compact));

    QSqlQuery query(db);
    query.prepare("INSERT INTO "+ tbName +
                      " (TaskName,IsContinuous,StartTime,StopTime,Priority,Finished,Tags) "+
                      "VALUES (?,?,?,?,?,?,?)");
    query.addBindValue(t.taskName);
    query.addBindValue(t.isContinuous ? 1 : 0);
    query.addBindValue(t.startTime.toString("yyyy-MM-dd hh:mm:ss"));
    query.addBindValue(t.stopTime.toString("yyyy-MM-dd hh:mm:ss"));
    query.addBindValue(t.priority);
    query.addBindValue(t.finished);
    query.addBindValue(tags);












    bool isok = query.exec();
    if(!isok) {
        qDebug()<<"Insert failed!" << query.lastError();
        db.close();
        return false;
        ok = false;
    }

    db.close();
    emit tManage->DatabaseChanged();
    ok = true;
    return true;
}

QVector<Task> Database::queryAllTask(){
    QVector<Task> tasks;
    if(!db.open()){
        qDebug()<<"database open failed!" << db.lastError();
        return tasks;
    }

    QSqlQuery query(db);
    query.prepare("SELECT TaskID,TaskName,IsContinuous,StartTime,StopTime,Priority,Tags,Finished FROM " + tbName);

    if(!query.exec()){
        qDebug()<<"Query failed!" << query.lastError();
        db.close();
        return tasks;
    }

    while(query.next()){
        QString tagsJson = query.value(6).toString();
        QVector<QString> tags;

        if(!tagsJson.isEmpty()){
            QJsonDocument doc = QJsonDocument::fromJson(tagsJson.toUtf8());
            if(doc.isArray()){
                QJsonArray qarr = doc.array();
                for(const auto& item : qarr){
                    tags.push_back(item.toString());
                }
            }
        }

        Task task;
        task.taskID = query.value(0).toInt();
        task.taskName = query.value(1).toString();
        task.isContinuous = query.value(2).toBool();
        task.startTime = QDateTime::fromString(query.value(3).toString(), "yyyy-MM-dd hh:mm:ss");

        QString stopTimeStr = query.value(4).toString();
        if(!stopTimeStr.isEmpty()){
            task.stopTime = QDateTime::fromString(stopTimeStr, "yyyy-MM-dd hh:mm:ss");
        }

        task.priority = query.value(5).toInt();
        task.tags = tags;
        task.finished = query.value(7).toBool();

        tasks.push_back(task);
    }

    db.close();
    return tasks;
}

bool Database::updateTask(const Task& t,bool &ok) {
    if (!db.open()) {
        qDebug() << "database open failed!" << db.lastError();
        ok = false;
        return false;
    }

    QJsonDocument jdcm;
    QJsonArray jarr;
    for (const auto& tag : t.tags) {
        jarr.append(tag);
    }
    jdcm.setArray(jarr);
    QString tags = QString::fromUtf8(jdcm.toJson(QJsonDocument::Compact));

    QSqlQuery query(db);

    if (t.isContinuous) {
        query.prepare("UPDATE " + tbName +
                      " SET TaskName = ?, IsContinuous = ?, StartTime = ?, StopTime = ?, Priority = ?, Tags = ? , Finished = ? " +
                      "WHERE TaskID = ?");
        query.addBindValue(t.taskName);
        query.addBindValue(t.isContinuous ? 1 : 0);
        query.addBindValue(t.startTime.toString("yyyy-MM-dd hh:mm:ss"));
        query.addBindValue(t.stopTime.toString("yyyy-MM-dd hh:mm:ss"));
        query.addBindValue(t.priority);
        query.addBindValue(tags);
        query.addBindValue(t.finished? 1 : 0);
        query.addBindValue(t.taskID);
    } else {
        query.prepare("UPDATE " + tbName +
                      " SET TaskName = ?, IsContinuous = ?, StartTime = ?, Priority = ?, Tags = ? , Finished = ? " +
                      "WHERE TaskID = ?");
        query.addBindValue(t.taskName);
        query.addBindValue(t.isContinuous ? 1 : 0);
        query.addBindValue(t.startTime.toString("yyyy-MM-dd hh:mm:ss"));
        query.addBindValue(t.priority);
        query.addBindValue(tags);
        query.addBindValue(t.finished? 1 : 0);
        query.addBindValue(t.taskID);
    }

    bool isok = query.exec();
    if (!isok) {
        qDebug() << "Update failed!" << query.lastError();
        db.close();
        ok = false;
        return false;
    }

    db.close();
    emit tManage->DatabaseChanged();
    qDebug()<<"database updated!"<<t.taskID<<t.taskName;
    ok = true;
    return true;
}

bool Database::deleteTask(const Task& t, bool &ok) {
    if (!db.open()) {
        qDebug() << "database open failed!" << db.lastError();
        ok = false;
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM " + tbName + " WHERE TaskID = ?");
    query.addBindValue(t.taskID);

    bool isok = query.exec();
    if (!isok) {
        qDebug() << "Delete failed!" << query.lastError();
        db.close();
        ok = false;
        return false;
    }

    db.close();
    emit tManage->DatabaseChanged();
    ok = true;
    return true;
}

bool Database::taskFinished(const Task &t, bool &ok) {
    if (!db.open()) {
        qDebug() << "database open failed!" << db.lastError();
        ok = false;
        return false;
    }
    QSqlQuery query(db);
    query.prepare("UPDATE FROM "+tbName + " WHERE TaskID = ? SET Finished = 1");
    query.addBindValue(t.taskID);

    bool isOk = query.exec();
    if(!isOk) {
        qDebug() << "Update Finish failed!" << query.lastError();
        db.close();
        ok = false;
        return false;
    }

    db.close();
    emit tManage->DatabaseChanged();
    ok = true;
    return true;
}