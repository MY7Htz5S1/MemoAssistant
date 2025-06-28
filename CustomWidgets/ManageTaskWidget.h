//
// Created by Keranol on 25-5-30.
//

#ifndef MANAGETASKWIDGET_H
#define MANAGETASKWIDGET_H

#include <ElaComboBox.h>
#include <ElaPushButton.h>
#include "../publicheader.h"
#include <QWidget>
#include "ElaLineEdit.h"
#include "ElaCheckBox.h"
#include "DateTimePicker.h"
#include "ElaText.h"

class ManageTaskWidget:public QWidget {
    Q_OBJECT
    QGroupBox *newTaskGroup{};
    ElaLineEdit *taskNameEdit{};
    ElaCheckBox *isContinuousCheck{};
    DateTimePicker *startDateTimePicker{};
    ElaText *stopLabel{};
    DateTimePicker *stopDateTimePicker{};
    ElaComboBox *priorityCombo{};
    ElaLineEdit *tagsEdit{};
    ElaPushButton *saveButton{};
    ElaPushButton *clearButton{};

    Task task{};
public:
    explicit ManageTaskWidget(QWidget *parent = nullptr);
    explicit ManageTaskWidget(Task &tsk, QWidget *parent = nullptr);
    void readTask(Task& tsk);
signals:
    void taskSaved(Task newTask);
    void taskChangeCancelled();
private slots:
    void onContinuousCheckChanged(int state) const;
    void onSaveButtonClicked();
    void onClearButtonClicked();

private:
    void createNewTaskForm();
    void clearForm();
    bool validateInput();

    bool isAddMode = true;
};

#endif //MANAGETASKWIDGET_H
