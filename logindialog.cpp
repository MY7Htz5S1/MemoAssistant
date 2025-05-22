#include "logindialog.h"

LoginDialog::LoginDialog(QWidget* parent):
    ElaWidget(parent)
{
    resize(300,300);
    setWindowTitle("登录...");
    setWindowIcon(QIcon(":/img/login.png"));
    setWindowButtonFlags(ElaAppBarType::StayTopButtonHint);
    setWindowButtonFlag(ElaAppBarType::StayTopButtonHint,false);
    setIsFixedSize(true);
    setWindowFlags(windowFlags()|Qt::Tool|Qt::WindowStaysOnTopHint);
    vBoxLayout = new QVBoxLayout(this);
    vBoxLayout->setAlignment(Qt::AlignCenter);

    loginText = new ElaText("登录", 30);

    nameLayout = new QHBoxLayout();
    nameTag = new ElaText("账户：");
    nameTag->setFixedSize(50,30);
    nameTag->setTextPixelSize(15);
    nameBox = new ElaLineEdit();
    nameBox->setFixedSize(250,30);
    nameBox->setAlignment(Qt::AlignCenter);
    nameLayout->addWidget(nameTag);
    nameLayout->addWidget(nameBox);

    pwdLayout = new QHBoxLayout();
    pwdTag = new ElaText("密码：");
    pwdTag->setFixedSize(50,30);
    pwdTag->setTextPixelSize(15);
    pwdBox = new ElaLineEdit();
    pwdBox->setFixedSize(250,30);
    pwdBox->setEchoMode(QLineEdit::Password);
    pwdBox->setAlignment(Qt::AlignCenter);
    pwdLayout->addWidget(pwdTag);
    pwdLayout->addWidget(pwdBox);

    btnLayout = new QHBoxLayout();

    cancelBtn = new ElaPushButton("取消");
    cancelBtn->setFixedSize(130,40);
    okBtn = new ElaPushButton("登入");
    okBtn->setFixedSize(130,40);

    //btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(okBtn);
    //btnLayout->addStretch();

    progressBar = new ElaProgressBar();
    progressBar->setMinimum(0);
    progressBar->setMaximum(0);
    progressBar->setVisible(false);

    timer = new QTimer();
    timer->setInterval(5*1000);
    connect(timer,&QTimer::timeout,this,[=](){
        progressBar->setVisible(false);
        emit state(false);
        this->close();
    });
    connect(okBtn,&ElaPushButton::clicked,this,[=](){
        nameBox->setEnabled(false);
        pwdBox->setEnabled(false);
        cancelBtn->setEnabled(false);
        okBtn->setEnabled(false);
        emit userInfo(nameBox->text(),pwdBox->text());
        timer->start();
        progressBar->setVisible(true);
    });
    connect(cancelBtn,&ElaPushButton::clicked,this,[=](){
        this->close();
    });
    vBoxLayout->addStretch(5);
    vBoxLayout->addWidget(loginText);
    vBoxLayout->addSpacing(20);
    vBoxLayout->addLayout(nameLayout);
    vBoxLayout->addSpacing(10);
    vBoxLayout->addLayout(pwdLayout);
    vBoxLayout->addSpacing(10);
    vBoxLayout->addLayout(btnLayout);
    vBoxLayout->addSpacing(5);
    vBoxLayout->addWidget(progressBar);
    vBoxLayout->addStretch(10);
}

void LoginDialog::keyPressEvent(QKeyEvent* event){
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
        this->okBtn->click();
    }
}

void LoginDialog::getFeedback(bool success){
    timer->stop();
    progressBar->setVisible(false);
    this->close();
}
