#ifndef P_HOME_H
#define P_HOME_H

#include "ElaScrollPage.h"
#include "ElaText.h"
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaComboBox.h"
#include "ElaCheckBox.h"
#include "ElaMessageBar.h"
#include "../database.h"
#include "../publicheader.h"
#include <QDateTimeEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>

class MainWindow;

class P_Home : public ElaScrollPage
{
    Q_OBJECT
private:
    QWidget* widget;
    ElaText *headLine;
    QVBoxLayout *contentLayout;

    // 新建备忘录表单组件
    QGroupBox *newTaskGroup;
    ElaLineEdit *taskNameEdit;
    ElaCheckBox *isContinuousCheck;
    QDateTimeEdit *startTimeEdit;
    QDateTimeEdit *stopTimeEdit;
    ElaComboBox *priorityCombo;
    ElaLineEdit *tagsEdit;
    ElaPushButton *saveButton;
    ElaPushButton *clearButton;

    MainWindow *mainWindow;

public:
    P_Home(QWidget *parent = nullptr);
    void createNewPage();

private slots:
    void onContinuousCheckChanged(int state);
    void onSaveButtonClicked();
    void onClearButtonClicked();

private:
    void createNewTaskForm();
    void clearForm();
    bool validateInput();
};

#endif // P_HOME_H
