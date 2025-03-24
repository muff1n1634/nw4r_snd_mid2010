#ifndef NW4R_SND_SOUND_HANDLE_H
#define NW4R_SND_SOUND_HANDLE_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "BasicSound.h"

#include "../ut/inlines.h" // NonCopyable

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd
{
	class SoundHandle : private ut::NonCopyable
	{
	// methods
	public:
		// cdtors
		SoundHandle(): mSound() {}
		~SoundHandle() { DetachSound(); }

		// methods
		bool IsAttachedSound() const { return mSound != nullptr; }

		void detail_AttachSound(detail::BasicSound *sound);
		void DetachSound();

		detail::BasicSound *detail_GetAttachedSound() { return mSound; }

		u32 GetId() const
		{
			if (IsAttachedSound())
				return mSound->GetId();

			return -1;
		}

		void StartPrepared()
		{
			if (IsAttachedSound())
				mSound->StartPrepared();
		}

	// members
	private:
		/* base NonCopyable */			// size 0x00, offset 0x00
		detail::BasicSound	*mSound;	// size 0x04, offset 0x00
	}; // size 0x04
}} // namespace nw4r::snd

#endif // NW4R_SND_SOUND_HANDLE_H
