#include "CalendarView.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QDebug>
#include <QScrollArea>

#include "ElaText.h"
#include "ElaPushButton.h"
#include "ElaIconButton.h"
#include "TaskCard.h"

DayCell::DayCell(const QDate& date, QWidget *parent) : QFrame(parent) {

    setFrameStyle(QFrame::StyledPanel);
    setMinimumSize(90, 60);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget *contentWidget = new QWidget;
    cellLayout = new QVBoxLayout(contentWidget);
    cellLayout->setContentsMargins(10, 8, 8, 10);
    cellLayout->setSpacing(4);

    dateLabel = new QLabel(QString::number(date.day()), contentWidget);
    QFont font = dateLabel->font();
    font.setBold(true);
    font.setPointSize(14);
    dateLabel->setFont(font);
    cellLayout->addWidget(dateLabel, 0, Qt::AlignLeft);

    scrollArea->setWidget(contentWidget);

    mainLayout->addWidget(scrollArea);

    setLayout(mainLayout);
}

void DayCell::setTasks(const QVector<Task>& tasks) {
    for(const Task& t : tasks) {
        QString taskText = QString(t.finished ? "✅ " : "〇 ") + t.taskName;
        auto *taskButton = new ElaPushButton(taskText, this);
        QString color = t.finished ? "white" : "grey";
        taskButton->setStyleSheet(QString("font-size: 13px; color: ") + color + QString("; text-align: left;"));

        connect(taskButton, &ElaPushButton::clicked, this, [=]() {
            auto* taskDetailWidget = new TaskCard(t);
            taskDetailWidget->setWindowModality(Qt::ApplicationModal);
            taskDetailWidget->setWindowFlag(Qt::WindowStaysOnTopHint);
            taskDetailWidget->show();
        });

        cellLayout->addWidget(taskButton);
    }
}

CalendarView::CalendarView(QVector<Task> &tasks, QWidget *parent):
    QWidget(parent), task_list(tasks) {

    this->setMaximumHeight(650);

    current_date = QDate::currentDate();

    mainLayout = nullptr;

    calendarGrid = nullptr;
    headerLayout = nullptr;

}

void CalendarView::buildCalendar(const QDate& month) {

    mainLayout = new QVBoxLayout(this);

    calendarGrid = new QGridLayout();
    headerLayout = new QGridLayout();

    current_date = month;

    initCalendar();

    setLayout(mainLayout);
}

