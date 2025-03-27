#ifndef NW4R_SND_SOUND_ARCHIVE_FILE_H
#define NW4R_SND_SOUND_ARCHIVE_FILE_H

/*******************************************************************************
 * headers
 */

#include <macros.h>
#include <types.h>

#include "Util.h"
#include "SoundArchive.h"

#include "../ut/binaryFileFormat.h"
#include "../ut/inlines.h"

/*******************************************************************************
 * types
 */

namespace nw4r { namespace snd { namespace detail { namespace SoundArchiveFile
{
static byte4_t const SIGNATURE_SYMB_BLOCK = 'SYMB';

struct StringTreeNode
{
	byte2_t flags; // size 0x02, offset 0x00
	u16 bit;       // size 0x02, offset 0x02
	u32 leftIdx;   // size 0x04, offset 0x04
	u32 rightIdx;  // size 0x04, offset 0x08
	u32 strIdx;    // size 0x04, offset 0x0c
	u32 id;        // size 0x04, offset 0x10
	}; // size 0x14

	struct StringTree
	{
		u32							rootIdx;	// size 0x04, offset 0x00
		Util::Table<StringTreeNode>	nodeTable;	// size 0x18, offset 0x04
	}; // size 0x1c

	struct StringTable
	{
		Util::Table<u32>	offsetTable;	// size 0x08, offset 0x00
	}; // size 0x08

	struct StringChunk
	{
		u32	tableOffset;		// size 0x04, offset 0x00
		u32	soundTreeOffset;	// size 0x04, offset 0x04
		u32	playerTreeOffset;	// size 0x04, offset 0x08
		u32	groupTreeOffset;	// size 0x04, offset 0x0c
		u32	bankTreeOffset;		// size 0x04, offset 0x10
	}; // size 0x14

	struct StringBlock
	{
		union
		{
			StringTable	stringTable;	// size 0x8
			StringChunk	stringChunk;	// size 0x14
		}; // size 0x14, offset 0x00
	}; // size 0x14

	struct SymbolBlock
	{
		ut::BinaryBlockHeader	blockHeader;	// size 0x08, offset 0x00
		StringBlock				stringBlock;	// size 0x14, offset 0x08
	}; // size 0x20

	struct Header
	{
		ut::BinaryFileHeader	fileHeader;			// size 0x10, offset 0x00
		u32						symbolDataOffset;	// size 0x04, offset 0x10
		u32						symbolDataSize;		// size 0x04, offset 0x14
		u32						infoOffset;			// size 0x04, offset 0x18
		u32						infoSize;			// size 0x04, offset 0x1c
		u32						fileImageOffset;	// size 0x04, offset 0x20
		u32						fileImageSize;		// size 0x04, offset 0x24
	}; // size 0x28

	// no ketteiban dwarf, and I don't need the definition here
	struct Sound3DParam;

	struct SeqSoundInfo
	{
		u32		dataOffset;			// size 0x04, offset 0x00
		u32		bankId;				// size 0x04, offset 0x04
		u32		allocTrack;			// size 0x04, offset 0x08
		u8		channelPriority;	// size 0x01, offset 0x0C
		u8		releasePriorityFix;	// size 0x01, offset 0x0D
		byte1_t	padding[2];
		byte4_t	reserved;
	}; // size 0x14

	struct StrmSoundInfo
	{
		u32		startPosition;		// size 0x04, offset 0x00
		u16		allocChannelCount;	// size 0x02, offset 0x04
		u16		allocTrackFlag;		// size 0x02, offset 0x06
		byte4_t	reserved;
	}; // size 0x0c

	struct WaveSoundInfo
	{
		s32		subNo;				// size 0x04, offset 0x00
		u32		allocTrack;			// size 0x04, offset 0x04
		u8		channelPriority;	// size 0x01, offset 0x08
		u8		releasePriorityFix;	// size 0x01, offset 0x09
		byte1_t	padding[2];
		byte4_t	reserved;
	}; // size 0x10

	typedef Util::DataRef<void, SeqSoundInfo, StrmSoundInfo, WaveSoundInfo>
		SoundInfoOffset;

	struct SoundCommonInfo
	{
	// typedefs
	public:
		typedef Util::Table<Util::DataRef<SoundCommonInfo> > Table;

	// members
	public:
		u32							stringId;		// size 0x04, offset 0x00
		u32							fileId;			// size 0x04, offset 0x04
		u32							playerId;		// size 0x04, offset 0x08
		Util::DataRef<Sound3DParam>	param3dRef;		// size 0x08, offset 0x0c
		u8							volume;			// size 0x01, offset 0x14
		u8							playerPriority;	// size 0x01, offset 0x15
		u8							soundType;		// size 0x01, offset 0x16
		u8							remoteFilter;	// size 0x01, offset 0x17
		SoundInfoOffset				soundInfoRef;	// size 0x08, offset 0x18
		register_t					userParam[2];	// size 0x08, offset 0x20
		u8							panMode;		// size 0x01, offset 0x28
		u8							panCurve;		// size 0x01, offset 0x29
		u8							actorPlayerId;	// size 0x01, offset 0x2a
		byte1_t						reserved;
	}; // size 0x2c

