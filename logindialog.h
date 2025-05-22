#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "ElaWidget.h"
#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaText.h"
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "ElaProgressBar.h"
#include "QTimer"
#include <QKeyEvent>

class LoginDialog:public ElaWidget
{
    Q_OBJECT
private:
    QVBoxLayout *vBoxLayout;
    QHBoxLayout *nameLayout, *pwdLayout;
    QHBoxLayout *btnLayout;
    ElaText *nameTag, *pwdTag;
    ElaText *loginText;
    ElaLineEdit *nameBox;
    ElaLineEdit *pwdBox;
    ElaPushButton *cancelBtn;
    ElaPushButton *okBtn;
    ElaProgressBar *progressBar;
    QTimer *timer;
public:
    LoginDialog(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent*) override;
public slots:
    void getFeedback(bool success);
signals:
    void userInfo(QString name,QString pwd);
    void state(bool);
};

#endif // LOGINDIALOG_H
