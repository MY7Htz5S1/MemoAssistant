#ifndef P_HOME_H
#define P_HOME_H

#include "ElaScrollPage.h"
#include "ElaText.h"
#include <QVBoxLayout>
class P_Home:public ElaScrollPage
{
    Q_OBJECT
private:
    QWidget* widget;
    ElaText *headLine;
    QVBoxLayout *contentLayout;
public:
    Q_INVOKABLE P_Home(QWidget *parent = nullptr);
    void createNewPage();
};

#endif // P_HOME_H
