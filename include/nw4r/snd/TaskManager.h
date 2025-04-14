#ifndef NW4R_SND_TASK_MANAGER_H
#define NW4R_SND_TASK_MANAGER_H

/*******************************************************************************
 * headers
 */

#include "Task.h"

#if 0
#include <revolution/OS/OSThread.h>
#else
#include <context_rvl.h>
#endif

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	class TaskManager
	{
	// enums
	public:
		enum TaskPriority
		{
			PRIORITY_LOW,
			PRIORITY_MIDDLE,
			PRIORITY_HIGH,
		};

	// methods
	public:
		// instance accessors
		static TaskManager &GetInstance();

		// methods
		void AppendTask(Task *task, TaskPriority priority);
		Task *PopTask();
		Task *GetNextTask();
		Task *GetNextTask(TaskPriority priority, bool doRemove);
		void CancelTask(Task *task);
		void CancelAllTask();
		Task *ExecuteTask();
		void WaitTask();
		void CancelWaitTask();

	private:
		// cdtors
		TaskManager();

	// static members
	public:
		static const int MAX_PRIORITY = 3;

	// members
	private:
		Task::LinkList	mTaskList[MAX_PRIORITY];	// size 0x24, offset 0x00
		Task			* volatile mCurrentTask;	// size 0x04, offset 0x24
		bool			mCancelWaitTaskFlag;		// size 0x01, offset 0x28
		/* 3 bytes padding */
		OSThreadQueue	mAppendThreadQueue;			// size 0x08, offset 0x2c
		OSThreadQueue	mDoneThreadQueue;			// size 0x08, offset 0x34
	}; // size 0x3c
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_TASK_MANAGER_H
