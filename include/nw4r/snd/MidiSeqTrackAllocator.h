#ifndef NW4R_SND_MIDI_SEQ_TRACK_ALLOCATOR_H
#define NW4R_SND_MIDI_SEQ_TRACK_ALLOCATOR_H

/*******************************************************************************
 * headers
 */

#include "SeqTrackAllocator.h"

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	class MidiSeqTrackAllocator : public SeqTrackAllocator
	{
		MidiSeqTrackAllocator();
		virtual SeqTrack *AllocTrack(SeqPlayer *) { return 0; }
		virtual void FreeTrack(SeqTrack *) {}
		virtual int GetAllocatableTrackCount() const { return 0; }
	};
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_MIDI_SEQ_TRACK_ALLOCATOR_H
