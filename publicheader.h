#ifndef PUBLICHEADER_H
#define PUBLICHEADER_H

#include <QDateTime>
#include <QVector>
#include <algorithm>

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

enum SortType{
    ByDate,
    ByPriority,
    ByTags
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

inline void sortTasks(QVector<Task> &tsk,SortType st){
    switch(st){
    case ByDate:
        std::sort(tsk.begin(),tsk.end(), sortByDate());
        break;
    case ByPriority:
        std::sort(tsk.begin(),tsk.end(), sortByPriority());
        break;
    case ByTags:
        std::sort(tsk.begin(),tsk.end(), sortByTags());
        break;
    default:
        throw "SortType Error!";
    }

}

#endif // PUBLICHEADER_H
