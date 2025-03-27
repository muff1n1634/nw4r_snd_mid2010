#ifndef NW4R_SND_VOICE_MANAGER_H
#define NW4R_SND_VOICE_MANAGER_H

/*******************************************************************************
 * headers
 */

#include <types.h> // u32

#include "Voice.h"

#if 0
#include <revolution/AX/AX.h> // AX_MAX_VOICES
#else
#include <context_rvl.h>
#endif

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	class VoiceManager
	{
	// methods
	public:
		// instance accessors
		static VoiceManager &GetInstance();

		// methods
		u32 GetRequiredMemSize(int voiceCount);

		void Setup(void *mem, u32 memSize);
		void Shutdown();
		void StopAllVoices();

		Voice *AllocVoice(int voiceChannelCount, int voiceOutCount,
		                  int priority, Voice::Callback *callback,
		                  void *callbackData);
		void FreeVoice(Voice *voice);

		void UpdateAllVoices();
		void NotifyVoiceUpdate();
		void ChangeVoicePriority(Voice *voice);
		void UpdateAllVoicesSync(u32 syncFlag);

		const Voice::LinkList &GetVoiceList() const { return mPrioVoiceList; }

	private:
		VoiceManager();

		void AppendVoiceList(Voice *voice);
		void RemoveVoiceList(Voice *voice);

		void UpdateEachVoicePriority(const Voice::LinkList::Iterator &beginItr,
		                             const Voice::LinkList::Iterator &endItr);

		int DropLowestPriorityVoice(int priority);

	// members
	private:
		bool			mInitialized;	// size 0x01, offset 0x00
		/* 3 bytes padding */
		Voice::LinkList	mPrioVoiceList;	// size 0x0c, offset 0x04
		Voice::LinkList	mFreeVoiceList;	// size 0x0c, offset 0x10

	// static members
	public:
		static const int VOICE_MAX = AX_MAX_VOICES;
		static const int WORK_SIZE_MAX = VOICE_MAX * sizeof(Voice);
	}; // size 0x1c
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_VOICE_MANAGER_H
