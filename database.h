#ifndef DATABASE_H
#define DATABASE_H

#include "publicheader.h"
#include "QtSql/QSqlDatabase"

class Database : public QObject
{
    Q_OBJECT
private:
    QSqlDatabase db;
    QString dbName;
    QString tbName = "Tasks";  // 修复拼写错误：tbNmae -> tbName

public:
    Database(QString dbname);
    ~Database();
    bool initDatabase();
    bool insertTask(const Task& t, bool& ok);
    QVector<Task> queryAllTask();

    // 新增一些有用的方法
    bool updateTask(const Task& t, bool& ok);
    bool deleteTask(const Task& t, bool& ok);
    QVector<Task> queryTasksByDate(const QDate& date);
    bool isConnected() const { return db.isOpen(); }

};

#endif // DATABASE_H
