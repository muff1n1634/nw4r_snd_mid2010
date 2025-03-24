#ifndef RVL_SDK_NW4R_UT_BINARY_FILE_HEADER_H
#define RVL_SDK_NW4R_UT_BINARY_FILE_HEADER_H

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
		byte4_t	signature;	// offset 0x00, size 0x04
		u16		byteOrder;	// offset 0x04, size 0x02
		u16		version;	// offset 0x06, size 0x02
		u32		fileSize;	// offset 0x08, size 0x04
		u16		headerSize;	// offset 0x0c, size 0x02
		u16		dataBlocks;	// offset 0x0e, size 0x02
	}; // size 0x10

	struct BinaryBlockHeader
	{
		u32		kind;	// offset 0x00, size 0x04
		u32		size;	// offset 0x04, size 0x04
	}; // size 0x08
}} // namespace nw4hbm::ut

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace ut
{
	bool IsValidBinaryFile(const BinaryFileHeader *header, u32 signature,
	                       u16 version, u16 minBlocks);
}} // namespace nw4hbm::ut

#endif // RVL_SDK_NW4R_UT_BINARY_FILE_HEADER_H
