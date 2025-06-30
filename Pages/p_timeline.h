#ifndef P_TIMELINE_H
#define P_TIMELINE_H

#include <QVBoxLayout>

#include "ElaScrollPage.h"
#include "../CustomWidgets/CalendarView.h"

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
    CalendarView *calendarWidget{nullptr};
    QString view_type;
    QDate current_date;
    void setView(QString type);
    void updateViewType(QString type);
};

#endif // P_TIMELINE_H
