#ifndef NW4R_SND_STRM_FILE_H
#define NW4R_SND_STRM_FILE_H

/*******************************************************************************
 * headers
 */

#include <macros.h>
#include <types.h>

#include "global.h"
#include "Util.h"

#include "../ut/binaryFileFormat.h"

/*******************************************************************************
 * types
 */

// forward declarations
namespace nw4r { namespace ut { class FileStream; }}

namespace nw4r { namespace snd { namespace detail { namespace StrmFile
{
	struct Header
	{
		ut::BinaryFileHeader	fileHeader;			// size 0x10, offset 0x00
		u32						headBlockOffset;	// size 0x04, offset 0x10
		u32						headBlockSize;		// size 0x04, offset 0x14
		u32						adpcBlockOffset;	// size 0x04, offset 0x18
		u32						adpcBlockSize;		// size 0x04, offset 0x1c
		u32						dataBlockOffset;	// size 0x04, offset 0x20
		u32						dataBlockSize;		// size 0x04, offset 0x24
	}; // size 0x28

	struct StrmDataInfo
	{
		u8	format;					// size 0x01, offset 0x00
		u8	loopFlag;				// size 0x01, offset 0x01
		u8	numChannels;			// size 0x01, offset 0x02
		u8	sampleRate24;			// size 0x01, offset 0x03
		u16	sampleRate;				// size 0x02, offset 0x04
		u16	blockHeaderOffset;		// size 0x02, offset 0x06
		u32	loopStart;				// size 0x04, offset 0x08
		u32	loopEnd;				// size 0x04, offset 0x0c
		u32	dataOffset;				// size 0x04, offset 0x10
		u32	numBlocks;				// size 0x04, offset 0x14
		u32	blockSize;				// size 0x04, offset 0x18
		u32	blockSamples;			// size 0x04, offset 0x1c
		u32	lastBlockSize;			// size 0x04, offset 0x20
		u32	lastBlockSamples;		// size 0x04, offset 0x24
		u32	lastBlockPaddedSize;	// size 0x04, offset 0x28
		u32	adpcmDataInterval;		// size 0x04, offset 0x2c
		u32	adpcmDataSize;			// size 0x04, offset 0x30
	}; // size 0x34

	struct TrackInfo
	{
		u8	channelCount;			// size 0x01, offset 0x00
		u8	channelIndexTable[];	// flexible,  offset 0x01 (unit size 0x01)
	}; // size 0x01

	struct TrackInfoEx
	{
		u8		volume;					// size 0x01, offset 0x00
		u8		pan;					// size 0x01, offset 0x01
		byte1_t	padding[2];
		byte4_t	reserved;
		u8		channelCount;			// size 0x01, offset 0x09
		u8		channelIndexTable[];	// flexible,  offset 0x0a (unit size 0x01)
		/* PAD(3) */
	}; // size 0x0c

	struct TrackTable
	{
		u8										trackCount;			// size 0x01, offset 0x00
		u8										trackDataType;		// size 0x01, offset 0x01
		byte1_t									padding[2];
		Util::DataRef<TrackInfo, TrackInfoEx>	refTrackHeader[];	// flexible,  offset 0x04 (unit size 0x08)
	}; // size 0x04

	struct AdpcmParamSet
	{
		AdpcmParam		adpcmParam;		// size 0x28, offset 0x00
		AdpcmLoopParam	adpcmLoopParam;	// size 0x06, offset 0x28
	}; // size 0x2e

	struct ChannelInfo
	{
		Util::DataRef<AdpcmParamSet>	refAdpcmInfo;	// size 0x08, offset 0x00
	}; // size 0x08

	struct ChannelTable
	{
		u8							channelCount;		// size 0x01, offset 0x00
		byte1_t						padding[3];
		Util::DataRef<ChannelInfo>	refChannelHeader[];	// flexible,  offset 0x04 (unit size 0x08)
	}; // size 0x04

	static byte4_t const SIGNATURE_HEAD_BLOCK = 'HEAD';

