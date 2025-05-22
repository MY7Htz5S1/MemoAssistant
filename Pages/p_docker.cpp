#include "p_docker.h"
#include "algorithm"
#include "ElaInteractiveCard.h"
#include "ElaText.h"
#include "ElaReminderCard.h"
P_Docker::P_Docker(QVector<Task>& tsk, QWidget* parent):
    QWidget(parent), tasks(tsk)
{
    showTasks();
}

void P_Docker::showTasks(){

    std::sort(tasks.begin(),tasks.end(),sortByDate());
    flowLayout = new ElaFlowLayout(this,0, 5, 5);
    flowLayout->setIsAnimation(true);
    //ElaReminderCard *card;
    ElaInteractiveCard *card;
    for(auto &p:tasks){
        QWidget* cardGroup = new QWidget(this);
        QHBoxLayout*cardLayout = new QHBoxLayout(cardGroup);
        QVBoxLayout*priorityBox = new QVBoxLayout(cardGroup);

        qDebug()<<p.taskName<<p.isContinuous<<p.startTime;
        int n = p.priority;
        priorityBox->addStretch();
        while(n--){
            ElaText *star = new ElaText("⭐",cardGroup);
            star->setTextPixelSize(10);
            priorityBox->addWidget(star);
        }
        priorityBox->addStretch();
        priorityBox->setSpacing(0);

        card = new ElaInteractiveCard(this);
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

        cardLayout->addWidget(card);
        cardLayout->addLayout(priorityBox);

        flowLayout->addWidget(cardGroup);
    }
}
