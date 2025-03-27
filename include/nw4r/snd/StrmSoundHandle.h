#ifndef NW4R_SND_STRM_SOUND_HANDLE_H
#define NW4R_SND_STRM_SOUND_HANDLE_H

/*******************************************************************************
 * headers
 */

#include <types.h> // nullptr

#include "../ut/inlines.h" // ut::NonCopyable

/*******************************************************************************
 * classes and functions
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail { class StrmSound; }}}

namespace nw4r { namespace snd
{
	class StrmSoundHandle : private ut::NonCopyable
	{
	// methods
	public:
		bool IsAttachedSound() const { return mSound != nullptr; }

		void DetachSound();

	// members
	private:
		/* base NonCopyable */			// size 0x00, offset 0x00
		detail::StrmSound	*mSound;	// size 0x04, offset 0x00
	}; // size 0x04
}} // namespace nw4r::snd

#endif // NW4R_SND_STRM_SOUND_HANDLE_H
