#ifndef PUBLICHEADER_H
#define PUBLICHEADER_H

#include <QDateTime>
#include <QVector>
struct Task{
    int taskID;
    QString taskName;
    bool isContinuous;
    QDateTime startTime;
    QDateTime stopTime;
    int priority;
    QVector<QString> tags;
};

struct User{
    int id;
    QString name;
    QString email;
    QString dbName;
};

struct sortByDate{
    using T = Task;
    bool operator()(const T& t1,const T& t2) const{
        if(t1.startTime == t2.startTime){
            return t1.priority > t2.priority;
        }
        return t1.startTime < t2.startTime;
    }
};

struct sortByPriority{
    using T = Task;
    bool operator()(const T& t1, const T& t2) const{
        if(t1.priority == t2.priority){
            return t1.startTime < t2.startTime;
        }
        return t1.priority > t2.priority;
    }
};

struct sortByTags{
    using T = Task;
    bool operator()(const T& t1, const T& t2) const{
        if(t1.tags == t2.tags){
            return t1.priority > t2.priority;
        }
        return t1.tags < t2.tags;
    }
};

#endif // PUBLICHEADER_H
