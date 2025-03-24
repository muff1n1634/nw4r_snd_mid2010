#ifndef NW4R_SND_SEQ_FILE_H
#define NW4R_SND_SEQ_FILE_H

/*******************************************************************************
 * headers
 */

#include <macros.h> // NW4R_VERSION
#include <types.h>

#include "Util.h"

#include "../ut/binaryFileFormat.h"

/*******************************************************************************
 * types
 */

namespace nw4r { namespace snd { namespace detail { namespace SeqFile
{
	struct Header
	{
		ut::BinaryFileHeader	fileHeader;			// size 0x10, offset 0x00
		u32						dataBlockOffset;	// size 0x04, offset 0x10
		u32						dataBlockSize;		// size 0x04, offset 0x14
		u32						labelBlockOffset;	// size 0x04, offset 0x18
		u32						labelBlockSize;		// size 0x04, offset 0x1c
	}; // size 0x20

	static const byte4_t SIGNATURE_DATA_BLOCK = 'DATA';

	struct DataBlock
	{
		ut::BinaryBlockHeader	blockHeader;	// size 0x08, offset 0x00
		u32						baseOffset;		// size 0x04, offset 0x08
	}; // size 0x0c

	struct LabelInfo
	{
		u32		offset;		// size 0x04, offset 0x00
		u32		nameLen;	// size 0x04, offset 0x04
		char	name[1];	// size 0x01, offset 0x08
		/* 3 bytes padding */
	}; // size 0x0c

	struct LabelBlock
	{
		ut::BinaryBlockHeader	blockHeader;	// size 0x08, offset 0x00
		Util::Table<u32>		labelInfoTable;	// size 0x08, offset 0x08 // presumably
	}; // size 0x10
}}}} // namespace nw4r::snd::detail::SeqFile

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	class SeqFileReader
	{
	// methods
	public:
		// cdtors
		SeqFileReader(void const *seqData);

		// gethods
		const void *GetBaseAddress() const;

		// methods
		bool IsValidFileHeader(void const *seqData);
		bool ReadOffsetByLabel(char const *labelName, u32 *offsetPtr) const;

	// static members
	public:
		static byte4_t const SIGNATURE = 'RSEQ';
		static int const VERSION = NW4R_VERSION(1, 0);

	// members
	private:
		SeqFile::Header const		*mHeader;		// size 0x04, offset 0x00
		SeqFile::DataBlock const	*mDataBlock;	// size 0x04, offset 0x04
	}; // size 0x08
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_SEQ_FILE_H
