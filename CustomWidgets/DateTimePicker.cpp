//
// Created by Keranol on 25-5-29.
//

#include "DateTimePicker.h"

#include <ElaText.h>

DateTimePicker::DateTimePicker(QWidget *parent):QGroupBox(parent) {
    layout = new QGridLayout(this);
    layout->setSpacing(5);
    layout->setContentsMargins(5, 5, 5, 5);

    timeLayout = new QHBoxLayout();
    layout->setSpacing(5);
    elaCalendarPicker = new ElaCalendarPicker(this);
    QStringList hours;
    for(int i = 0;i<=23;i++) {
        hours.append(QString("%1").arg(i,2,10,QChar('0')));
    }
    hourRoller = new ElaRoller(this);
    hourRoller->setItemList(hours);
    hourRoller->setMaxVisibleItems(3);
    hourRoller->setFixedWidth(50);
    timeLayout->addWidget(hourRoller);
    timeLayout->addWidget(new ElaText(":", 12, this));

    QStringList minutes;
    for(int i = 0;i<=59;i++) {
        minutes.append(QString("%1").arg(i,2,10,QChar('0')));
    }
    minRoller = new ElaRoller(this);
    minRoller->setItemList(minutes);
    minRoller->setMaxVisibleItems(3);
    minRoller->setFixedWidth(50);
    timeLayout->addWidget(minRoller);
    timeLayout->addWidget(new ElaText(":", 12, this));

    QStringList seconds;
    for(int i = 0;i<=59;i++) {
        seconds.append(QString("%1").arg(i,2,10,QChar('0')));
    }
    secRoller = new ElaRoller(this);
    secRoller->setItemList(seconds);
    secRoller->setMaxVisibleItems(3);
    secRoller->setFixedWidth(50);
    timeLayout->addWidget(secRoller);
    layout->addWidget(new ElaText("日期：", 12, this), 0,0,Qt::AlignCenter);
    layout->addWidget(new ElaText("时间：", 12, this), 1,0,Qt::AlignCenter);
    layout->addWidget(elaCalendarPicker,0,1,Qt::AlignLeft);
    layout->addLayout(timeLayout,1, 1, Qt::AlignLeft);
    layout->setColumnStretch(1,2);

    setLayout(layout);
}

QDateTime DateTimePicker::dateTime() const {
    QDate date = elaCalendarPicker->getSelectedDate();
    QTime time(hourRoller->getCurrentData().toInt(),minRoller->getCurrentData().toInt(),secRoller->getCurrentData().toInt());
    return {date,time};
}

void DateTimePicker::setDateTime(const QDateTime& dateTime) {
    elaCalendarPicker->setSelectedDate(dateTime.date());
    hourRoller->setCurrentIndex(dateTime.time().hour());
    minRoller->setCurrentIndex(dateTime.time().minute());
    secRoller->setCurrentIndex(dateTime.time().second());
    update();
}


