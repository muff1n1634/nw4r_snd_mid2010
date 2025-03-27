#ifndef NW4R_SND_DISPOSE_CALLBACK_MANAGER_H
#define NW4R_SND_DISPOSE_CALLBACK_MANAGER_H

/*******************************************************************************
 * headers
 */

#include <types.h> // u32

#include "../ut/LinkList.h"

/*******************************************************************************
 * classes
 */

namespace nw4r { namespace snd { namespace detail
{
	class DisposeCallback
	{
	// typedefs
	public:
		typedef ut::LinkList<DisposeCallback, 0> LinkList;

	// methods
	public:
		// cdtors
		DisposeCallback() {}

	// members
	public:
		ut::LinkListNode	node;	// size 0x08, offset 0x00
		/* vtable */				// size 0x04, offset 0x08

	// late virtual methods
	public:
		// virtual function ordering
		// vtable DisposeCallback
		virtual ~DisposeCallback() {}
		virtual void InvalidateData(const void *pStart, const void *pEnd) = 0;
		virtual void InvalidateWaveData(const void *pStart, const void *pEnd) = 0;
	}; // size 0x0c

	class DisposeCallbackManager
	{
	// methods
	public:
		// instance accessors
		static DisposeCallbackManager &GetInstance();

		// methods
		void RegisterDisposeCallback(DisposeCallback *callback);
		void UnregisterDisposeCallback(DisposeCallback *callback);

		void Dispose(void *mem, u32 size, void *arg);
		void DisposeWave(void *mem, u32 size, void *arg);

	private:
		DisposeCallbackManager();

	// members
	private:
		DisposeCallback::LinkList	mCallbackList;	// size 0x0c, offset 0x00
	}; // size 0x0c
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_DISPOSE_CALLBACK_MANAGER_H
