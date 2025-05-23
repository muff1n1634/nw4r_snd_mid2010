#ifndef NW4R_UT_LOCK_H
#define NW4R_UT_LOCK_H

/*******************************************************************************
 * headers
 */

#include <types.h> // BOOL

#include "inlines.h" // NonCopyable

#if 0
#include <revolution/OS/OSInterrupt.h>
#include <revolution/OS/OSMutex.h>
#else
#include <context_rvl.h>
#endif

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace ut
{
	namespace detail
	{
		inline void Lock(OSMutex &rMutex) { OSLockMutex(&rMutex); }
		inline void Unlock(OSMutex &rMutex) { OSUnlockMutex(&rMutex); }

		template <typename T>
		class AutoLock : private NonCopyable
		{
		// methods
		public:
			// cdtors
			AutoLock(T &rLockObj) :
				mLockObj(rLockObj)
			{
				Lock(mLockObj);
			}
			~AutoLock() { Unlock(mLockObj); }

		private:
			T	&mLockObj;	// size 0x04, offset 0x00
		}; // size 0x04
	} // namespace detail

	class AutoInterruptLock : private NonCopyable
	{
	// methods
	public:
		// cdtors
		AutoInterruptLock() : mOldState(OSDisableInterrupts()) {}
		~AutoInterruptLock() { OSRestoreInterrupts(mOldState); }

	// members
	private:
		BOOL	mOldState;	// size 0x04, offset 0x00
	}; // size 0x04
}} // namespace nw4r::ut

#endif // NW4R_UT_LOCK_H
