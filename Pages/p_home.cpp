#include "p_home.h"
#include "../mainwindow.h"
#include <QDateTime>
#include <QGroupBox>

P_Home::P_Home(QWidget* parent):
    ElaScrollPage(parent) {
    mainWindow = qobject_cast<MainWindow*>(parent);
    createNewPage();
}

void P_Home::createNewPage() {
    widget = new QWidget(this);
    contentLayout = new QVBoxLayout(widget);
    contentLayout->setSpacing(20);
    setWindowTitle("主页");
    setTitleVisible(false);

    // 欢迎标题
    headLine = new ElaText("欢迎使用智能备忘录助手", this);
    headLine->setTextPixelSize(45);
    contentLayout->addWidget(headLine);

    // 添加新建备忘录表单
    createNewTaskForm();

    contentLayout->addStretch();
    widget->setLayout(contentLayout);
    setCustomWidget(widget);
    addCentralWidget(widget, true, true, 0);
}

void P_Home::createNewTaskForm() {
    // 创建新建备忘录分组框
    newTaskGroup = new QGroupBox("新建备忘录", widget);
    QGridLayout *formLayout = new QGridLayout(newTaskGroup);
    formLayout->setSpacing(15);
    formLayout->setContentsMargins(20, 20, 20, 20);

    int row = 0;

    // 任务名称
    ElaText *nameLabel = new ElaText("任务名称:", newTaskGroup);
    nameLabel->setTextPixelSize(14);
    taskNameEdit = new ElaLineEdit(newTaskGroup);
    taskNameEdit->setPlaceholderText("请输入任务名称");
    formLayout->addWidget(nameLabel, row, 0);
    formLayout->addWidget(taskNameEdit, row, 1, 1, 2);
    row++;

    // 是否持续性任务
    ElaText *continuousLabel = new ElaText("持续性任务:", newTaskGroup);
    continuousLabel->setTextPixelSize(14);
    isContinuousCheck = new ElaCheckBox("是否为持续性任务", newTaskGroup);
    connect(isContinuousCheck, &ElaCheckBox::stateChanged, this, &P_Home::onContinuousCheckChanged);
    formLayout->addWidget(continuousLabel, row, 0);
    formLayout->addWidget(isContinuousCheck, row, 1, 1, 2);
    row++;

    // 开始时间
    ElaText *startLabel = new ElaText("开始时间:", newTaskGroup);
    startLabel->setTextPixelSize(14);
    startTimeEdit = new QDateTimeEdit(newTaskGroup);
    startTimeEdit->setDateTime(QDateTime::currentDateTime());
    startTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    startTimeEdit->setCalendarPopup(true);
    formLayout->addWidget(startLabel, row, 0);
    formLayout->addWidget(startTimeEdit, row, 1, 1, 2);
    row++;

    // 结束时间（默认禁用）
    ElaText *stopLabel = new ElaText("结束时间:", newTaskGroup);
    stopLabel->setTextPixelSize(14);
    stopTimeEdit = new QDateTimeEdit(newTaskGroup);
    stopTimeEdit->setDateTime(QDateTime::currentDateTime());
    stopTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    stopTimeEdit->setCalendarPopup(true);
    stopTimeEdit->setEnabled(false);
    formLayout->addWidget(stopLabel, row, 0);
    formLayout->addWidget(stopTimeEdit, row, 1, 1, 2);
    row++;

    // 优先级
    ElaText *priorityLabel = new ElaText("优先级:", newTaskGroup);
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
    ElaText *tagsLabel = new ElaText("标签:", newTaskGroup);
    tagsLabel->setTextPixelSize(14);
    tagsEdit = new ElaLineEdit(newTaskGroup);
    tagsEdit->setPlaceholderText("请输入标签，多个标签用逗号分隔");
    formLayout->addWidget(tagsLabel, row, 0);
    formLayout->addWidget(tagsEdit, row, 1, 1, 2);
    row++;

    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    clearButton = new ElaPushButton("清空", newTaskGroup);
    clearButton->setFixedSize(100, 35);
    connect(clearButton, &ElaPushButton::clicked, this, &P_Home::onClearButtonClicked);

    saveButton = new ElaPushButton("保存备忘录", newTaskGroup);
    saveButton->setFixedSize(120, 35);
    connect(saveButton, &ElaPushButton::clicked, this, &P_Home::onSaveButtonClicked);

    buttonLayout->addStretch();
    buttonLayout->addWidget(clearButton);
    buttonLayout->addWidget(saveButton);

    formLayout->addLayout(buttonLayout, row, 0, 1, 3);

    contentLayout->addWidget(newTaskGroup);
}

void P_Home::onContinuousCheckChanged(int state) {
    stopTimeEdit->setEnabled(state == Qt::Checked);
}

void P_Home::onSaveButtonClicked() {
    if (!validateInput()) {
        return;
    }

    // 检查数据库是否可用
    if (!mainWindow || !mainWindow->getDatabase()) {
        ElaMessageBar::error(
            ElaMessageBarType::TopRight,
            "数据库错误",
            "无法连接到数据库，请重试",
            2000
            );
        return;
    }

    // 创建任务对象
    Task newTask;
    newTask.taskName = taskNameEdit->text().trimmed();
    newTask.isContinuous = isContinuousCheck->isChecked();
    newTask.startTime = startTimeEdit->dateTime();

    if (newTask.isContinuous) {
        newTask.stopTime = stopTimeEdit->dateTime();
    }

    newTask.priority = priorityCombo->currentData().toInt();

    // 处理标签
    QString tagsText = tagsEdit->text().trimmed();
    if (!tagsText.isEmpty()) {
        QStringList tagList = tagsText.split(",", Qt::SkipEmptyParts);
        for (const QString& tag : tagList) {
            newTask.tags.push_back(tag.trimmed());
        }
    }

    // 保存到数据库
    Database* db = mainWindow->getDatabase();
    if (db && db->insertTask(newTask)) {
        ElaMessageBar::success(
            ElaMessageBarType::TopRight,
            "保存成功",
            "备忘录已成功创建",
            2000
            );

        // 清空表单
        clearForm();

        // 更新任务列表和Docker
        mainWindow->initDB(mainWindow->getCurrentDatabaseName());
    } else {
        ElaMessageBar::error(
            ElaMessageBarType::TopRight,
            "保存失败",
            "无法保存备忘录，请重试",
            2000
            );
    }
}

void P_Home::onClearButtonClicked() {
    clearForm();
}

void P_Home::clearForm() {
    taskNameEdit->clear();
    isContinuousCheck->setChecked(false);
    startTimeEdit->setDateTime(QDateTime::currentDateTime());
    stopTimeEdit->setDateTime(QDateTime::currentDateTime());
    stopTimeEdit->setEnabled(false);
    priorityCombo->setCurrentIndex(1);
    tagsEdit->clear();
}

bool P_Home::validateInput() {
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
        if (stopTimeEdit->dateTime() <= startTimeEdit->dateTime()) {
            ElaMessageBar::warning(
                ElaMessageBarType::TopRight,
                "时间错误",
                "结束时间必须晚于开始时间",
                2000
                );
            stopTimeEdit->setFocus();
            return false;
        }
    }

    return true;
}
