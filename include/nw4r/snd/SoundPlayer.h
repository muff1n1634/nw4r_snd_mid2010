#ifndef NW4R_SND_SOUND_PLAYER_H
#define NW4R_SND_SOUND_PLAYER_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "BasicSound.h"
#include "global.h"
#include "PlayerHeap.h"

/*******************************************************************************
 * classes
 */

namespace nw4r { namespace snd
{
	class SoundPlayer
	{
	// methods
	public:
		// cdtors
		SoundPlayer();
		~SoundPlayer();

		// methods
		int GetDefaultOutputLine() const { return mOutputLineFlag; }
		float GetVolume() const { return mVolume; }
		float GetLpfFreq() const { return mLpfFreq; }
		int GetBiquadFilterType() const { return mBiquadType; }
		float GetBiquadFilterValue() const { return mBiquadValue; }
		float GetMainOutVolume() const { return mMainOutVolume; }
		float GetMainSend() const { return mMainSend; }
		float GetFxSend(int index) const { return mFxSend[index]; }

		void SetPlayableSoundCount(int count);
		int GetPlayableSoundCount() const { return mPlayableCount; }

		int GetPlayingSoundCount() const { return mSoundList.GetSize(); }

		detail::BasicSound *GetLowestPrioritySound()
		{
			return &mPriorityList.GetFront();
		}

		void Update();
		void PauseAllSound(bool flag, int fadeFrames);
		void StopAllSound(int fadeFrames);

		void InsertPriorityList(detail::BasicSound *sound);
		void RemovePriorityList(detail::BasicSound *sound);

		void RemoveSoundList(detail::BasicSound *sound);

	// implementation details
	public:
		bool detail_CanPlaySound(int startPriority);
		void detail_SetPlayableSoundLimit(int limit);
		bool detail_AppendSound(detail::BasicSound *sound);
		void detail_RemoveSound(detail::BasicSound *sound);

		detail::PlayerHeap *detail_AllocPlayerHeap(detail::BasicSound *sound);
		void detail_FreePlayerHeap(detail::BasicSound *sound);
		void detail_AppendPlayerHeap(detail::PlayerHeap *heap);

		void detail_SortPriorityList(detail::BasicSound *sound);
		void detail_SortPriorityList();

	// members
	private:
		detail::BasicSound::SoundPlayerPlayLinkList		mSoundList;				// size 0x0c, offset 0x00
		detail::BasicSound::SoundPlayerPriorityLinkList	mPriorityList;			// size 0x0c, offset 0x0c
		detail::PlayerHeap::LinkList					mHeapList;				// size 0x0c, offset 0x18
		int												mPlayableCount;			// size 0x04, offset 0x24
		int												mPlayableLimit;			// size 0x04, offset 0x28
		f32												mVolume;				// size 0x04, offset 0x2c
		f32												mLpfFreq;				// size 0x04, offset 0x30
		int												mOutputLineFlag;		// size 0x04, offset 0x34
		f32												mMainOutVolume;			// size 0x04, offset 0x38
		int												mBiquadType;			// size 0x04, offset 0x3c
		f32												mBiquadValue;			// size 0x04, offset 0x40
		f32												mMainSend;				// size 0x04, offset 0x54
		f32												mFxSend[AUX_BUS_NUM];	// size 0x0c, offset 0x58
	}; // size 0x64
}} // namespace nw4r::snd

#endif // NW4R_SND_SOUND_PLAYER_H
