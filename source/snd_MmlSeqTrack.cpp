#include "nw4r/snd/MmlSeqTrack.h"

/* Original source:
 * kiwi515/ogws
 * src/nw4r/snd/snd_MmlSeqTrack.cpp
 */

/*******************************************************************************
 * headers
 */

#include "nw4r/snd/MmlParser.h"
#include "nw4r/snd/SeqTrack.h" // ParseResult

/*******************************************************************************
 * functions
 */

namespace nw4r { namespace snd { namespace detail {

MmlSeqTrack::MmlSeqTrack() {}

ParseResult MmlSeqTrack::Parse(bool doNoteOn)
{
	return mParser->Parse(this, doNoteOn);
}

}}} // namespace nw4r::snd::detail
