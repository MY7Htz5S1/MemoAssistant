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
private:
    QVector<Task> &task_list;
    QWidget *widget;
    ElaFlowLayout *layout{nullptr};
    QString view_type;
    void setView(QString type);
};

#endif // P_TIMELINE_H
