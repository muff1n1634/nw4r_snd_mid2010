#ifndef NW4R_SND_SOUND_ACTOR_H
#define NW4R_SND_SOUND_ACTOR_H

/*******************************************************************************
 * headers
 */

#include <types.h> // nullptr

#include "BasicSound.h"
#include "ExternalSoundPlayer.h"
#include "SoundStartable.h"

/*******************************************************************************
 * classes and functions
 */

// forward declarations
namespace nw4r { namespace snd { class SoundArchivePlayer; }}

namespace nw4r { namespace snd
{
	class SoundActor : public SoundStartable
	{
	// methods
	public:
		// cdtors
		SoundActor();

		// gethods
		const detail::SoundActorParam *detail_GetActorParam() const
		{
			return &mActorParam;
		}

		detail::ExternalSoundPlayer *detail_GetActorPlayer(int actorPlayerId)
		{
			if (0 > actorPlayerId || actorPlayerId >= 4)
				return nullptr;

			return &mActorPlayer[actorPlayerId];
		}

	// members
	private:
		/* base SoundStartable */							// offset 0x00, size 0x04
		SoundArchivePlayer			&mSoundArchivePlayer;	// offset 0x04, size 0x04
		detail::ExternalSoundPlayer	mActorPlayer[4];		// offset 0x08, size 0x40
		detail::SoundActorParam		mActorParam;			// offset 0x48, size 0x0c
	}; // size 0x54
}} // namespace nw4r::snd

#endif // NW4R_SND_SOUND_ACTOR_H