	struct HeadBlock
	{
		ut::BinaryBlockHeader		blockHeader;		// size 0x08, offset 0x00
		Util::DataRef<StrmDataInfo>	refDataHeader;		// size 0x08, offset 0x08
		Util::DataRef<TrackTable>	refTrackTable;		// size 0x08, offset 0x10
		Util::DataRef<ChannelTable>	refChannelTable;	// size 0x08, offset 0x18
	}; // total size: 0x20
}}}} // namespace nw4r::snd::detail::StrmFile

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	class StrmFileReader
	{
	// nested types
	public:
		struct StrmInfo
		{
			SampleFormat	sampleFormat;			// size 0x04, offset 0x00
			bool			loopFlag;				// size 0x01, offset 0x04
			/* 3 bytes padding */
			int				numChannels;			// size 0x04, offset 0x08
			int				sampleRate;				// size 0x04, offset 0x0c
			u16				blockHeaderOffset;		// size 0x02, offset 0x10
			/* 2 bytes padding */
			u32				loopStart;				// size 0x04, offset 0x14
			u32				loopEnd;				// size 0x04, offset 0x18
			u32				dataOffset;				// size 0x04, offset 0x1c
			u32				numBlocks;				// size 0x04, offset 0x20
			u32				blockSize;				// size 0x04, offset 0x24
			u32				blockSamples;			// size 0x04, offset 0x28
			u32				lastBlockSize;			// size 0x04, offset 0x2c
			u32				lastBlockSamples;		// size 0x04, offset 0x30
			u32				lastBlockPaddedSize;	// size 0x04, offset 0x34
			u32				adpcmDataInterval;		// size 0x04, offset 0x38
			u32				adpcmDataSize;			// size 0x04, offset 0x3c
		}; // size 0x40

		struct StrmTrackInfo
		{
			u8	volume;					// size 0x01, offset 0x00
			u8	pan;					// size 0x01, offset 0x01
			int	channelCount;			// size 0x04, offset 0x04
			u8	channelIndexTable[32];	// size 0x20, offset 0x08
		}; // size 0x28

	// methods
	public:
		// cdtors
		StrmFileReader();

		// methods
		void Setup(void const *strmData);

		bool IsAvailable() const { return mHeader != nullptr; }
		u32 GetAdpcBlockOffset() const
		{
			if (IsAvailable())
				return mHeader->adpcBlockOffset;
			else
				return 0;
		}

		int GetTrackCount() const;
		int GetChannelCount() const;

		bool ReadStrmInfo(StrmInfo *strmInfo) const;
		bool ReadStrmTrackInfo(StrmTrackInfo *trackInfo, int trackIndex) const;
		bool ReadAdpcmInfo(AdpcmParam *adpcmParam,
		                   AdpcmLoopParam *adpcmLoopParam,
		                   int channelIndex) const;

		static SampleFormat GetSampleFormatFromStrmFileFormat(u8 format);

		static bool IsValidFileHeader(void const *strmData);

	// static members
	public:
		static byte4_t const SIGNATURE = 'RSTM';
		static int const VERSION = NW4R_VERSION(1, 0);

	// members
	private:
		StrmFile::Header const *mHeader;       // size 0x04, offset 0x00
		StrmFile::HeadBlock const *mHeadBlock; // size 0x04, offset 0x04
	}; // size 0x08

	class StrmFileLoader
	{
	// methods
	public:
		// cdtors
		StrmFileLoader(ut::FileStream &stream) : mStream(stream), mReader() {}

		// methods
		bool LoadFileHeader(void *buffer, u32 size);

		int GetTrackCount() const;
		int GetChannelCount() const;

		bool ReadStrmInfo(StrmFileReader::StrmInfo *strmInfo) const;
		bool ReadStrmTrackInfo(StrmFileReader::StrmTrackInfo *trackInfo,
		                       int trackIndex) const;
		bool ReadAdpcmInfo(AdpcmParam *adpcmParam,
		                   AdpcmLoopParam *adpcmLoopParam,
		                   int channelIndex) const;
		bool ReadAdpcBlockData(u16 *yn1, u16 *yn2, int blockIndex,
		                       int channelCount);

	// static members
	private:
		static u32 const HEADER_ALIGNED_SIZE =
			ROUND_UP(sizeof(StrmFile::Header), 32);

		// members
	private:
		ut::FileStream	&mStream;	// size 0x04, offset 0x00
		StrmFileReader	mReader;	// size 0x08, offset 0x04
	}; // size 0x0c
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_STRM_FILE_H
