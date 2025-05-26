#ifndef USERCHOICEDIALOG_H
#define USERCHOICEDIALOG_H

#include "ElaWidget.h"
#include "ElaText.h"
#include "ElaPushButton.h"
#include "QVBoxLayout"
#include "QHBoxLayout"

class UserChoiceDialog : public ElaWidget
{
    Q_OBJECT

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *buttonLayout;
    ElaText *titleText;
    ElaText *messageText;
    ElaPushButton *createUserBtn;
    ElaPushButton *retryBtn;
    ElaPushButton *cancelBtn;

public:
    explicit UserChoiceDialog(const QString& username, QWidget *parent = nullptr);

signals:
    void createNewUser();
    void retryLogin();
    void cancelLogin();
};

#endif // USERCHOICEDIALOG_H
