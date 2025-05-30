#ifndef P_HOME_H
#define P_HOME_H

#include "ElaScrollPage.h"
#include "ElaText.h"
#include "ElaLineEdit.h"
#include "ElaMessageBar.h"
#include "../publicheader.h"
#include <QDateTimeEdit>

#include "../CustomWidgets/ManageTaskWidget.h"

class MainWindow;

class P_Home : public ElaScrollPage
{
    Q_OBJECT
    QWidget* widget{};
    ElaText *headLine{};
    QVBoxLayout *contentLayout{};
    ManageTaskWidget *manageTaskWidget{}; // 使用新的Widget
    MainWindow *mainWindow;

public:
    explicit P_Home(QWidget *parent = nullptr);
    void createNewPage();

    private slots:
        void handleTaskSaved(const Task &newTask) const; // 处理新任务保存的槽函数

signals:
    void databaseChanged() const;
};

#endif // P_HOME_H