void CalendarView::initCalendar() {

    QString MONTH_NAME[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                              "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    QString DAY_NAME[7] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

    auto *prevMonthButton = new ElaIconButton(ElaIconType::AnglesLeft, 20);
    auto *nextMonthButton = new ElaIconButton(ElaIconType::AnglesRight, 20);

    prevMonthButton->setFixedSize(40, 40);
    nextMonthButton->setFixedSize(40, 40);

    connect(prevMonthButton, &ElaIconButton::clicked, this, &CalendarView::prevMonth);
    connect(nextMonthButton, &ElaIconButton::clicked, this, &CalendarView::nextMonth);

    auto *prevMonthLabel = new ElaText("上个月");
    ElaText *nextMonthLabel = new ElaText("下个月");

    prevMonthLabel->setTextPixelSize(11);
    nextMonthLabel->setTextPixelSize(11);

    ElaText *titleLabel = new ElaText(
        MONTH_NAME[current_date.month()-1] + ' ' + QString::number(current_date.year()), this);
    titleLabel->setTextPixelSize(20);
    QFont font = titleLabel->font();
    font.setPointSize(20);
    font.setBold(true);
    titleLabel->setFont(font);

    headerLayout->addWidget(prevMonthButton, 0, 0);
    headerLayout->setColumnStretch(0, 1);
    headerLayout->addWidget(titleLabel, 0, 2, Qt::AlignCenter);
    headerLayout->setColumnStretch(0, 3);
    headerLayout->addWidget(nextMonthButton, 0, 4);

    prevMonthLabel->setFixedHeight(30);

    headerLayout->addWidget(prevMonthLabel, 1, 0, Qt::AlignCenter);
    headerLayout->addWidget(nextMonthLabel, 1, 4, Qt::AlignCenter);

    int row = 0;

    QDate firstDay = QDate(current_date.year(), current_date.month(), 1);
    int startDayOfWeek = firstDay.dayOfWeek();

    QDate current = firstDay.addDays(-startDayOfWeek + 1);  // 从这个周的周一开始

    for(int col = 0; col < 7; col++) {
        ElaText *nameLabel = new ElaText(" "+DAY_NAME[col], this);
        nameLabel->setFixedHeight(30);
        nameLabel->setTextPixelSize(16);
        calendarGrid->addWidget(nameLabel, row, col);
    }

    bool end_generate = false;

    for(row++; ; row++) {
        for(int col = 0; col < 7; col++) {
            DayCell *cell = new DayCell(current, this);
            bool in_current_month = current.month() == current_date.month();
            if(!in_current_month) cell->setStyleSheet("color: grey;");
            QVector<Task> tasks = tasksForDate(current);
            cell->setTasks(tasks);
            calendarGrid->addWidget(cell, row, col);
            current = current.addDays(1);
            if(in_current_month && current.month() != current_date.month()) // 生成到下一个月了
                end_generate = true;
        }
        if(end_generate) break;
    }

    mainLayout->addLayout(headerLayout);
    mainLayout->addLayout(calendarGrid);

    mainLayout->addSpacing(10);
}

QVector<Task> CalendarView::tasksForDate(const QDate& date) {
    QVector<Task> result;
    for(const Task &t: task_list) {
        if((t.isContinuous && t.stopTime.date() == date) ||
           (!t.isContinuous && t.startTime.date() == date))
            result.push_back(t); // 这天的非连续任务或者结束于这天的连续任务
    }
    return result;
}

void CalendarView::nextMonth() {
    current_date = current_date.addMonths(1);
    refreshCalendar();
}

void CalendarView::prevMonth() {
    current_date = current_date.addMonths(-1);
    refreshCalendar();
}

void CalendarView::taskChanged(Task t, bool& ok) {
    //refreshCalendar();
}

void CalendarView::refreshCalendar() {
    clearLayout(mainLayout);
    buildCalendar(current_date);
}

QSize CalendarView::sizeHint() const {
    //qDebug() << "calendar size:" << QWidget::sizeHint();
    return QWidget::sizeHint();
}

WeekCalendarView::WeekCalendarView(QVector<Task> &tasks, QDate date, QWidget *parent):
    CalendarView(tasks, parent) {

    this->setMaximumHeight(350);
}

void WeekCalendarView::initCalendar() {

    QString MONTH_NAME[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                              "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    QString DAY_NAME[7] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

    auto *prevMonthButton = new ElaIconButton(ElaIconType::AnglesLeft, 20);
    auto *prevWeekButton  = new ElaIconButton(ElaIconType::AngleLeft, 20);
    auto *nextWeekButton  = new ElaIconButton(ElaIconType::AngleRight, 20);
    auto *nextMonthButton = new ElaIconButton(ElaIconType::AnglesRight, 20);

    prevMonthButton->setFixedSize(40, 40);
    prevWeekButton ->setFixedSize(40, 40);
    nextWeekButton ->setFixedSize(40, 40);
    nextMonthButton->setFixedSize(40, 40);

    connect(prevMonthButton, &ElaIconButton::clicked, this, &WeekCalendarView::prevMonth);
    connect(prevWeekButton,  &ElaIconButton::clicked, this, &WeekCalendarView::prevWeek);
    connect(nextWeekButton,  &ElaIconButton::clicked, this, &WeekCalendarView::nextWeek);
    connect(nextMonthButton, &ElaIconButton::clicked, this, &WeekCalendarView::nextMonth);

    ElaText *prevMonthLabel = new ElaText("上个月");
    ElaText *nextMonthLabel = new ElaText("下个月");
    ElaText *nextWeekLabel  = new ElaText("下一周");
    ElaText *prevWeekLabel  = new ElaText("上一周");

    prevMonthLabel->setTextPixelSize(11);
    nextMonthLabel->setTextPixelSize(11);
    prevWeekLabel->setTextPixelSize(11);
    nextWeekLabel->setTextPixelSize(11);

    int startDayOfWeek = current_date.dayOfWeek();

    QDate current = current_date.addDays(-startDayOfWeek + 1);  // 从这个周的周一开始
    QDate endDate = current.addDays(7);

    ElaText *titleLabel = new ElaText(
        MONTH_NAME[current.month()-1] + " " + QString::number(current.day()) + " - " \
        + MONTH_NAME[endDate.month()-1] + " " + QString::number(endDate.day()), this);
    titleLabel->setTextPixelSize(20);
    QFont font = titleLabel->font();
    font.setPointSize(20);
    font.setBold(true);
    titleLabel->setFont(font);

    headerLayout->addWidget(prevMonthButton, 0, 0);
    headerLayout->addWidget(prevWeekButton, 0, 1);
    headerLayout->setColumnStretch(0, 2);
    headerLayout->addWidget(titleLabel, 0, 3, Qt::AlignCenter);
    headerLayout->setColumnStretch(0, 4);
    headerLayout->addWidget(nextWeekButton, 0, 5);
    headerLayout->addWidget(nextMonthButton, 0, 6);

    prevMonthLabel->setFixedHeight(30);

    headerLayout->addWidget(prevMonthLabel, 1, 0, Qt::AlignCenter);
    headerLayout->addWidget(prevWeekLabel, 1, 1, Qt::AlignCenter);
    headerLayout->addWidget(nextWeekLabel, 1, 5, Qt::AlignCenter);
    headerLayout->addWidget(nextMonthLabel, 1, 6, Qt::AlignCenter);

    int row = 0;

    for(int col = 0; col < 7; col++) {
        ElaText *nameLabel = new ElaText(" "+DAY_NAME[col], this);
        nameLabel->setFixedHeight(30);
        nameLabel->setTextPixelSize(16);
        calendarGrid->addWidget(nameLabel, row, col);
    }

    row++;

    for(int col = 0; col < 7; col++) {
        DayCell *cell = new DayCell(current, this);
        QVector<Task> tasks = tasksForDate(current);
        cell->setTasks(tasks);
        cell->setFixedHeight(250);
        calendarGrid->addWidget(cell, row, col);
        current = current.addDays(1);
    }
    mainLayout->addLayout(headerLayout);

    mainLayout->addLayout(calendarGrid);

    mainLayout->addSpacing(10);
}

void WeekCalendarView::prevWeek() {
    current_date = current_date.addDays(-7);
    refreshCalendar();
}

void WeekCalendarView::nextWeek() {
    current_date = current_date.addDays(7);
    refreshCalendar();
}


