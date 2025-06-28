//
// Created by Keranol on 25-5-30.
//

#include "ManageTaskWidget.h"

#include <ElaMessageBar.h>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QDateTime>

ManageTaskWidget::ManageTaskWidget(QWidget* parent):
    QWidget(parent) {
    isAddMode = true;
    createNewTaskForm();
}

ManageTaskWidget::ManageTaskWidget(Task& tsk,QWidget* parent):
    QWidget(parent),
    task(tsk){
    isAddMode = false;
    createNewTaskForm();
    readTask(task);
    qDebug()<<"task id:"<<  task.taskID;
}

void ManageTaskWidget::readTask(Task& tsk) {
    newTaskGroup->setTitle("");
    taskNameEdit->setText(tsk.taskName);
    isContinuousCheck->setChecked(tsk.isContinuous);
    startDateTimePicker->setDateTime(tsk.startTime);
    if(tsk.isContinuous)
        stopDateTimePicker->setDateTime(tsk.stopTime);
    priorityCombo->setCurrentIndex(tsk.priority-1);
    QString tagsString;
    for(auto i:tsk.tags) {
        tagsString+=(i+",");
    }
    tagsEdit->setText(tagsString);
}

void ManageTaskWidget::createNewTaskForm() {
    // 创建新建备忘录分组框
    newTaskGroup = new QGroupBox("新建备忘录", this);
    auto *formLayout = new QGridLayout(newTaskGroup);
    formLayout->setSpacing(15);
    formLayout->setContentsMargins(20, 20, 20, 20);
    formLayout->setRowMinimumHeight(0,50);
    formLayout->setRowMinimumHeight(5,50);

    int row = 0;

    // 任务名称
    auto *nameLabel = new ElaText("任务名称:", newTaskGroup);
    nameLabel->setTextPixelSize(14);
    taskNameEdit = new ElaLineEdit(newTaskGroup);
    taskNameEdit->setPlaceholderText("请输入任务名称");
    formLayout->addWidget(nameLabel, row, 0);
    formLayout->addWidget(taskNameEdit, row, 1, 1, 2);
    row++;

    // 是否持续性任务
    auto *continuousLabel = new ElaText("持续性任务:", newTaskGroup);
    continuousLabel->setTextPixelSize(14);
    isContinuousCheck = new ElaCheckBox("是否为持续性任务", newTaskGroup);
    connect(isContinuousCheck, &ElaCheckBox::checkStateChanged, this, &ManageTaskWidget::onContinuousCheckChanged);
    formLayout->addWidget(continuousLabel, row, 0);
    formLayout->addWidget(isContinuousCheck, row, 1, 1, 2);
    row++;

    // 开始时间
    auto *startLabel = new ElaText("开始时间:",13, newTaskGroup);
    startDateTimePicker = new DateTimePicker(newTaskGroup);
    formLayout->addWidget(startLabel, row, 0,Qt::AlignLeft);
    formLayout->addWidget(startDateTimePicker, row, 1, 1, 2);
    row++;

    // 结束时间（默认禁用）
    stopLabel = new ElaText("结束时间:",13, newTaskGroup);
    stopLabel->setVisible(false);
    stopDateTimePicker = new DateTimePicker(newTaskGroup);
    stopDateTimePicker->setVisible(false);
    formLayout->addWidget(stopLabel, row, 0,Qt::AlignLeft);
    formLayout->addWidget(stopDateTimePicker, row, 1, 1, 2);
    row++;

    // 优先级
    auto *priorityLabel = new ElaText("优先级:", newTaskGroup);
    priorityLabel->setTextPixelSize(14);
    priorityCombo = new ElaComboBox(newTaskGroup);
    priorityCombo->addItem("⭐ 低优先级", 1);
    priorityCombo->addItem("⭐⭐ 中优先级", 2);
    priorityCombo->addItem("⭐⭐⭐ 高优先级", 3);
    priorityCombo->addItem("⭐⭐⭐⭐ 紧急", 4);
    priorityCombo->addItem("⭐⭐⭐⭐⭐ 最高优先级", 5);
    priorityCombo->setCurrentIndex(1); // 默认中优先级
    formLayout->addWidget(priorityLabel, row, 0);
    formLayout->addWidget(priorityCombo, row, 1, 1, 2);
    row++;

    // 标签
    auto *tagsLabel = new ElaText("标签:", newTaskGroup);
    tagsLabel->setTextPixelSize(14);
    tagsEdit = new ElaLineEdit(newTaskGroup);
    tagsEdit->setPlaceholderText("请输入标签，多个标签用逗号分隔");
    formLayout->addWidget(tagsLabel, row, 0);
    formLayout->addWidget(tagsEdit, row, 1, 1, 2);
    row++;

    // 按钮布局
    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    clearButton = new ElaPushButton("清空", newTaskGroup);
    clearButton->setFixedSize(100, 35);
    connect(clearButton, &ElaPushButton::clicked, this, &ManageTaskWidget::onClearButtonClicked);

    saveButton = new ElaPushButton("保存事项", newTaskGroup);
    saveButton->setFixedSize(120, 35);
    connect(saveButton, &ElaPushButton::clicked, this, &ManageTaskWidget::onSaveButtonClicked);

    buttonLayout->addStretch();
    buttonLayout->addWidget(clearButton);
    buttonLayout->addWidget(saveButton);

    formLayout->addLayout(buttonLayout, row, 0, 1, 3);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(newTaskGroup);
    setLayout(mainLayout);
}

