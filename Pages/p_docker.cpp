#include "p_docker.h"

#include <ElaPushButton.h>

#include "ElaInteractiveCard.h"
#include "ElaText.h"

P_Docker::P_Docker(QVector<Task>& tsk, QWidget* parent):
    QWidget(parent), tasks(tsk)
{
    text = new ElaPushButton("今天没有事项~", this);
    text->setFixedSize(150,30);
    showTasks(ByDate);
}

void P_Docker::showTasks(SortType st){
    sortTasks(tasks,st);

    flowLayout = new ElaFlowLayout(this,5, 5, 5);
    flowLayout->setIsAnimation(true);
    flowLayout->addWidget(text);
    //ElaReminderCard *card;
    const QDate presentD = QDate::currentDate();
    for(auto &p:tasks){
        if(!((!p.isContinuous&& p.startTime.date() == presentD) ||
            (p.isContinuous&&p.startTime.date()<=presentD &&p.stopTime.date()>=presentD))) {
            continue;
        }
        text->setVisible(false);
        auto* cardGroup = new QWidget(this);
        auto* cardLayout = new QHBoxLayout(cardGroup);

        auto *flag = new QLabel(cardGroup);
        QPixmap flagImg;
        if(p.isContinuous) {
            flagImg = QPixmap(":/img/continuous.png");
        }else {
            flagImg = QPixmap(":/img/uncontinuous.png");
        }
        flag->setPixmap(flagImg.scaled(20,20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        qDebug()<<p.taskName<<p.isContinuous<<p.startTime;

        auto* priorityBox = new QVBoxLayout();
        int n = p.priority;
        priorityBox->addStretch();
        while(n--){
            auto *star = new ElaText("⭐",cardGroup);
            star->setTextPixelSize(10);
            priorityBox->addWidget(star);
        }
        priorityBox->addStretch();
        priorityBox->setSpacing(0);

        auto *card = new ElaInteractiveCard();
        card->setTitle(p.taskName);
        if(!p.isContinuous){
            card->setSubTitle("起始时间："+p.startTime.toString("MM-dd hh:mm")+"\n");
        }else{
            card->setSubTitle("起始时间："+p.startTime.toString("MM-dd hh:mm")+"\n"+
                              "结束时间："+p.stopTime.toString("MM-dd hh:mm"));
        }
        card->setCardPixmap(QPixmap(":/include/Image/Moon.jpg"));
        card->setCardPixmapSize(0,0);
        card->setCardPixMode(ElaCardPixType::RoundedRect);
        card->setTitleSpacing(5);

        cardLayout->addWidget(flag);
        cardLayout->addWidget(card);
        cardLayout->addLayout(priorityBox);

        flowLayout->addWidget(cardGroup);
    }
}
