#ifndef NW4R_SND_TASK_H
#define NW4R_SND_TASK_H

/*******************************************************************************
 * headers
 */

#include "../ut/inlines.h" // ut::NonCopyable
#include "../ut/LinkList.h"

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	class Task : private ut::NonCopyable
	{
	// typedefs
	public:
		typedef ut::LinkList<Task, 4> LinkList;

	// methods
	public:
		// cdtors
		Task() : mBusyFlag(false) {}
		virtual ~Task();

		// virtual function ordering
		// vtable Task
		virtual void Execute() = 0;
		virtual void Cancel() = 0;
		virtual void OnCancel() = 0;

	// members
	private:
		/* base NonCopyable */			// size 0x00, offset 0x00
		/* vtable */					// size 0x04, offset 0x00
		ut::LinkListNode	mTaskLink;	// size 0x08, offset 0x04
		bool				mBusyFlag;	// size 0x01, offset 0x0c
		/* 3 bytes padding */

	// friends
	private:
		friend class TaskManager;
	}; // size 0x10
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_TASK_H
