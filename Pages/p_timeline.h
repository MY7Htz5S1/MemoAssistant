#ifndef P_TIMELINE_H
#define P_TIMELINE_H

#include <QVBoxLayout>

#include "ElaScrollPage.h"

class P_Timeline : public ElaScrollPage
{
    Q_OBJECT
public:
    P_Timeline(QWidget* parent = nullptr);
};

#endif // P_TIMELINE_H
