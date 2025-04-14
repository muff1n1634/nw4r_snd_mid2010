#ifndef NW4R_SND_NOTE_ON_CALLBACK_H
#define NW4R_SND_NOTE_ON_CALLBACK_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "Channel.h"

/*******************************************************************************
 * types
 */

namespace nw4r { namespace snd { namespace detail
{
	struct NoteOnInfo
	{
		int					prgNo;					// size 0x04, offset 0x00
		int					key;					// size 0x04, offset 0x04
		int					velocity;				// size 0x04, offset 0x08
		int					length;					// size 0x04, offset 0x0c
		int					initPan;				// size 0x04, offset 0x10
		int					priority;				// size 0x04, offset 0x14
		int					voiceOutCount;			// size 0x04, offset 0x18
		Channel::Callback	*channelCallback;		// size 0x04, offset 0x1c
		register_t			channelCallbackData;	// size 0x04, offset 0x20
	}; // size 0x24
}}} // namespace nw4r::snd::detail

/*******************************************************************************
 * classes and functions
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail { class SeqPlayer; }}}

namespace nw4r { namespace snd { namespace detail
{
	class NoteOnCallback
	{
	// methods
	public:
		virtual ~NoteOnCallback() {}

		// virtual function ordering
		// vtable NoteOnCallback
		virtual Channel *NoteOn(SeqPlayer *seqPlayer, int bankNo,
		                        NoteOnInfo const &noteOnInfo) = 0;

	// members
	private:
		/* vtable */	// size 0x04, offset 0x00
	}; // size 0x00
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_NOTE_ON_CALLBACK_H
