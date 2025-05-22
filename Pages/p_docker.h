#ifndef P_DOCKER_H
#define P_DOCKER_H

#include <QWidget>
#include "../publicheader.h"
#include "ElaFlowLayout.h"

class P_Docker : public QWidget
{
    Q_OBJECT
private:
    QVector<Task>& tasks;

    ElaFlowLayout *flowLayout;
public:
    P_Docker(QVector<Task>& tsk, QWidget *parent = nullptr);
    void showTasks();
};

#endif // P_DOCKER_H
