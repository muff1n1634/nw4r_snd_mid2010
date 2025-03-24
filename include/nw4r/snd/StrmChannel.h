#ifndef NW4R_SND_STRM_CHANNEL_H
#define NW4R_SND_STRM_CHANNEL_H

/*******************************************************************************
 * headers
 */

#include <climits> // CHAR_BIT

#include <types.h>

#include "global.h"

/*******************************************************************************
 * types
 */

namespace nw4r { namespace snd { namespace detail
{
	struct StrmChannel
	{
		void			*bufferAddress;	// size 0x04, offset 0x00
		AdpcmParam		adpcmParam;		// size 0x28, offset 0x04
		AdpcmLoopParam	adpcmLoopParam;	// size 0x06, offset 0x2c
		u16				adpcmPredScale;	// size 0x02, offset 0x32
	}; // size 0x34
}}} // namespace nw4r::snd::detail

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	class StrmBufferPool
	{
	// methods
	public:
		void Setup(void *buffer, u32 size, int blockCount);
		void Shutdown();

		void *Alloc();
		void Free(void *buffer);

		u32 GetBlockSize() const { return mBlockSize; }

	// static members
	private:
		static int const BLOCK_MAX = 32;

	// members
	private:
		void	*mBuffer;							// size 0x04, offset 0x00
		u32		mBufferSize;						// size 0x04, offset 0x04
		u32		mBlockSize;							// size 0x04, offset 0x08
		int		mBlockCount;						// size 0x04, offset 0x0c
		int		mAllocCount;						// size 0x04, offset 0x10
		byte_t	mAllocFlags[BLOCK_MAX / CHAR_BIT];	// size 0x04, offset 0x14
	}; // size 0x18
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_STRM_CHANNEL_H
