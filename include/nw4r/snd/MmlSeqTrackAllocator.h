#ifndef NW4R_SND_MML_SEQ_TRACK_ALLOCATOR_H
#define NW4R_SND_MML_SEQ_TRACK_ALLOCATOR_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "InstancePool.h"
#include "MmlParser.h"
#include "SeqPlayer.h"
#include "SeqTrack.h"
#include "SeqTrackAllocator.h"

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	class MmlSeqTrackAllocator : public SeqTrackAllocator
	{
	// methods
	public:
		// cdtors
		MmlSeqTrackAllocator(MmlParser *parser) : mParser(parser) {}
		virtual ~MmlSeqTrackAllocator() {}

		// virtual function ordering
		// vtable SeqTrackAllocator
		virtual SeqTrack *AllocTrack(SeqPlayer *player);
		virtual void FreeTrack(SeqTrack *track);
		virtual int GetAllocatableTrackCount() const
		{
			return mTrackPool.Count();
		}

		// methods
		u32 Create(void *buffer, u32 size);
		void Destroy(void *buffer, u32 size);

	// members
	private:
		/* base SeqTrackAllocator */			// size 0x04, offset 0x00
		MmlParser					*mParser;	// size 0x04, offset 0x04
		InstancePool<MmlSeqTrack>	mTrackPool;	// size 0x04, offset 0x08
	}; // size 0x0c
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_MML_SEQ_TRACK_ALLOCATOR_H
