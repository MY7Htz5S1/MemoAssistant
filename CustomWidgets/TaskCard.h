//
// Created by Keranol on 25-5-29.
//

#ifndef TASKSCARD_H
#define TASKSCARD_H
#include <ElaFlowLayout.h>
#include <ElaLineEdit.h>
#include <ElaText.h>
#include <ElaWidget.h>

#include "ElaToggleSwitch.h"
#include "../publicheader.h"
#include <QGridLayout>
#include <QGroupBox>

#include "ManageTaskWidget.h"

class TaskCard: public QGroupBox {
    Q_OBJECT
private:
    Task task;
    QGridLayout *layout{};
    QVBoxLayout *gLayout{};
    QHBoxLayout *hLayout{};
    ElaCheckBox *finishBox{};
    ElaText *title{};
    ElaText *stars{};
    ElaText *continuousText{};
    ElaToggleSwitch *continuousSwitch{};
    ElaText *startText;
    ElaText *startDT{};
    ElaText *stopText{};
    ElaText *stopDT{};
    ElaFlowLayout *tags{};
    ElaPushButton *deleteButton{};
    ElaPushButton *changeButton{};
    QHBoxLayout *buttons{};

    ManageTaskWidget *taskWidget{};
    ElaWidget *changeWindow{};

public:
    explicit TaskCard(const Task& tsk ,QWidget *parent = nullptr);
    void showCard();
    void changeButtonClicked();
    void deleteButtonClicked();
    void finishBoxStateChanged(bool checked);

    ~TaskCard();
};

#endif //TASKSCARD_H
