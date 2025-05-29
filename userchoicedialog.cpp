#include "userChoiceDialog.h"
#include <QIcon>

UserChoiceDialog::UserChoiceDialog(const QString& username, QWidget *parent)
    : ElaWidget(parent)
{
    resize(400, 200);
    setWindowTitle("用户不存在");
    setWindowIcon(QIcon(":/img/login.png"));
    setWindowButtonFlags(ElaAppBarType::StayTopButtonHint);
    setWindowButtonFlag(ElaAppBarType::StayTopButtonHint, false);
    setIsFixedSize(true);
    setWindowFlags(windowFlags() | Qt::Tool | Qt::WindowStaysOnTopHint);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setSpacing(15);

    // 标题
    titleText = new ElaText("用户不存在", 24);
    titleText->setAlignment(Qt::AlignCenter);

    // 提示信息
    QString message = QString("用户 \"%1\" 不存在。\n您可以选择：").arg(username);
    messageText = new ElaText(message, 14);
    messageText->setAlignment(Qt::AlignCenter);
    messageText->setWordWrap(true);

    // 按钮布局
    buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    createUserBtn = new ElaPushButton("创建新用户");
    createUserBtn->setFixedSize(120, 35);

    retryBtn = new ElaPushButton("重新输入");
    retryBtn->setFixedSize(120, 35);

    cancelBtn = new ElaPushButton("取消");
    cancelBtn->setFixedSize(120, 35);

    buttonLayout->addWidget(createUserBtn);
    buttonLayout->addWidget(retryBtn);
    buttonLayout->addWidget(cancelBtn);

    // 连接信号
    connect(createUserBtn, &ElaPushButton::clicked, this, [=](){
        emit createNewUser();
        this->close();
    });

    connect(retryBtn, &ElaPushButton::clicked, this, [=](){
        emit retryLogin();
        this->close();
    });

    connect(cancelBtn, &ElaPushButton::clicked, this, [=](){
        emit cancelLogin();
        this->close();
    });

    // 添加到主布局
    mainLayout->addStretch(2);
    mainLayout->addWidget(titleText);
    mainLayout->addWidget(messageText);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch(3);
}
