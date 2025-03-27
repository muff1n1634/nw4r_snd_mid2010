#ifndef NW4R_SND_SOUND_ARCHIVE_H
#define NW4R_SND_SOUND_ARCHIVE_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "global.h"

/*******************************************************************************
 * types
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail { class SoundArchiveFileReader; }}}

namespace nw4r { namespace ut { class FileStream; }}

namespace nw4r { namespace snd
{
	enum SoundType
	{
		SOUND_TYPE_INVALID,
		SOUND_TYPE_SEQ,
		SOUND_TYPE_STRM,
		SOUND_TYPE_WAVE,
	};
}} // namespace nw4r::snd

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd
{
	class SoundArchive
	{
	// nested types
	public:
		struct BankInfo
		{
			u32	fileId;	// size 0x04, offset 0x00
		}; // size 0x04

		struct FileInfo
		{
			u32			fileSize;			// size 0x04, offset 0x00
			u32			waveDataFileSize;	// size 0x04, offset 0x04
			char const	*extFilePath;		// size 0x04, offset 0x08
			u32			filePosCount;		// size 0x04, offset 0x0c
		}; // size 0x10

		struct FilePos
		{
			u32	groupId;	// size 0x04, offset 0x00
			u32	index;		// size 0x04, offset 0x04
		}; // size 0x08

		struct GroupInfo
		{
			u32			itemCount;		// size 0x04, offset 0x00
			char const	*extFilePath;	// size 0x04, offset 0x04
			u32			offset;			// size 0x04, offset 0x08
			u32			size;			// size 0x04, offset 0x0c
			u32			waveDataOffset;	// size 0x04, offset 0x10
			u32			waveDataSize;	// size 0x04, offset 0x14
		}; // size 0x18

		struct GroupItemInfo
		{
			u32	fileId;			// size 0x04, offset 0x00
			u32	offset;			// size 0x04, offset 0x04
			u32	size;			// size 0x04, offset 0x08
			u32	waveDataOffset;	// size 0x04, offset 0x0c
			u32	waveDataSize;	// size 0x04, offset 0x10
		}; // size 0x14

		struct PlayerInfo
		{
			int	playableSoundCount;	// size 0x04, offset 0x00
			u32	heapSize;			// size 0x04, offset 0x04
		}; // size 0x08

		struct SeqSoundInfo
		{
			u32		dataOffset;				// size 0x04, offset 0x00
			u32		bankId;					// size 0x04, offset 0x04
			u32		allocTrack;				// size 0x04, offset 0x08
			int		channelPriority;		// size 0x04, offset 0x0c
			bool	releasePriorityFixFlag;	// size 0x01, offset 0x10
			/* 3 bytes padding */
		}; // size 0x14

		struct SoundArchivePlayerInfo
		{
			int	seqSoundCount;		// size 0x04, offset 0x00
			int	seqTrackCount;		// size 0x04, offset 0x04
			int	strmSoundCount;		// size 0x04, offset 0x08
			int	strmTrackCount;		// size 0x04, offset 0x0c
			int	strmChannelCount;	// size 0x04, offset 0x10
			int	waveSoundCount;		// size 0x04, offset 0x14
			int	waveTrackCount;		// size 0x04, offset 0x18
		}; // size 0x1c

		// no ketteiban dwarf, and I don't need the members here
		struct Sound3DParam {};

		struct SoundInfo
		{
			u32			fileId;			// size 0x04, offset 0x00
			u32			playerId;		// size 0x04, offset 0x04
			int			actorPlayerId;	// size 0x04, offset 0x08
			int			playerPriority;	// size 0x04, offset 0x0c
			int			volume;			// size 0x04, offset 0x10
			int			remoteFilter;	// size 0x04, offset 0x14
			PanMode		panMode;		// size 0x04, offset 0x18
			PanCurve	panCurve;		// size 0x04, offset 0x1c
		}; // size 0x20

		struct StrmSoundInfo
		{
			u32	startPosition;		// size 0x04, offset 0x00
			u16	allocChannelCount;	// size 0x02, offset 0x04
			u16	allocTrackFlag;		// size 0x02, offset 0x06
		}; // size 0x08

		struct WaveSoundInfo
		{
			s32		subNo;					// size 0x04, offset 0x00
			int		channelPriority;		// size 0x04, offset 0x04
			bool	releasePriorityFixFlag;	// size 0x01, offset 0x08
			/* 3 bytes padding */
		}; // size 0x0c

	// methods
	public:
		// cdtors
		SoundArchive();
		virtual ~SoundArchive();

		// virtual function ordering
		// vtable SoundArchive
		virtual const void *detail_GetFileAddress(u32 id) const = 0;
		virtual const void *detail_GetWaveDataFileAddress(u32 id) const = 0;
		virtual int detail_GetRequiredStreamBufferSize() const = 0;
		virtual ut::FileStream *OpenStream(void *buffer, int size, u32 begin,
		                                   u32 length) const = 0;
		virtual ut::FileStream *OpenExtStream(void *buffer, int size,
		                                      const char *extFilePath,
		                                      u32 begin, u32 length) const = 0;

		// methods
		void Setup(detail::SoundArchiveFileReader *fileReader);
		void Shutdown();

		u32 GetPlayerCount() const;
		u32 GetGroupCount() const;
		u32 ConvertLabelStringToSoundId(const char *label) const;
		SoundType GetSoundType(u32 soundId) const;
		bool ReadSoundInfo(u32 soundId, SoundInfo *info) const;
		bool ReadSeqSoundInfo(u32 soundId, SeqSoundInfo *info) const;
		bool detail_ReadStrmSoundInfo(u32 soundId, StrmSoundInfo *info) const;
		bool detail_ReadWaveSoundInfo(u32 soundId, WaveSoundInfo *info) const;
		bool ReadPlayerInfo(u32 playerId, PlayerInfo *info) const;
		bool ReadSoundArchivePlayerInfo(SoundArchivePlayerInfo *info) const;
		bool ReadBankInfo(u32 bankId, BankInfo *info) const;
		bool detail_ReadGroupInfo(u32 groupId, GroupInfo *info) const;
		bool detail_ReadGroupItemInfo(u32 groupId, u32 index,
		                              GroupItemInfo *info) const;
		u32 detail_GetFileCount() const;
		bool detail_ReadFileInfo(u32 id, FileInfo *info) const;
		bool detail_ReadFilePos(u32 fileId, u32 posId, FilePos *pos) const;

		ut::FileStream *detail_OpenFileStream(u32 fileId, void *buffer,
		                                      int size) const;
		ut::FileStream *OpenExtStreamImpl(void *buffer, int size,
		                                  const char *extFilePath, u32 begin,
		                                  u32 length) const;

		bool IsAvailable() const;

	// static members
	public:
		static const int FILE_PATH_MAX = 256;

	// members
	private:
		/* vtable */													// size 0x004, offset 0x000
		detail::SoundArchiveFileReader	*mFileReader;					// size 0x004, offset 0x004
		char							mExtFileRoot[FILE_PATH_MAX];	// size 0x100, offset 0x008
	}; // size 0x108
}} // namespace nw4r::snd

#endif // NW4R_SND_SOUND_ARCHIVE_H
