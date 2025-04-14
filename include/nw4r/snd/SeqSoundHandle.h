#ifndef NW4R_SND_SEQ_SOUND_HANDLE_H
#define NW4R_SND_SEQ_SOUND_HANDLE_H

/*******************************************************************************
 * headers
 */

#include <types.h> // nullptr

#include "../ut/inlines.h" // ut::NonCopyable

/*******************************************************************************
 * classes and functions
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail { class SeqSound; }}}

namespace nw4r { namespace snd { namespace detail
{
	class SeqSoundHandle : private ut::NonCopyable
	{
	// methods
	public:
		void DetachSound();

		bool IsAttachedSound() const { return mSound != nullptr; }

	// members
	private:
		SeqSound	*mSound;	// size 0x04, offset 0x00
	}; // size 0x00
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_SEQ_SOUND_HANDLE_H