	struct BankInfo
	{
	// typedefs
	public:
		typedef Util::Table<Util::DataRef<BankInfo> > Table;

	// members
	public:
		u32		stringId;	// size 0x04, offset 0x00
		u32		fileId;		// size 0x04, offset 0x04
		byte4_t	reserved;
	}; // size 0x0c

	struct PlayerInfo
	{
	// typedefs
	public:
		typedef Util::Table<Util::DataRef<PlayerInfo> > Table;

	// members
	public:
		u32		stringId;			// size 0x04, offset 0x00
		u8		playableSoundCount;	// size 0x01, offset 0x04
		byte1_t	padding;
		byte2_t	padding2;
		u32		heapSize;			// size 0x04, offset 0x08
		byte4_t	reserved;
	}; // size 0x10

	struct FilePos
	{
		typedef Util::Table<Util::DataRef<SoundArchive::FilePos> > Table;

		/* delegates out to SoundArchive::FilePos, but this is to keep the
		 * Table typedef namespacing consistent
		 *
		 * also the typedef is here instead of SoundArchive::FilePos because
		 * the rest of the Table typedefs are in this namespace which seems
		 * reasonable to me
		 */
	}; // fake

	struct FileInfo
	{
	// typedefs
	public:
		typedef Util::Table<Util::DataRef<FileInfo> > Table;

	// members
	public:
		u32								fileSize;			// size 0x04, offset 0x00
		u32								waveDataFileSize;	// size 0x04, offset 0x04
		s32								entryNum;			// size 0x04, offset 0x08
		Util::DataRef<char>				extFilePathRef;		// size 0x08, offset 0x0c
		Util::DataRef<FilePos::Table>	filePosTableRef;	// size 0x08, offset 0x14
	}; // total size: 0x1C

	struct GroupItemInfo
	{
	// typedefs
	public:
		typedef Util::Table<Util::DataRef<GroupItemInfo> > Table;

	// members
	public:
		u32	fileId;			// size 0x04, offset 0x00
		u32	offset;			// size 0x04, offset 0x04
		u32	size;			// size 0x04, offset 0x08
		u32	waveDataOffset;	// size 0x04, offset 0x0c
		u32	waveDataSize;	// size 0x04, offset 0x10
		byte4_t	reserved;
	}; // size 0x18

	struct GroupInfo
	{
	// typedefs
	public:
		typedef Util::Table<Util::DataRef<GroupInfo> > Table;

	// members
	public:
		u32									stringId;		// size 0x04, offset 0x00
		s32									entryNum;		// size 0x04, offset 0x04
		Util::DataRef<char>					extFilePathRef;	// size 0x08, offset 0x08
		u32									offset;			// size 0x04, offset 0x10
		u32									size;			// size 0x04, offset 0x14
		u32									waveDataOffset;	// size 0x04, offset 0x18
		u32									waveDataSize;	// size 0x04, offset 0x1c
		Util::DataRef<GroupItemInfo::Table>	itemTableRef;	// size 0x08, offset 0x20
	}; // size 0x28

	struct SoundArchivePlayerInfo
	{
		u16		seqSoundCount;		// size 0x02, offset 0x00
		u16		seqTrackCount;		// size 0x02, offset 0x02
		u16		strmSoundCount;		// size 0x02, offset 0x04
		u16		strmTrackCount;		// size 0x02, offset 0x06
		u16		strmChannelCount;	// size 0x02, offset 0x08
		u16		waveSoundCount;		// size 0x02, offset 0x0a
		u16		waveTrackCount;		// size 0x02, offset 0x0c
		byte2_t	padding;
		byte4_t	reserved;
	}; // size 0x14

	struct Info
	{
		Util::DataRef<SoundCommonInfo::Table>	soundTableRef;				// size 0x08, offset 0x00
		Util::DataRef<BankInfo::Table>			bankTableRef;				// size 0x08, offset 0x08
		Util::DataRef<PlayerInfo::Table>		playerTableRef;				// size 0x08, offset 0x10
		Util::DataRef<FileInfo::Table>			fileTableRef;				// size 0x08, offset 0x18
		Util::DataRef<GroupInfo::Table>			groupTableRef;				// size 0x08, offset 0x20
		Util::DataRef<SoundArchivePlayerInfo>	soundArchivePlayerInfoRef;	// size 0x08, offset 0x28
	}; // size 0x30

	static byte4_t const SIGNATURE_INFO_BLOCK = 'INFO';