void ManageTaskWidget::onContinuousCheckChanged(const int state) const {
    stopLabel->setVisible(state == Qt::Checked);
    stopDateTimePicker->setVisible(state == Qt::Checked);
}

void ManageTaskWidget::onSaveButtonClicked() {
    if (!validateInput()) {
        return;
    }
    // 创建任务对象
    Task newTask(task);
    newTask.taskName = taskNameEdit->text().trimmed();
    newTask.isContinuous = isContinuousCheck->isChecked();
    newTask.startTime = startDateTimePicker->dateTime();
    if (newTask.isContinuous) {
        newTask.stopTime = stopDateTimePicker->dateTime();
    }else {
        newTask.stopTime = newTask.startTime;
    }
    newTask.priority = priorityCombo->currentData().toInt();
    newTask.finished = false;

    // 处理标签
    QString tagsText = tagsEdit->text().trimmed();
    static auto sep = QRegularExpression("[,，]");
    if (!tagsText.isEmpty()) {
        QStringList tagList = tagsText.split(sep, Qt::SkipEmptyParts);
        newTask.tags.clear();
        for (const QString& tag : tagList) {
            newTask.tags.push_back(tag.trimmed());
        }
    }
    emit taskSaved(newTask);
    clearForm();
}

void ManageTaskWidget::onClearButtonClicked() {
    if(task) {
        readTask(task);
    }else {
        clearForm();
    }
    emit taskChangeCancelled();
}

void ManageTaskWidget::clearForm() {
    taskNameEdit->clear();
    isContinuousCheck->setChecked(false);
    startDateTimePicker->setDateTime(QDateTime::currentDateTime());
    stopDateTimePicker->setDateTime(QDateTime::currentDateTime());
    stopDateTimePicker->setVisible(false);
    priorityCombo->setCurrentIndex(1);
    tagsEdit->clear();
}

bool ManageTaskWidget::validateInput() {
    // 验证任务名称
    if (taskNameEdit->text().trimmed().isEmpty()) {
        ElaMessageBar::warning(
            ElaMessageBarType::TopRight,
            "输入错误",
            "请输入任务名称",
            2000
            );
        taskNameEdit->setFocus();
        return false;
    }

    // 验证时间逻辑
    if (isContinuousCheck->isChecked()) {
        if (stopDateTimePicker->dateTime() <= startDateTimePicker->dateTime()) {
            qDebug()<<stopDateTimePicker;
            qDebug()<<"结束时间："<<stopDateTimePicker->dateTime()<<"\n开始时间："<<startDateTimePicker->dateTime();
            ElaMessageBar::warning(
                ElaMessageBarType::TopRight,
                "时间错误",
                "结束时间必须晚于开始时间",
                2000
            );
            stopDateTimePicker->setFocus();
            return false;
        }
    }
    return true;
}