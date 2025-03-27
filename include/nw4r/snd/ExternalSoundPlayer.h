#ifndef NW4R_SND_EXTERNAL_SOUND_PLAYER_H
#define NW4R_SND_EXTERNAL_SOUND_PLAYER_H

/*******************************************************************************
 * headers
 */

#include "BasicSound.h"

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	class ExternalSoundPlayer
	{
	// methods
	public:
		// methods
		bool AppendSound(BasicSound *sound);
		void RemoveSound(BasicSound *sound);

		BasicSound *GetLowestPrioritySound();
		int GetPlayableSoundCount() const { return mPlayableCount; }
		int GetPlayingSoundCount() const { return mSoundList.GetSize(); }

		bool detail_CanPlaySound(int startPriority);

	// members
	private:
		BasicSound::ExtSoundPlayerPlayLinkList	mSoundList;		// size 0x0c, offset 0x00 // break include loop
		int										mPlayableCount;	// size 0x04, offset 0x0c
	}; // size 0x10
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_EXTERNAL_SOUND_PLAYER_H
