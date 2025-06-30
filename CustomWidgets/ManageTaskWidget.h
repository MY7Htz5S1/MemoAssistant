//
// Created by Keranol on 25-5-30.
//

#ifndef MANAGETASKWIDGET_H
#define MANAGETASKWIDGET_H

#include <ElaComboBox.h>
#include <ElaPushButton.h>
#include <QTextEdit>

#include "../publicheader.h"
#include <QWidget>
#include "ElaLineEdit.h"
#include "ElaCheckBox.h"
#include "DateTimePicker.h"
#include "ElaText.h"
#include "ElaPlainTextEdit.h"

class ManageTaskWidget:public QWidget {
    Q_OBJECT
    ElaPlainTextEdit *paragraphInput;       // 段落输入框
    ElaPushButton *recognizeButton;         // 识别按钮

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
    explicit ManageTaskWidget(const Task &tsk, QWidget *parent = nullptr);
    void readTask(const Task& tsk);
    signals:
        void taskSaved(Task newTask);
    void taskChangeCancelled();

private slots:
    void onContinuousCheckChanged(int state) const;
    void onSaveButtonClicked();
    void onClearButtonClicked();
    void onTextChanged();
    void onParseButtonClicked();

private:
    void createNewTaskForm();
    void clearForm();
    bool validateInput();
    void parseText(const QString &text);
    QDateTime parseSmartDateTime(const QString &input);
    int parsePriority(const QString &text);
    bool isAddMode = true;
};

#endif //MANAGETASKWIDGET_H
