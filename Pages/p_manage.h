#ifndef P_MANAGE_H
#define P_MANAGE_H

#include <ElaFlowLayout.h>
#include <ElaWidget.h>

#include "ElaScrollPage.h"
#include "../publicheader.h"
class P_Manage : public ElaScrollPage
{
    Q_OBJECT
    typedef QVector<Task> TaskList;
    TaskList& tasks;
    QWidget* widget;
    ElaFlowLayout* layout{nullptr};

public:
    explicit P_Manage(TaskList& tskList, QWidget* parent = nullptr);
    void updateCards();
};

#endif // P_MANAGE_H
