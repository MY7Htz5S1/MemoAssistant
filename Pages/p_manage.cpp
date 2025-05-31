#include "p_manage.h"
#include "../CustomWidgets/TaskCard.h"

P_Manage::P_Manage(TaskList& tskList, QWidget* parent):
    ElaScrollPage(parent),
    tasks(tskList) {
    widget = new  QWidget(this);
    setWindowTitle("事项管理");
    setTitleVisible(false);

    updateCards();
}
void P_Manage::updateCards() {
    if(layout!=nullptr) delete layout;
    layout = new ElaFlowLayout(widget);
    layout->setIsAnimation(true);

    for(auto &p:tasks) {
        auto *task = new TaskCard(p,widget);
        layout->addWidget(task);
    }
    widget->setLayout(layout);
    setCustomWidget(widget);
    addCentralWidget(widget);
}
