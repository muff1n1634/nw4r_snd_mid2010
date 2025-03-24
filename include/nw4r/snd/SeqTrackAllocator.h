#ifndef NW4R_SND_SEQ_TRACK_ALLOCATOR_HPP
#define NW4R_SND_SEQ_TRACK_ALLOCATOR_HPP

/*******************************************************************************
 * classes and functions
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail { class SeqPlayer; }}}
namespace nw4r { namespace snd { namespace detail { class SeqTrack; }}}

namespace nw4r { namespace snd { namespace detail
{
	class SeqTrackAllocator
	{
	public:
		virtual ~SeqTrackAllocator() {}

		// virtual function ordering
		// vtable SeqTrackAllocator
		virtual SeqTrack *AllocTrack(SeqPlayer *player) = 0;
		virtual void FreeTrack(SeqTrack *track) = 0;
		virtual int GetAllocatableTrackCount() const = 0;
	};

}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_SEQ_TRACK_ALLOCATOR_HPP
