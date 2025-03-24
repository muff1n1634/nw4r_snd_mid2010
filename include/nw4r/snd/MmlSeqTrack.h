#ifndef NW4R_SND_MML_SEQ_TRACK_H
#define NW4R_SND_MML_SEQ_TRACK_H

/*******************************************************************************
 * headers
 */

#include "SeqTrack.h"

/*******************************************************************************
 * classes and functions
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail { class MmlParser; } } }

namespace nw4r { namespace snd { namespace detail
{
	class MmlSeqTrack : public SeqTrack
	{
	// methods
	public:
		// cdtors
		MmlSeqTrack();
		virtual ~MmlSeqTrack() {}

		// virtual function ordering
		// vtable SeqTrack
		virtual ParseResult Parse(bool doNoteOn);

		// methods
		void SetMmlParser(const MmlParser *parser) { mParser = parser; }

	// members
	private:
		/* base SeqTrack */			// size 0xc8, offset 0x00
		MmlParser const	*mParser;	// size 0x04, offset 0xc8
	}; // size 0xcc
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_MML_SEQ_TRACK_H
