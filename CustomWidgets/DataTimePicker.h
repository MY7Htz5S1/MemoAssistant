//
// Created by Keranol on 25-5-29.
//

#ifndef DATATIMEPICKER_H
#define DATATIMEPICKER_H
#include "QGroupBox"
#include "ElaCalendarPicker.h"
#include "ElaRoller.h"
#include <QDateTime>
#include <QHBoxLayout>
#include <QGridLayout>

class DataTimePicker:public QGroupBox {
	QGridLayout * layout;
	QHBoxLayout * timeLayout;
	ElaCalendarPicker *elaCalendarPicker;
	ElaRoller *hourRoller;
	ElaRoller *minRoller{};
	ElaRoller *secRoller{};


public:
	explicit DataTimePicker(QWidget *parent = nullptr);
	QDateTime dateTime() const;
	void setDateTime(const QDateTime& dateTime);
};

#endif //DATATIMEPICKER_H
