#include "p_home.h"
#include "../mainwindow.h"
#include <QDateTime>
#include "../TaskManageEventBus.h"

P_Home::P_Home(QWidget* parent):
    ElaScrollPage(parent) {
    mainWindow = qobject_cast<MainWindow*>(parent);
    createNewPage();
}

void P_Home::createNewPage() {
    widget = new QWidget(this);
    contentLayout = new QVBoxLayout(widget);
    contentLayout->setSpacing(20);
    setWindowTitle("主页");
    setTitleVisible(false);

    // 欢迎标题
    headLine = new ElaText("欢迎使用智能备忘录助手", this);
    headLine->setTextPixelSize(45);
    contentLayout->addWidget(headLine);

    manageTaskWidget = new ManageTaskWidget(widget); // 创建新的Widget实例
    connect(manageTaskWidget, &ManageTaskWidget::taskSaved, this, &P_Home::handleTaskSaved);
    contentLayout->addWidget(manageTaskWidget);

    contentLayout->addStretch();
    widget->setLayout(contentLayout);
    setCustomWidget(widget);
    addCentralWidget(widget, true, true, 0);

}

void P_Home::handleTaskSaved(const Task &newTask) const {
    bool ok;
    emit tManage->TaskAdded(newTask, ok);
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
}
