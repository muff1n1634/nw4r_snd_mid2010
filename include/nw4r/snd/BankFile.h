#ifndef NW4R_SND_BANK_FILE_H
#define NW4R_SND_BANK_FILE_H

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
namespace nw4r { namespace snd { namespace detail { namespace WaveFile { struct WaveInfo; }}}}
namespace nw4r { namespace snd { namespace detail { class WaveDataLocationCallback; }}}
namespace nw4r { namespace snd { namespace detail { struct WaveInfo; }}}

namespace nw4r { namespace snd { namespace detail
{
	namespace BankFile
	{
		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2afe21
		struct Header
		{
			ut::BinaryFileHeader	fileHeader;			// size 0x10, offset 0x00
			u32						dataBlockOffset;	// size 0x04, offset 0x10
			u32						dataBlockSize;		// size 0x04, offset 0x14
			u32						waveBlockOffset;	// size 0x04, offset 0x18
			u32						waveBlockSize;		// size 0x04, offset 0x1c
		}; // size 0x20

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b0666
		struct InstParam
		{
			// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b04df
			union
			{
				s32							waveIndex;					// size 0x04
				const WaveInfo				*waveInfoAddress;			// size 0x04
				WaveDataLocationCallback	*waveDataLocationCallback;	// size 0x04
			}; // size 0x04, offset 0x00
			u8					attack;					// size 0x01, offset 0x04
			u8					decay;					// size 0x01, offset 0x05
			u8					sustain;				// size 0x01, offset 0x06
			u8					release;				// size 0x01, offset 0x07
			u8					hold;					// size 0x01, offset 0x08
			u8					waveDataLocationType;	// size 0x01, offset 0x09
			u8					noteOffType;			// size 0x01, offset 0x0a
			u8					alternateAssign;		// size 0x01, offset 0x0b
			u8					originalKey;			// size 0x01, offset 0x0c
			u8					volume;					// size 0x01, offset 0x0d
			u8					pan;					// size 0x01, offset 0x0e
			byte1_t				padding2;
			f32					tune;					// size 0x04, offset 0x10
			// TODO: probably not the right template arguments
			Util::DataRef<void>	lfoTableRef;			// size 0x08, offset 0x14
			Util::DataRef<void>	graphEnvTablevRef;		// size 0x08, offset 0x1c
			Util::DataRef<void>	randomizerTableRef;		// size 0x08, offset 0x24
			byte4_t				reserved;
		}; // size 0x30

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b2114
		struct RangeTable
		{
			u8	tableSize;	// size 0x01, offset 0x00
			u8	key[];		// flexible,  offset 0x01 (unit size 0x01)
		}; // size 0x01

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b203e
		struct IndexTable
		{
			u8	min;		// size 0x01, offset 0x00
			u8	max;		// size 0x01, offset 0x01
			u16	reserved;
			u8	ref[];		// flexible,  offset 0x04 (unit size 0x01)
		}; // size 0x04

		// TODO: template parameters
		static const byte4_t SIGNATURE_TABLE_BLOCK = 'TABL';
		typedef Util::DataRef<void> TableRegion;

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b29a1
		struct TableBlock
		{
			ut::BinaryBlockHeader		blockHeader;	// size 0x08, offset 0x00
			Util::Table<TableRegion>	fileTable;		// size 0x10, offset 0x08
		}; // size 0x18

		static const byte4_t SIGNATURE_DATA_BLOCK = 'DATA';
		typedef Util::DataRef<void, InstParam, RangeTable, IndexTable> DataRegion;

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b006b
		struct DataBlock
		{
			ut::BinaryBlockHeader	blockHeader;	// size 0x08, offset 0x00
			Util::Table<DataRegion>	instTable;		// size 0x0c, offset 0x08
		}; // size 0x14

		static const byte4_t SIGNATURE_WAVE_BLOCK = 'WAVE';
		typedef Util::DataRef<WaveFile::WaveInfo> WaveRegion;

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b022c
		struct WaveBlock
		{
			ut::BinaryBlockHeader	blockHeader;	// size 0x08, offset 0x00
			Util::Table<WaveRegion>	waveInfoTable;	// size 0x0c, offset 0x08
		}; // size 0x14
	} // namespace BankFile

	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b0cdd
	struct InstInfo
	{
	// enums
	public:
		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b0c84
		enum NoteOffType
		{
			NOTE_OFF_TYPE_RELEASE,
			NOTE_OFF_TYPE_IGNORE,
		};

	// nested types
	public:
		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b03e4
		struct WaveDataLocation
		{
		// enums
		public:
			// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b02aa
			enum WaveDataLocationType
			{
				WAVE_DATA_LOCATION_INDEX,
				WAVE_DATA_LOCATION_ADDRESS,
				WAVE_DATA_LOCATION_CALLBACK,
			};

		// members
		public:
			WaveDataLocationType	type;	// offset 0x00, size 0x04
			// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b0335
			union
			{
				long						index;		// size 0x04
				const WaveInfo				*address;	// size 0x04
				WaveDataLocationCallback	*callback;	// size 0x04
			}; // size 0x04, offset 0x04
		}; // size 0x08

	// members
	public:
		WaveDataLocation	waveDataLocation;	// size 0x08, offset 0x00
		u8					attack;				// size 0x01, offset 0x08
		u8					hold;				// size 0x01, offset 0x09
		u8					decay;				// size 0x01, offset 0x0a
		u8					sustain;			// size 0x01, offset 0x0b
		u8					release;			// size 0x01, offset 0x0c
		/* 3 bytes padding */
		NoteOffType			noteOffType;		// size 0x04, offset 0x10
		u8					alternateAssign;	// size 0x01, offset 0x14
		u8					originalKey;		// size 0x01, offset 0x15
		u8					pan;				// size 0x01, offset 0x16
		u8					volume;				// size 0x01, offset 0x17
		f32					tune;				// size 0x04, offset 0x18
	}; // size 0x1c
}}} // namespace nw4r::snd::detail

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b0a17
	class BankFileReader
	{
	// methods
	public:
		// cdtors
		BankFileReader(const void *bankData);

		// methods
		const BankFile::InstParam *GetInstParam(int prgNo, int key,
		                                        int velocity) const;
		bool ReadInstInfo(InstInfo *instInfo, int prgNo, int key,
		                  int velocity) const;
		bool ReadWaveInfo(WaveInfo *waveParam,
		                  const InstInfo::WaveDataLocation &waveDataLocation,
		                  const void *waveDataAddress,
		                  const WaveInfo **waveInfoAddress) const;

	private:
		const BankFile::DataRegion *GetReferenceToSubRegion(
			const BankFile::DataRegion *ref, int splitKey) const;

		static bool IsValidFileHeader(const void *bankData);

	// static members
	public:
		static const byte4_t SIGNATURE = 'RBNK';
		static const int VERSION = NW4R_VERSION(1, 2);

	// members
	private:
		const BankFile::Header		*mHeader;		// size 0x04, offset 0x00
		const BankFile::DataBlock	*mDataBlock;	// size 0x04, offset 0x04
		const BankFile::WaveBlock	*mWaveBlock;	// size 0x04, offset 0x08
	}; // size 0x0c
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_BANK_FILE_H
