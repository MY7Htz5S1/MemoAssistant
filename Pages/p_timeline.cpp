#include "p_timeline.h"
#include "ElaMessageBar.h"
#include "../CustomWidgets/CalendarView.h"

P_Timeline::P_Timeline(QVector<Task> &tasks, QWidget* parent):
    ElaScrollPage(parent),
    task_list(tasks),
    view_type("Calendar") {

    widget = new QWidget(this);
    setWindowTitle("时间视图");
    setTitleVisible(false);

    setView(view_type);
}

void P_Timeline::setView(QString type) {

    if(layout != nullptr) delete layout;
    layout = new ElaFlowLayout(widget);
    layout->setIsAnimation(true);

    if(type == "Calendar") {
        auto calendar = new CalendarView(task_list, widget);
        layout->addWidget(calendar);
    }
    else if(type == "Timelist") {

    }
    else {
        ElaMessageBar::error(
            ElaMessageBarType::BottomRight, "不支持的视图!", "", 3000);
        return;
    }

    widget->setLayout(layout);
    setCustomWidget(widget);
    addCentralWidget(widget);
}


//void CalendarChanged()
