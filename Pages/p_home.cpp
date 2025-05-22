#include "p_home.h"

P_Home::P_Home(QWidget* parent):
    ElaScrollPage(parent){
    createNewPage();
}

void P_Home::createNewPage(){
    widget = new QWidget(this);
    contentLayout = new QVBoxLayout(widget);
    setWindowTitle("主页");
    setTitleVisible(false);

    headLine = new ElaText("欢迎使用智能备忘录助手",this);
    headLine->setTextPixelSize(45);
    contentLayout->addWidget(headLine);

    widget->setLayout(contentLayout);
    setCustomWidget(widget);
    addCentralWidget(widget,true,true,0);
}
