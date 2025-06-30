//
// Created by Keranol on 25-5-29.
//

#include "TaskCard.h"

#include <ElaMessageBar.h>
#include <ElaPushButton.h>
#include <ElaWidget.h>

#include <utility>

#include "../TaskManageEventBus.h"

TaskCard::TaskCard(const Task& tsk, QWidget *parent):
    QGroupBox(parent),
    task(tsk) {
    showCard();
}

void TaskCard::showCard() {
    layout = new QGridLayout(this);
    layout->setSpacing(10);
    layout->setContentsMargins(10,10,10,10);
    int row1 = 0;
    int row2 = 0;
    gLayout = new QGridLayout;
    gLayout->setSpacing(10);

    title = new ElaText(task.taskName, 25, this);
    gLayout->addWidget(title,row2,0,Qt::AlignLeft);
    row2++;

    QString starString{};
    for(int i = 0;i<task.priority;i++) {
        starString+="⭐";
    }
    stars = new ElaText(starString, 15, this);
    gLayout->addWidget(stars,row2, 0, Qt::AlignLeft);

    layout->addLayout(gLayout,row1,0,1,0,Qt::AlignLeft);
    finishBox = new ElaCheckBox("已完成",this);
    finishBox->setChecked(task.finished);
    connect(finishBox,&ElaCheckBox::toggled,this,&TaskCard::finishBoxStateChanged);
    gLayout->addWidget(finishBox,0,1,0,2,Qt::AlignCenter);
    row1++;

    continuousText = new ElaText("连续事件:",15,this);
    layout->addWidget(continuousText,row1,0,Qt::AlignLeft);
    continuousSwitch = new ElaToggleSwitch(this);
    continuousSwitch->setIsToggled(task.isContinuous);
    continuousSwitch->setEnabled(false);
    layout->addWidget(continuousSwitch,row1,1,Qt::AlignLeft);
    row1++;

    startText = new ElaText("开始于:",15,this);
    layout->addWidget(startText,row1,0,Qt::AlignLeft);
    startDT = new ElaText(this);
    startDT->setText(task.startTime.toString("yyyy-MM-dd hh:mm:ss"));
    startDT->setTextPixelSize(15);
    //startDT->setFocusPolicy(Qt::NoFocus);
    layout->addWidget(startDT,row1,1,Qt::AlignCenter);
    row1++;

    if(task.isContinuous) {
        stopText = new ElaText("结束于:",15,this);
        layout->addWidget(stopText,row1,0,Qt::AlignLeft);

        stopDT = new ElaText(this);
        stopDT->setText(task.stopTime.toString("yyyy-MM-dd hh:mm:ss"));
        stopDT->setTextPixelSize(15);
        layout->addWidget(stopDT,row1,1,Qt::AlignCenter);
        row1++;
    }
    if(!task.tags.empty()) {
        tags = new ElaFlowLayout();
        for(auto& t:task.tags) {
            auto *tag = new QPushButton("#"+t,this);
            //connect(tag,&QPushButton::clicked,this,[=](){qDebug()<<tag->text();});
            tag->setFixedHeight(30);
            tag->setMinimumWidth(50);
            uint hash = qHash(tag->text());
            QColor color = baseColors[hash%baseColors.size()];
            tag->setStyleSheet(QString("border-radius: 15px; "
                            "padding: 5px;"
                           "background-color: rgba(%1, %2, %3, %4); ")
                           .arg(color.red())
                           .arg(color.green())
                           .arg(color.blue())
                           .arg(color.alpha()));  // 半透明背景
            tags->addWidget(tag);
        }
        layout->addLayout(tags,row1,0,1,0, Qt::AlignCenter);
        row1++;
    }

    buttons = new QHBoxLayout();
    buttons->setSpacing(10);
    buttons->setContentsMargins(10,10,10,10);
    buttons->addStretch();
    deleteButton = new ElaPushButton("删除",this);
    buttons->addWidget(deleteButton);
    buttons->addStretch();
    changeButton = new ElaPushButton("修改",this);
    buttons->addWidget(changeButton);
    buttons->addStretch();
    layout->addLayout(buttons,row1,0,1,0, Qt::AlignCenter);

    layout->setColumnStretch(2,5);
    connect(changeButton,&ElaPushButton::clicked,this,&TaskCard::changeButtonClicked);
    connect(deleteButton,&ElaPushButton::clicked,this,&TaskCard::deleteButtonClicked);
    row1++;
}


void TaskCard::changeButtonClicked() {
    changeWindow = new ElaWidget();
    changeWindow->setWindowTitle("修改事项");
    changeWindow->setWindowButtonFlag(ElaAppBarType::StayTopButtonHint, false);
    changeWindow->setWindowFlag(Qt::WindowStaysOnTopHint);
    changeWindow->setWindowModality(Qt::ApplicationModal);

    auto *changeWindowLayout = new QHBoxLayout(changeWindow);
    taskWidget = new ManageTaskWidget(task,changeWindow);
    changeWindowLayout->addWidget(taskWidget);
    connect(taskWidget,&ManageTaskWidget::taskSaved,this,[=](const Task& t) {
        changeWindow->close();
        bool ok = false;
        emit tManage->TaskChanged(t,ok);
        if(ok) {
            ElaMessageBar::success(ElaMessageBarType::TopRight,"修改成功","",3000);
        }else {
            ElaMessageBar::error(ElaMessageBarType::TopRight,"修改失败","",3000);
        }
        this->close();
    });
    changeWindow->show();
}


void TaskCard::deleteButtonClicked() {
    bool ok = false;
    emit tManage->TaskDeleted(task, ok);
    if(ok) {
        ElaMessageBar::success(ElaMessageBarType::TopRight, "删除成功", "", 3000);
        // 不要在这里直接删除TaskCard，让父窗口通过数据库更新来处理
        // 数据库更新会触发 databaseChangedSlot，进而更新界面
    } else {
        ElaMessageBar::error(ElaMessageBarType::TopRight, "删除失败", "", 3000);
    }
}

void TaskCard::finishBoxStateChanged(const bool checked) {
    bool ok = false;
    emit tManage->TaskFinished(task,checked,ok);
    if(ok) {
        ElaMessageBar::success(ElaMessageBarType::TopRight, "更新成功", "", 3000);
    } else {
        ElaMessageBar::error(ElaMessageBarType::TopRight, "更新失败", "", 3000);
    }
}

TaskCard::~TaskCard() {
    if (changeWindow && !changeWindow->isHidden()) {
        changeWindow->close();
    }
}
