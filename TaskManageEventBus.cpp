//
// Created by Keranol on 25-5-30.
//

#include "TaskManageEventBus.h"

#include <qmutex.h>

TaskManageEventBus* TaskManageEventBus::getInstance() {
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    static TaskManageEventBus instance;
    return &instance;
}



