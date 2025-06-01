#ifndef P_MANAGE_H
#define P_MANAGE_H

#include <ElaFlowLayout.h>
#include <ElaPushButton.h>
#include <ElaWidget.h>
#include <ElaIconButton.h>
#include <ElaToolTip.h>
#include <QGroupBox>

#include "ElaScrollPage.h"
#include "../publicheader.h"
class P_Manage : public ElaScrollPage
{
    Q_OBJECT
    ElaText *title;
    ElaText *description;

    QVBoxLayout *layout{nullptr};
    QHBoxLayout *textAndButtonLayout{nullptr};
    ElaIconButton *addButton{nullptr};
    ElaToolTip *addButtonToolTip{nullptr};

    typedef QVector<Task> TaskList;
    QWidget* widget{};
    ElaFlowLayout* cardLayout{nullptr};
    QGroupBox *cardGroup{nullptr};

public:
    explicit P_Manage(TaskList& tskList, QWidget* parent = nullptr);
    void updateCards(TaskList& tasks);
    void addNewTask();
};

#endif // P_MANAGE_H
