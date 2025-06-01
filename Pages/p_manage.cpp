#include "p_manage.h"

#include <ElaMessageBar.h>

#include "../CustomWidgets/TaskCard.h"

P_Manage::P_Manage(TaskList& tskList, QWidget* parent):
    ElaScrollPage(parent){

    widget = new QWidget(this);
    setWindowTitle("事项管理");
    setTitleVisible(false);
    layout = new ElaFlowLayout(widget);
    layout->setIsAnimation(true);
    updateCards(tskList);
    setCustomWidget(widget);
    addCentralWidget(widget);
}
void P_Manage::updateCards(TaskList& tasks) {
    while(layout->count()) {
        const auto item = layout->itemAt(0);
        layout->removeItem(item);
        delete item->widget();
    }
    update();
    for(auto &p:tasks) {
        auto *task = new TaskCard(p,widget);
        layout->addWidget(task);
        qDebug()<<"addCard:"<<p.taskName;
    }
    update();
}