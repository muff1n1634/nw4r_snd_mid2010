#ifndef NW4R_SND_SOUND_THREAD_H
#define NW4R_SND_SOUND_THREAD_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "AxManager.h"

#include "../ut/inlines.h" // NonCopyable
#include "../ut/LinkList.h"

#if 0
#include <revolution/OS/OSMessage.h>
#include <revolution/OS/OSMutex.h>
#include <revolution/OS/OSThread.h>
#else
#include <context_rvl.h>
#endif

/*******************************************************************************
 * classes
 */

namespace nw4r { namespace snd { namespace detail
{
	class SoundThread
	{
	// enums
	private:
		enum ThreadMessage
		{
			MSG_AX_CALLBACK = 1,
			MSG_SHUTDOWN = 2,
		};

	// nested types
	public:
		class AutoLock : private ut::NonCopyable
		{
		// methods
		public:
			// cdtors
			AutoLock() { SoundThread::GetInstance().Lock(); }
			~AutoLock() { SoundThread::GetInstance().Unlock(); }
		}; // size 0x01 (0x00 for inheritance)

		class SoundFrameCallback
		{
		// typedefs
		public:
			typedef ut::LinkList<SoundFrameCallback, 0> LinkList;

		// members
		private:
			ut::LinkListNode	mLink;	// size 0x08, offset 0x00
			/* vtable */				// size 0x04, offset 0x08

		// late virtual methods
		public:
			virtual ~SoundFrameCallback() {}
			virtual void OnBeginSoundFrame() {}
			virtual void OnEndSoundFrame() {}
		}; // size 0x0c

		class PlayerCallback
		{
		// typedefs
		public:
			typedef ut::LinkList<PlayerCallback, 0> LinkList;

		// members
		private:
			ut::LinkListNode	mLink;	// size 0x08, offset 0x00
			/* vtable */				// size 0x04, offset 0x08

		// late virtual methods
		public:
			virtual ~PlayerCallback() {}
			virtual void OnUpdateFrameSoundThread() {}
			virtual void OnUpdateVoiceSoundThread() {}
			virtual void OnShutdownSoundThread() {}
		}; // size 0x0c

	// methods
	public:
		static SoundThread &GetInstance();
		void Shutdown();

		bool Create(s32 priority, void *stack, u32 stackSize);

		void Lock() { OSLockMutex(&mMutex); }
		void Unlock() { OSUnlockMutex(&mMutex); }

		void RegisterSoundFrameCallback(SoundFrameCallback *callback);
		void UnregisterSoundFrameCallback(SoundFrameCallback *callback);

		void RegisterPlayerCallback(PlayerCallback *callback);
		void UnregisterPlayerCallback(PlayerCallback *callback);

	private:
		// cdtors
		SoundThread();

		// fibers and callbacks
		void AxCallbackProc();
		static void AxCallbackFunc();

		void SoundThreadProc();
		static void *SoundThreadFunc(void *arg);

	// early constants
	private:
		static int const MSG_QUEUE_CAPACITY = 4;

	// members
	private:
		OSThread						mThread;						// offset 0x000, size 0x318
		OSThreadQueue					mThreadQueue;					// offset 0x318, size 0x008
		OSMessageQueue					mMsgQueue;						// offset 0x320, size 0x020
		OSMessage						mMsgBuffer[MSG_QUEUE_CAPACITY];	// offset 0x340, size 0x010
		byte4_t							*mStackEnd;						// offset 0x350, size 0x004
		OSMutex							mMutex;							// offset 0x354, size 0x018
		AxManager::CallbackListNode		mAxCallbackNode;				// offset 0x36c, size 0x00c
		SoundFrameCallback::LinkList	mSoundFrameCallbackList;		// offset 0x378, size 0x00c
		PlayerCallback::LinkList		mPlayerCallbackList;			// offset 0x384, size 0x00c
		u32								mProcessTick;					// offset 0x390, size 0x004
		bool							mCreateFlag;					// offset 0x394, size 0x001
		/* 3 bytes padding */
	}; // size 0x398
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_SOUND_THREAD_H
