#include "nw4r/snd/SoundHandle.h"

/* Original source:
 * kiwi515/ogws
 * src/nw4r/snd/snd_SoundHandle.cpp
 */

/*******************************************************************************
 * headers
 */

#include <types.h> // nullptr

#include "nw4r/snd/BasicSound.h"

#include "nw4r/NW4RAssert.hpp"

/*******************************************************************************
 * functions
 */

namespace nw4r { namespace snd {

void SoundHandle::detail_AttachSound(detail::BasicSound *sound)
{
	NW4RAssertPointerNonnull_Line(81, sound);

	mSound = sound;

	if (sound->IsAttachedGeneralHandle())
		mSound->DetachGeneralHandle();

	mSound->mGeneralHandle = this;
}

void SoundHandle::DetachSound()
{
	if (IsAttachedSound())
	{
		if (mSound->mGeneralHandle == this)
			mSound->mGeneralHandle = nullptr;

		if (mSound->mTempGeneralHandle == this)
			mSound->mTempGeneralHandle = nullptr;
	}

	if (mSound)
		mSound = nullptr;
}

}} // namespace nw4r::snd
