//
// Created by Keranol on 25-5-30.
//

#ifndef TASKMANAGEEVENTBUS_H
#define TASKMANAGEEVENTBUS_H

#define tManage TaskManageEventBus::getInstance()

#include "publicheader.h"
class TaskManageEventBus:public QObject {
Q_OBJECT
public:
	static TaskManageEventBus* getInstance();
signals:
	void TaskChanged(Task tsk, bool& ok);
	void TaskDeleted(Task tsk, bool& ok);
	void TaskAdded(Task tsk, bool& ok);
	void TaskFinished(Task tsk,bool finished, bool& ok);
	void DatabaseChanged();
	void DatabaseUnchanged();
private:
	TaskManageEventBus() = default;
	TaskManageEventBus(const TaskManageEventBus&) = delete;
	TaskManageEventBus& operator=(const TaskManageEventBus&) = delete;
};



#endif //TASKMANAGEEVENTBUS_H
