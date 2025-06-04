#include "p_manage.h"

#include <ElaMessageBar.h>
#include <filesystem>

#include "../CustomWidgets/TaskCard.h"
#include "../TaskManageEventBus.h"

P_Manage::P_Manage(TaskList& tskList, QWidget* parent):
    ElaScrollPage(parent){

    widget = new QWidget(this);
    setWindowTitle("事项管理");
    setTitleVisible(false);

    layout = new QVBoxLayout(widget);
    layout->setSpacing(10);
    title = new ElaText("事项管理",35,this);
    title->setFixedHeight(35);
    layout->addWidget(title);

    layout->addSpacing(5);

    textAndButtonLayout = new QHBoxLayout();
    description = new ElaText("下方是你所有的事项",15,this);
    description->setFixedHeight(25);
    textAndButtonLayout->addWidget(description);
    textAndButtonLayout->addStretch();

    addButton = new ElaIconButton(ElaIconType::CirclePlus,20,20,20,this);
    connect(addButton,&ElaIconButton::clicked,this,&P_Manage::addNewTask);
    textAndButtonLayout->addWidget(addButton);
    addButtonToolTip = new ElaToolTip(addButton);
    addButtonToolTip->setToolTip("添加新事项");
    layout->addLayout(textAndButtonLayout);
    layout->addSpacing(5);


    cardGroup = new QGroupBox(widget);
    cardLayout = new ElaFlowLayout(cardGroup);
    cardLayout->setIsAnimation(true);
    updateCards(tskList);

    layout->addWidget(cardGroup);
    layout->addStretch();
    setCustomWidget(widget);
    addCentralWidget(widget);
}
void P_Manage::updateCards(TaskList& tasks) {
    while(cardLayout->count()) {
        const auto item = cardLayout->itemAt(0);
        cardLayout->removeItem(item);
        delete item->widget();
    }
    for(auto &p:tasks) {
        auto *task = new TaskCard(p,widget);
        cardLayout->addWidget(task);
        qDebug()<<"addCard:"<<p.taskName;
    }
    if(tasks.size() == 0) {
        auto *text = new ElaPushButton("今天没有事项~,点击新建事项", cardGroup);
        connect(text,&ElaPushButton::clicked,this,&P_Manage::addNewTask);
        cardLayout->addWidget(text);
    }
    update();
}

void P_Manage::addNewTask() {
    auto manageTaskWidget = new ManageTaskWidget(); // 创建新的Widget实例
    manageTaskWidget->setWindowModality(Qt::ApplicationModal);
    manageTaskWidget->setWindowFlag(Qt::WindowStaysOnTopHint);
    connect(manageTaskWidget, &ManageTaskWidget::taskSaved, this, [=] (const Task& t){
        manageTaskWidget->close();
        bool ok;
        emit tManage->TaskAdded(t, ok);
        if (ok) {
            // 更新任务列表和Docker
            //mainWindow->initDB(mainWindow->getCurrentDatabaseName());
            ElaMessageBar::success(
                ElaMessageBarType::TopRight,
                "保存成功",
                "备忘录已成功创建",
                2000
                );
        } else {
            ElaMessageBar::error(
                ElaMessageBarType::TopRight,
                "保存失败",
                "无法保存备忘录，请重试",
                2000
                );
        }
    });
    manageTaskWidget->show();
}