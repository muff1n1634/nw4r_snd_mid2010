#ifndef NW4R_UT_BINARY_FILE_HEADER_H
#define NW4R_UT_BINARY_FILE_HEADER_H

/*******************************************************************************
 * headers
 */

#include <types.h>

/*******************************************************************************
 * types
 */

namespace nw4r { namespace ut
{
	struct BinaryFileHeader
	{
		byte4_t	signature;	// size 0x04, offset 0x00
		u16		byteOrder;	// size 0x02, offset 0x04
		u16		version;	// size 0x02, offset 0x06
		u32		fileSize;	// size 0x04, offset 0x08
		u16		headerSize;	// size 0x02, offset 0x0c
		u16		dataBlocks;	// size 0x02, offset 0x0e
	}; // size 0x10

	struct BinaryBlockHeader
	{
		byte4_t	kind;	// size 0x04, offset 0x00
		u32		size;	// size 0x04, offset 0x04
	}; // size 0x08
}} // namespace nw4r::ut

#endif // NW4R_UT_BINARY_FILE_HEADER_H
