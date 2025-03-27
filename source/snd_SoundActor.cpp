#include "nw4r/snd/SoundActor.h"

#include <types.h>
#include "nw4r/snd/SoundArchivePlayer.h"

nw4r::snd::SoundActor::SoundActor():
	mSoundArchivePlayer	(*new SoundArchivePlayer)
{
	mSoundArchivePlayer.detail_ConvertLabelStringToSoundId(nullptr);
}
