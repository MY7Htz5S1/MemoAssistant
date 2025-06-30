#include "CalendarView.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QDebug>

#include "ElaText.h"
#include "ElaPushButton.h"
#include "ElaIconButton.h"
#include "TaskCard.h"

DayCell::DayCell(const QDate& date, QWidget *parent):
    QFrame(parent) {
    setFrameStyle(QFrame::StyledPanel);
    setMinimumSize(90, 70);  // 可调整

    cellLayout = new QVBoxLayout(this);
    cellLayout->setContentsMargins(10, 8, 8, 10);

    dateLabel = new QLabel(QString::number(date.day()), this);
    QFont font = dateLabel->font();
    font.setBold(true);
    dateLabel->setFont(font);
    cellLayout->addWidget(dateLabel, Qt::AlignLeft);

    setLayout(cellLayout);
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

    current_date = QDate::currentDate();

    mainLayout = nullptr;

    calendarGrid = nullptr;
    headerLayout = nullptr;

    buildCalendar(QDate::currentDate());

    setLayout(mainLayout);
}

void CalendarView::buildCalendar(const QDate& month) {

    QString MONTH_NAME[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                              "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    QString DAY_NAME[7] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

    mainLayout = new QVBoxLayout(this);

    calendarGrid = new QGridLayout();
    headerLayout = new QHBoxLayout();

    current_date = month;

    auto *prevMonthButton = new ElaIconButton(ElaIconType::AngleLeft);
    auto *nextMonthButton = new ElaIconButton(ElaIconType::AngleRight);

    prevMonthButton->setFixedSize(70, 50);
    nextMonthButton->setFixedSize(70, 50);

    connect(prevMonthButton, &ElaIconButton::clicked, this, &CalendarView::prevMonth);
    connect(nextMonthButton, &ElaIconButton::clicked, this, &CalendarView::nextMonth);

    ElaText *titleLabel = new ElaText(
        MONTH_NAME[month.month()-1] + ' ' + QString::number(month.year()), this);
    titleLabel->setTextPixelSize(20);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont font = titleLabel->font();
    font.setPointSize(20);
    font.setBold(true);
    titleLabel->setFont(font);

    headerLayout->addWidget(prevMonthButton);

    headerLayout->addStretch();
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    headerLayout->addWidget(nextMonthButton);

    int row = 0;

    QDate firstDay = QDate(month.year(), month.month(), 1);
    int startDayOfWeek = firstDay.dayOfWeek();

    QDate current = firstDay.addDays(-startDayOfWeek + 1);  // 从这个周的周一开始

    for(int col = 0; col < 7; col++) {
        ElaText *nameLabel = new ElaText(DAY_NAME[col], this);
        nameLabel->setTextPixelSize(15);
        calendarGrid->addWidget(nameLabel, row, col);
    }

    bool end_generate = false;

    for(row++; ; row++) {
        for(int col = 0; col < 7; col++) {
            DayCell *cell = new DayCell(current, this);
            bool in_current_month = current.month() == month.month();
            if(in_current_month) {
                QVector<Task> tasks = tasksForDate(current);
                cell->setTasks(tasks);
            }
            else cell->setStyleSheet("color: grey;");
            calendarGrid->addWidget(cell, row, col);
            current = current.addDays(1);
            if(in_current_month && current.month() != month.month()) // 生成到下一个月了
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



