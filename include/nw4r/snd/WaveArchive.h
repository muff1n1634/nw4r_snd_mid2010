#ifndef NW4R_SND_WAVE_ARCHIVE_H
#define NW4R_SND_WAVE_ARCHIVE_H

/*******************************************************************************
 * headers
 */

#include <macros.h>
#include <types.h>

#include "Util.h"

#include "../ut/binaryFileFormat.h"

/*******************************************************************************
 * types
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail { namespace WaveFile
{
	struct FileHeader;
}}}} // namespace nw4r::snd::detail::WaveFile

namespace nw4r { namespace snd { namespace detail
{
	namespace WaveArchive
	{
		struct FileHeader
		{
			ut::BinaryFileHeader	fileHeader;			// size 0x10, offset 0x00
			u32						tableChunkOffset;	// size 0x04, offset 0x10
			u32						tableChunkSize;		// size 0x04, offset 0x14
			u32						dataChunkOffset;	// size 0x04, offset 0x18
			u32						dataChunkSize;		// size 0x04, offset 0x1c
		}; // size 0x20

		static byte4_t const SIGNATURE_TABLE_BLOCK = 'TABL';

		struct TableItem
		{
			Util::DataRef<WaveFile::FileHeader>	waveFileRef;	// size 0x08, offset 0x00
			u32									waveFileSize;	// size 0x04, offset 0x08
		}; // size 0x0c

		struct TableBlock
		{
			ut::BinaryBlockHeader	blockHeader;	// size 0x08, offset 0x00
			Util::Table<TableItem>	fileTable;		// size 0x10, offset 0x08
		}; // size 0x18

		static byte4_t const SIGNATURE_DATA_BLOCK = 'DATA';

		struct DataBlock
		{
			ut::BinaryBlockHeader	blockHeader;	// size 0x08, offset 0x00
			byte_t					data[1];		// size 0x01, offset 0x08
			/* 3 bytes padding */
		}; // size 0x0c
	} // namespace WaveArchive
}}} // namespace nw4r::snd::detail

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	class WaveArchiveReader
	{
	// methods
	public:
		// cdtors
		WaveArchiveReader(void const *waveArc);

		// methods
		WaveFile::FileHeader const *GetWaveFile(int index) const;

		bool VerifyFileHeader(void const *waveArc);

	// static members
	public:
		static byte4_t const SIGNATURE = 'RWAR';
		static u16 const VERSION = NW4R_VERSION(1, 0);

	// members
	private:
		WaveArchive::TableBlock	const *mTableBlock;	// size 0x04, offset 0x00
		WaveArchive::DataBlock	const *mDataBlock;	// size 0x04, offset 0x04
	}; // size 0x08
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_WAVE_ARCHIVE_H
