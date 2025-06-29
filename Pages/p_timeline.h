#ifndef P_TIMELINE_H
#define P_TIMELINE_H

#include <QVBoxLayout>

#include "ElaScrollPage.h"

#include "../publicheader.h"
#include <ElaFlowLayout.h>
#include <ElaWidget.h>


class P_Timeline : public ElaScrollPage
{
    Q_OBJECT
public:
    P_Timeline(QVector<Task> &tasks, QWidget* parent = nullptr);
    void updateTimeline(QVector<Task> &tasks);
private:
    QVector<Task> &task_list;
    QWidget *widget;
    QVBoxLayout *mainLayout{nullptr};
    QWidget *calendarWidget{nullptr};
    QString view_type;
    void setView(QString type);
    void updateViewType(QString type);
};

#endif // P_TIMELINE_H
