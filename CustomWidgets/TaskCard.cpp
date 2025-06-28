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
    int row = 0;

    title = new ElaText(task.taskName, 25, this);
    layout->addWidget(title,row,0,1,0,Qt::AlignLeft);
    row++;

    QString starString{};
    for(int i = 0;i<task.priority;i++) {
        starString+="⭐";
    }
    stars = new ElaText(starString, 15, this);
    layout->addWidget(stars,row,0,1,0,Qt::AlignLeft);
    row++;

    continuousText = new ElaText("连续事件:",15,this);
    layout->addWidget(continuousText,row,0,Qt::AlignLeft);

    continuousSwitch = new ElaToggleSwitch(this);
    continuousSwitch->setIsToggled(task.isContinuous);
    continuousSwitch->setEnabled(false);
    layout->addWidget(continuousSwitch,row,1,Qt::AlignLeft);
    row++;

    startText = new ElaText("开始于:",15,this);
    layout->addWidget(startText,row,0,Qt::AlignLeft);

    startDT = new ElaText(this);
    startDT->setText(task.startTime.toString("yyyy-MM-dd hh:mm:ss"));
    startDT->setTextPixelSize(15);
    //startDT->setFocusPolicy(Qt::NoFocus);
    layout->addWidget(startDT,row,1,Qt::AlignCenter);
    row++;

    if(task.isContinuous) {
        stopText = new ElaText("结束于:",15,this);
        layout->addWidget(stopText,row,0,Qt::AlignLeft);

        stopDT = new ElaText(this);
        stopDT->setText(task.stopTime.toString("yyyy-MM-dd hh:mm:ss"));
        stopDT->setTextPixelSize(15);
        layout->addWidget(stopDT,row,1,Qt::AlignCenter);
        row++;
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
        layout->addLayout(tags,row,0,1,0, Qt::AlignCenter);
        row++;
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
    layout->addLayout(buttons,row,0,1,0, Qt::AlignCenter);

    layout->setColumnStretch(2,5);
    connect(changeButton,&ElaPushButton::clicked,this,&TaskCard::changeButtonClicked);
    connect(deleteButton,&ElaPushButton::clicked,this,&TaskCard::deleteButtonClicked);
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
    });
    changeWindow->show();
}

void TaskCard::deleteButtonClicked() {
    bool ok;
    emit tManage->TaskDeleted(task,ok);
    if(ok) {
        ElaMessageBar::success(ElaMessageBarType::TopRight,"删除成功","",3000);
    }else {
        ElaMessageBar::error(ElaMessageBarType::TopRight,"删除失败","",3000);
    }
}




