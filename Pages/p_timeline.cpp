#include "p_timeline.h"
#include "ElaMessageBar.h"
#include "ElaRadioButton.h"
#include "ElaText.h"
#include <QDebug>

P_Timeline::P_Timeline(QVector<Task> &tasks, QWidget* parent):
    ElaScrollPage(parent),
    task_list(tasks),
    view_type("Calendar") {

    widget = new QWidget(this);
    setWindowTitle("时间视图");
    setTitleVisible(false);

    current_date = QDate::currentDate();

    mainLayout = new QVBoxLayout(widget);

    auto monthButton = new ElaRadioButton("月视图", this);
    auto weekButton = new ElaRadioButton("周视图", this);

    monthButton->setChecked(true);
    QHBoxLayout* viewTypeLayout = new QHBoxLayout();

    ElaText* viewTypeText = new ElaText("日历视图选择", this);
    viewTypeText->setWordWrap(false);
    viewTypeText->setTextPixelSize(15);
    viewTypeLayout->addWidget(viewTypeText);
    viewTypeLayout->addStretch();
    viewTypeLayout->addWidget(monthButton);
    viewTypeLayout->addWidget(weekButton);

    connect(monthButton, &ElaRadioButton::clicked, this, [=]() {
        updateViewType("Calendar");
    });

    connect(weekButton, &ElaRadioButton::clicked, this, [=]() {
        updateViewType("WeekCalendar");
    });

    mainLayout->addLayout(viewTypeLayout);

    setView(view_type);

    current_date = calendarWidget->current_date;

    widget->setLayout(mainLayout);

    setCustomWidget(widget);
    addCentralWidget(widget);
}

void P_Timeline::setView(QString type) {
    view_type = type;

    if(type == "Calendar") {
        calendarWidget = new CalendarView(task_list);
        calendarWidget->buildCalendar(current_date);
    }
    else if(type == "WeekCalendar") {
        calendarWidget = new WeekCalendarView(task_list, current_date);
        calendarWidget->buildCalendar(current_date);
    }
    else if(type == "Timeline") {
        ElaMessageBar::error(
            ElaMessageBarType::BottomRight, "暂不支持!", "", 3000);
        return;
    }
    else {
        ElaMessageBar::error(
            ElaMessageBarType::BottomRight, "不支持的视图!", "", 3000);
        return;
    }

    if(mainLayout) mainLayout->addWidget(calendarWidget);
}

void P_Timeline::updateTimeline(QVector<Task> &tasks) {
    task_list = tasks;
    mainLayout->removeWidget(calendarWidget);
    if(calendarWidget) {
        delete calendarWidget;
        calendarWidget = nullptr;
    }
    setView(view_type);
}

void P_Timeline::updateViewType(QString type) {
    qDebug() << "Change view type to" << type;
    mainLayout->removeWidget(calendarWidget);
    if(calendarWidget) {
        delete calendarWidget;
        calendarWidget = nullptr;
    }
    setView(type);
}

