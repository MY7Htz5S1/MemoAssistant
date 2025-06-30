//
// Created by Keranol on 25-5-30.
//

#include "ManageTaskWidget.h"

#include <ElaMessageBar.h>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QRegularExpression>

ManageTaskWidget::ManageTaskWidget(QWidget* parent):
    QWidget(parent) {
    isAddMode = true;
    // 初始化一个空的task，确保taskID为-1
    task = Task();
    task.taskID = -1;
    createNewTaskForm();
}

ManageTaskWidget::ManageTaskWidget(const Task& tsk,QWidget* parent):
    QWidget(parent),
    task(tsk){
    isAddMode = false;
    createNewTaskForm();
    readTask(task);
    qDebug()<<"task id:"<<  task.taskID;
}

void ManageTaskWidget::readTask(const Task& tsk) {
    newTaskGroup->setTitle("");
    taskNameEdit->setText(tsk.taskName);
    isContinuousCheck->setChecked(tsk.isContinuous);
    startDateTimePicker->setDateTime(tsk.startTime);
    if(tsk.isContinuous)
        stopDateTimePicker->setDateTime(tsk.stopTime);
    priorityCombo->setCurrentIndex(tsk.priority-1);

    QString tagsString;
    for(const auto& tag : tsk.tags) {
        tagsString += tag;
        if(&tag != &tsk.tags.last()) {
            tagsString += ",";
        }
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
    if (isAddMode){
        // 初始化文本输入框（支持段落输入）
        paragraphInput = new ElaPlainTextEdit(this);
        paragraphInput->setPlaceholderText("请输入任务段落（例：开会，讨论项目进度@明天10点 &紧急 #会议）");
        paragraphInput->setMinimumHeight(150);
        formLayout->addWidget(paragraphInput, row,0,1,0);
        row++;

        // 初始化识别按钮
        recognizeButton = new ElaPushButton("识别任务", this);
        connect(recognizeButton, &QPushButton::clicked, this, &ManageTaskWidget::onParseButtonClicked);
        formLayout->addWidget(recognizeButton);
        row++;
    }

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
    // 检查是否在编辑模式且task有效
    if (!isAddMode && task.taskID != -1) {
        readTask(task);
    } else {
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
void ManageTaskWidget::onTextChanged()
{

}

void ManageTaskWidget::onParseButtonClicked()
{
    parseText(paragraphInput->toPlainText());
    readTask(task);
    update();
}

void ManageTaskWidget::parseText(const QString &text)
{
    // 清空之前的结果
    clearForm();

    // 使用QRegularExpression提取@时间标记
    QRegularExpression timeRegex("@([^@#\\n]+)");
    QRegularExpressionMatch timeMatch = timeRegex.match(text);
    if (timeMatch.hasMatch()) {
        QString timeStr = timeMatch.captured(1).trimmed();
        QDateTime dateTime = parseSmartDateTime(timeStr);

        if (dateTime.isValid()) {
            task.startTime = dateTime;
        }
    }

    // 使用QRegularExpression提取#标签标记
    QRegularExpression tagRegex("#([^@#\\n]+)");
    QStringList tags;
    QRegularExpressionMatchIterator tagIterator = tagRegex.globalMatch(text);
    while (tagIterator.hasNext()) {
        QRegularExpressionMatch tagMatch = tagIterator.next();
        tags.append(tagMatch.captured(1).trimmed());
    }
    task.tags = tags;

    // 使用QRegularExpression提取&标签标记
    QRegularExpression priorityRegex("&([^@#\\n]+)");
    QRegularExpressionMatch priorityMatch = priorityRegex.match(text);
    if (priorityMatch.hasMatch()) {
        QString priorityStr = priorityMatch.captured(1).trimmed();
        task.priority = parsePriority(priorityStr);
    }
    // 提取任务名称（剩余文本）
    QString taskName = text;
    // 移除@时间和#标签
    taskName = taskName.replace(timeRegex, "");
    taskName = taskName.replace(tagRegex, "");
    taskName = taskName.trimmed();
    task.taskName = taskName;
}
QDateTime ManageTaskWidget::parseSmartDateTime(const QString &input)
{
    QDateTime result = QDateTime::currentDateTime();
    QDate currentDate = QDate::currentDate();
    QString text = input.toLower(); // 转换为小写方便处理

    // 检查是否包含日期信息
    bool hasDate = false;

    // 1. 处理绝对日期格式 (yyyy-MM-dd)
    QRegularExpression datePattern("(\\d{4})[-年]?(\\d{1,2})[-月]?(\\d{1,2})日?");
    QRegularExpressionMatch dateMatch = datePattern.match(text);
    if (dateMatch.hasMatch()) {
        int year = dateMatch.captured(1).toInt();
        int month = dateMatch.captured(2).toInt();
        int day = dateMatch.captured(3).toInt();

        if (year < 100) year += 2000; // 处理简写年份

        result.setDate(QDate(year, month, day));
        hasDate = true;
    }

    // 2. 处理相对日期 (今天/明天/后天/周一/...)
    if (!hasDate) {
        if (text.contains("今天")) {
            result.setDate(currentDate);
            hasDate = true;
        } else if (text.contains("明天")) {
            result.setDate(currentDate.addDays(1));
            hasDate = true;
        } else if (text.contains("大后天")) {
            result.setDate(currentDate.addDays(3));
            hasDate = true;
        } else if (text.contains("后天")) {
            result.setDate(currentDate.addDays(2));
            hasDate = true;
        }
        // 处理星期几
        else if (text.contains("周一") || text.contains("礼拜一") || text.contains("星期一")) {
            int daysToAdd = 1 - currentDate.dayOfWeek();
            if (daysToAdd <= 0) daysToAdd += 7;
            result.setDate(currentDate.addDays(daysToAdd));
            hasDate = true;
        } else if (text.contains("周二") || text.contains("礼拜二") || text.contains("星期二")) {
            int daysToAdd = 2 - currentDate.dayOfWeek();
            if (daysToAdd <= 0) daysToAdd += 7;
            result.setDate(currentDate.addDays(daysToAdd));
            hasDate = true;
        } else if (text.contains("周三") || text.contains("礼拜三") || text.contains("星期三")) {
            int daysToAdd = 3 - currentDate.dayOfWeek();
            if (daysToAdd <= 0) daysToAdd += 7;
            result.setDate(currentDate.addDays(daysToAdd));
            hasDate = true;
        } else if (text.contains("周四") || text.contains("礼拜四") || text.contains("星期四")) {
            int daysToAdd = 4 - currentDate.dayOfWeek();
            if (daysToAdd <= 0) daysToAdd += 7;
            result.setDate(currentDate.addDays(daysToAdd));
            hasDate = true;
        } else if (text.contains("周五") || text.contains("礼拜五") || text.contains("星期五")) {
            int daysToAdd = 5 - currentDate.dayOfWeek();
            if (daysToAdd <= 0) daysToAdd += 7;
            result.setDate(currentDate.addDays(daysToAdd));
            hasDate = true;
        } else if (text.contains("周六") || text.contains("礼拜六") || text.contains("星期六")) {
            int daysToAdd = 6 - currentDate.dayOfWeek();
            if (daysToAdd <= 0) daysToAdd += 7;
            result.setDate(currentDate.addDays(daysToAdd));
            hasDate = true;
        } else if (text.contains("周日") || text.contains("礼拜日") || text.contains("星期日") || text.contains("星期天")) {
            int daysToAdd = 7 - currentDate.dayOfWeek();
            if (daysToAdd < 0) daysToAdd += 7;
            result.setDate(currentDate.addDays(daysToAdd));
            hasDate = true;
        }
    }

    // 3. 处理时间部分 (默认今天)
    QTime time = QTime(23, 59); // 默认时间为23:59

    // 处理"HH:mm"格式
    QRegularExpression timePattern1("(\\d{1,2})[:点](\\d{1,2})");
    QRegularExpressionMatch timeMatch1 = timePattern1.match(text);
    if (timeMatch1.hasMatch()) {
        int hour = timeMatch1.captured(1).toInt();
        int minute = timeMatch1.captured(2).toInt();
        time = QTime(hour, minute);
    }
    else if (text.contains("上午") || text.contains("下午")) {
        bool isAfternoon = text.contains("下午");
        QRegularExpression timePattern5("(上午|下午)(\\d{1,2})[点时]");
        QRegularExpressionMatch timeMatch5 = timePattern5.match(text);
        if (timeMatch5.hasMatch()) {
            int hour = timeMatch5.captured(2).toInt();
            if (isAfternoon && hour < 12) hour += 12;
            time = QTime(hour, 0);
        }
    }
    // 处理"HH点"格式
    else if (text.contains("点")) {
        QRegularExpression timePattern2("(\\d{1,2})点");
        QRegularExpressionMatch timeMatch2 = timePattern2.match(text);
        if (timeMatch2.hasMatch()) {
            int hour = timeMatch2.captured(1).toInt();
            time = QTime(hour, 0);
        }
    }
    // 处理"HH时MM分"格式
    else if (text.contains("时") && text.contains("分")) {
        QRegularExpression timePattern3("(\\d{1,2})时(\\d{1,2})分");
        QRegularExpressionMatch timeMatch3 = timePattern3.match(text);
        if (timeMatch3.hasMatch()) {
            int hour = timeMatch3.captured(1).toInt();
            int minute = timeMatch3.captured(2).toInt();
            time = QTime(hour, minute);
        }
    }
    // 处理"HH时"格式
    else if (text.contains("时")) {
        QRegularExpression timePattern4("(\\d{1,2})时");
        QRegularExpressionMatch timeMatch4 = timePattern4.match(text);
        if (timeMatch4.hasMatch()) {
            int hour = timeMatch4.captured(1).toInt();
            time = QTime(hour, 0);
        }
    }
    // 处理"上午/下午 HH点"格式


    // 4. 处理"半小时后"/"1小时30分钟后"等相对时间
    QRegularExpression relativeTimePattern("(\\d+)[小时]?\\s*(\\d*)[分钟]?后");
    QRegularExpressionMatch relativeMatch = relativeTimePattern.match(text);
    if (relativeMatch.hasMatch()) {
        int hours = relativeMatch.captured(1).toInt();
        int minutes = relativeMatch.captured(2).toInt();

        if (minutes == 0 && text.contains("半")) {
            minutes = 30; // 处理"半小时后"
        }

        result = QDateTime::currentDateTime().addSecs(hours * 3600 + minutes * 60);
        return result;
    }

    // 如果没有指定日期，默认为今天
    if (!hasDate) {
        result.setDate(currentDate);
    }

    result.setTime(time);

    // 确保时间在当前时间之后，如果不是则加一天
    if (result < QDateTime::currentDateTime()) {
        result = result.addDays(1);
    }

    return result;
}
int ManageTaskWidget::parsePriority(const QString &text)
{
    // 默认优先级为2
    int priority = 2;

    // 处理"几级"格式（如"1级"、"五级"）
    QRegularExpression levelRegex("([一二三四五1-5])级");
    QRegularExpressionMatch levelMatch = levelRegex.match(text);

    if (levelMatch.hasMatch()) {
        QString levelStr = levelMatch.captured(1);

        // 将中文数字转换为阿拉伯数字
        if (levelStr == "一") priority = 1;
        else if (levelStr == "二") priority = 2;
        else if (levelStr == "三") priority = 3;
        else if (levelStr == "四") priority = 4;
        else if (levelStr == "五") priority = 5;
        else {
            // 直接解析数字
            bool ok;
            int num = levelStr.toInt(&ok);
            if (ok && num >= 1 && num <= 5) {
                priority = num;
            }
        }
    }
    // 处理"高/中/低"优先级
    else if (text.contains("高") || text.contains("紧急")) {
        priority = 5;
    } else if (text.contains("中")) {
        priority = 3;
    } else if (text.contains("低") || text.contains("次要")) {
        priority = 1;
    }

    // 确保优先级在有效范围内
    return qBound(1, priority, 5);
}