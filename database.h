#ifndef DATABASE_H
#define DATABASE_H

#include "publicheader.h"
#include "QtSql/QSqlDatabase"

class Database:public QObject
{
    Q_OBJECT
private:
    QSqlDatabase db;
    QString dbName;
    QString tbNmae = "Tasks";
public:
    Database(QString dbname);
    ~Database();
    bool initDatabase();
    bool insertTask(const Task& t);
    QVector<Task> queryAllTask();
};

#endif // DATABASE_H
