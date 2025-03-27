#ifndef NW4R_SND_PLAYER_HEAP_H
#define NW4R_SND_PLAYER_HEAP_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "SoundMemoryAllocatable.h"

#include "../ut/LinkList.h"

/*******************************************************************************
 * classes and functions
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail { class BasicSound; } } }
namespace nw4r { namespace snd { class SoundPlayer; } }

namespace nw4r { namespace snd { namespace detail
{
	class PlayerHeap : public SoundMemoryAllocatable
	{
	// typedefs
	public:
		typedef ut::LinkList<PlayerHeap, 24> LinkList;

	// methods
	public:
		// cdtors
		PlayerHeap();
		virtual ~PlayerHeap();

		// virtual function ordering
		// vtable SoundMemoryAllocatable
		virtual void *Alloc(u32 size);

		// get methods
		u32 GetFreeSize() const;

		// methods
		bool Create(void *startAddress, u32 size);
		void Destroy();

		void Clear();

		void AttachSound(BasicSound *sound);
		void DetachSound(BasicSound *sound);

		void AttachSoundPlayer(SoundPlayer *player)
			{ mPlayer = player; }

	// members
	private:
		/* base SoundMemoryAllocatable */	// size 0x04, offset 0x00
		BasicSound			*mSound;		// size 0x04, offset 0x04
		SoundPlayer			*mPlayer;		// size 0x04, offset 0x08
		void				*mStartAddress;	// size 0x04, offset 0x0c
		void				*mEndAddress;	// size 0x04, offset 0x10
		void				*mAllocAddress;	// size 0x04, offset 0x14
	public:
		ut::LinkListNode	mLink;			// size 0x08, offset 0x18
	}; // size 0x20
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_PLAYER_HEAP_H