	struct InfoBlock
	{
		ut::BinaryBlockHeader	blockHeader;	// size 0x08, offset 0x00
		Info					info;			// size 0x30, offset 0x08
	}; // total size: 0x38
}}}} // namespace nw4r::snd::detail::SoundArchiveFile

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	class SoundArchiveFileReader
	{
	// methods
	public:
		// cdtors
		SoundArchiveFileReader();

		// methods
		void Init(void const *soundArchive);

		u32 GetLabelStringChunkOffset() const
		{
			return mHeader.symbolDataOffset;
		}
		u32 GetLabelStringChunkSize() const { return mHeader.symbolDataSize; }
		u32 GetInfoChunkOffset() const { return mHeader.infoOffset; }
		u32 GetInfoChunkSize() const { return mHeader.infoSize; }

		void SetStringChunk(void const *chunk, u32 size);
		void SetInfoChunk(void const *infoChunk, u32 infoChunkSize);

		bool ReadFileInfo(u32 fileId, SoundArchive::FileInfo *info) const;
		bool ReadFilePos(u32 fileId, u32 index,
		                 SoundArchive::FilePos *filePos) const;

		bool ReadGroupInfo(u32 groupId, SoundArchive::GroupInfo *info) const;
		bool ReadGroupItemInfo(u32 groupId, u32 index,
		                       SoundArchive::GroupItemInfo *info) const;
		u32 GetFileCount() const;
		bool ReadBankInfo(u32 bankId, SoundArchive::BankInfo *info) const;
		bool ReadSoundArchivePlayerInfo(
			SoundArchive::SoundArchivePlayerInfo *info) const;
		bool ReadPlayerInfo(u32 playerId, SoundArchive::PlayerInfo *info) const;
		bool ReadWaveSoundInfo(u32 soundId,
		                       SoundArchive::WaveSoundInfo *info) const;
		bool ReadStrmSoundInfo(u32 soundId,
		                       SoundArchive::StrmSoundInfo *info) const;
		bool ReadSeqSoundInfo(u32 soundId,
		                      SoundArchive::SeqSoundInfo *info) const;
		bool ReadSound3DInfo(u32, SoundArchive::Sound3DParam *) const;
		bool ReadSoundInfo(u32 soundId, SoundArchive::SoundInfo *info) const;
		SoundType GetSoundType(u32 soundId) const;
		u32 ConvertLabelStringToId(SoundArchiveFile::StringTree const *tree,
		                           char const *str) const;
		u32 ConvertLabelStringToSoundId(char const *label) const
		{
			return ConvertLabelStringToId(mStringTreeSound, label);
		}
		u32 GetGroupCount() const;
		u32 GetPlayerCount() const;
		u32 GetBankCount() const;

		static bool IsValidFileHeader(void const *soundArchiveData);

		void const *GetPtrConst(void const *base, u32 offset) const
		{
			if (offset == 0)
				return nullptr;

			return ut::AddOffsetToPtr(base, offset);
		}

		u16 GetVersion() const { return mHeader.fileHeader.version; }
		char const *GetString(u32 id) const;

	private:
		SoundArchiveFile::SoundCommonInfo const *impl_GetSoundInfo(
			u32 soundId) const;
		SoundArchiveFile::SeqSoundInfo const *impl_GetSeqSoundInfo(
			u32 soundId) const;
		SoundArchiveFile::StrmSoundInfo const *impl_GetStrmSoundInfo(
			u32 soundId) const;
		SoundArchiveFile::WaveSoundInfo const *impl_GetWaveSoundInfo(
			u32 soundId) const;
		SoundArchiveFile::BankInfo const *impl_GetBankInfo(u32 bankId) const;
		SoundArchiveFile::PlayerInfo const *impl_GetPlayerInfo(
			u32 playerId) const;
		SoundArchiveFile::GroupInfo const *impl_GetGroupInfo(u32 groupId) const;
		SoundArchiveFile::GroupItemInfo const *impl_GetGroupItemInfo(
			u32 groupId) const;

		bool impl_GetSoundInfoOffset(
			u32 id, SoundArchiveFile::SoundInfoOffset *soundInfoRef) const;

	// static members
	public:
		static byte4_t const SIGNATURE = 'RSAR';
		static int const VERSION = NW4R_VERSION(1, 4);

	// members
	private:
		SoundArchiveFile::Header		mHeader;					// size 0x28, offset 0x00
		SoundArchiveFile::Info			const *mInfo;				// size 0x04, offset 0x28
		void							const *mStringBase;			// size 0x04, offset 0x2c
		SoundArchiveFile::StringTable	const *mStringTable;		// size 0x04, offset 0x30
		SoundArchiveFile::StringTree	const *mStringTreeSound;	// size 0x04, offset 0x34
		SoundArchiveFile::StringTree	const *mStringTreePlayer;	// size 0x04, offset 0x38
		SoundArchiveFile::StringTree	const *mStringTreeGroup;	// size 0x04, offset 0x3c
		SoundArchiveFile::StringTree	const *mStringTreeBank;		// size 0x04, offset 0x40
	}; // size 0x44
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_SOUND_ARCHIVE_FILE_H
