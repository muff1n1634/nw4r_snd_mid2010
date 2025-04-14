#include "nw4r/snd/SoundArchiveFile.h"

/* Original source:
 * kiwi515/ogws
 * src/nw4r/snd/snd_SoundArchiveFile.cpp
 */

/*******************************************************************************
 * headers
 */

#include <nw4r/NW4RAssert.hpp>

#include <cstring>

#include <macros.h>
#include <types.h>

#include "nw4r/snd/global.h"
#include "nw4r/snd/SoundArchive.h"
#include "nw4r/snd/Util.h"

#include "nw4r/ut/binaryFileFormat.h"
#include "nw4r/ut/inlines.h"

/*******************************************************************************
 * functions
 */

namespace nw4r { namespace snd { namespace detail {

SoundArchiveFileReader::SoundArchiveFileReader():
	mInfo				(nullptr),
	mStringBase			(nullptr),
	mStringTable		(nullptr),
	mStringTreeSound	(nullptr),
	mStringTreePlayer	(nullptr),
	mStringTreeGroup	(nullptr),
	mStringTreeBank		(nullptr)
	{}

void SoundArchiveFileReader::Init(void const *soundArchiveData)
{
	NW4RAssertPointerNonnull_Line(50, soundArchiveData);

	if (IsValidFileHeader(soundArchiveData))
	{
		mHeader = *static_cast<SoundArchiveFile::Header const *>(
			soundArchiveData);
	}
}

bool SoundArchiveFileReader::IsValidFileHeader(void const *soundArchiveData)
{
	ut::BinaryFileHeader const *fileHeader =
		static_cast<ut::BinaryFileHeader const *>(soundArchiveData);

	NW4RAssertMessage_Line(
		75, fileHeader->signature == SIGNATURE,
		"invalid file signature. sound archive data is not available.");

	if (fileHeader->signature != SIGNATURE)
		return false;


	NW4RAssertMessage_Line(83, fileHeader->version >= NW4R_VERSION(1, 0),
	            "sound archive file is not supported version.\n  please "
	            "reconvert file using new version tools.\n");

	if (fileHeader->version < NW4R_VERSION(1, 0))
		return false;

	NW4RAssertMessage_Line(89, fileHeader->version <= VERSION,
	            "sound archive file is not supported version.\n  please "
	            "reconvert file using new version tools.\n");

	if (fileHeader->version > VERSION)
		return false;

	return true;
}

void SoundArchiveFileReader::SetStringChunk(void const *stringChunk,
                                            u32 stringChunkSize ATTR_UNUSED)
{
	NW4RAssertPointerNonnull_Line(99, stringChunk);

	SoundArchiveFile::SymbolBlock const *symbolBlock =
		static_cast<SoundArchiveFile::SymbolBlock const *>(stringChunk);
	NW4RAssert_Line(102, symbolBlock->blockHeader.kind
	                         == SoundArchiveFile::SIGNATURE_SYMB_BLOCK);

	SoundArchiveFile::StringBlock const *stringBlock =
		&symbolBlock->stringBlock;

	mStringBase = stringBlock;

	mStringTable = static_cast<SoundArchiveFile::StringTable const *>(
		GetPtrConst(mStringBase, stringBlock->stringChunk.tableOffset));

	mStringTreeSound = static_cast<SoundArchiveFile::StringTree const *>(
		GetPtrConst(mStringBase, stringBlock->stringChunk.soundTreeOffset));

	mStringTreePlayer = static_cast<SoundArchiveFile::StringTree const *>(
		GetPtrConst(mStringBase, stringBlock->stringChunk.playerTreeOffset));

	mStringTreeGroup = static_cast<SoundArchiveFile::StringTree const *>(
		GetPtrConst(mStringBase, stringBlock->stringChunk.groupTreeOffset));

	mStringTreeBank = static_cast<SoundArchiveFile::StringTree const *>(
		GetPtrConst(mStringBase, stringBlock->stringChunk.bankTreeOffset));
}

void SoundArchiveFileReader::SetInfoChunk(void const *infoChunk,
                                          u32 infoChunkSize ATTR_UNUSED)
{
	NW4RAssertPointerNonnull_Line(123, infoChunk);

	SoundArchiveFile::InfoBlock const *infoBlock =
		static_cast<SoundArchiveFile::InfoBlock const *>(infoChunk);
	NW4RAssert_Line(126, infoBlock->blockHeader.kind
	                       == SoundArchiveFile::SIGNATURE_INFO_BLOCK);

	mInfo = &infoBlock->info;
}

SoundType SoundArchiveFileReader::GetSoundType(u32 soundId) const
{
	SoundType soundType;

	SoundArchiveFile::SoundCommonInfo::Table const *table =
		Util::GetDataRefAddress0(mInfo->soundTableRef, mInfo);

	if (!table)
		return SOUND_TYPE_INVALID;

	if (soundId >= table->count)
		return SOUND_TYPE_INVALID;

	if (GetVersion() >= NW4R_VERSION(1, 1))
	{
		SoundArchiveFile::SoundCommonInfo const *soundCommonInfo =
			Util::GetDataRefAddress0(table->items[soundId], mInfo);

		if (!soundCommonInfo)
			return SOUND_TYPE_INVALID;

		soundType = static_cast<SoundType>(soundCommonInfo->soundType);
	}
	else
	{
		soundType = static_cast<SoundType>(table->items[soundId].dataType);
	}

	switch (soundType)
	{
	case SOUND_TYPE_SEQ:
		return SOUND_TYPE_SEQ;

	case SOUND_TYPE_STRM:
		return SOUND_TYPE_STRM;

	case SOUND_TYPE_WAVE:
		return SOUND_TYPE_WAVE;

	default:
		return SOUND_TYPE_INVALID;
	}
}

bool SoundArchiveFileReader::ReadSoundInfo(u32 soundId,
                                           SoundArchive::SoundInfo *info) const
{
	SoundArchiveFile::SoundCommonInfo const *src = impl_GetSoundInfo(soundId);
	if (!src)
		return false;

	info->fileId			= src->fileId;
	info->playerId			= src->playerId;
	info->actorPlayerId		= src->actorPlayerId;
	info->playerPriority	= src->playerPriority;
	info->volume			= src->volume;
	info->remoteFilter		= src->remoteFilter;

	if (GetVersion() >= NW4R_VERSION(1, 2))
	{
		info->panMode = static_cast<PanMode>(src->panMode);
		info->panCurve = static_cast<PanCurve>(src->panCurve);
	}
	else
	{
		info->panMode = PAN_MODE_BALANCE;
		info->panCurve = PAN_CURVE_SQRT;
	}

	return true;
}

bool SoundArchiveFileReader::ReadSeqSoundInfo(
	u32 soundId, SoundArchive::SeqSoundInfo *info) const
{
	SoundArchiveFile::SeqSoundInfo const *src = impl_GetSeqSoundInfo(soundId);
	if (!src)
		return false;

	info->dataOffset		= src->dataOffset;
	info->bankId			= src->bankId;
	info->channelPriority	= src->channelPriority;
	info->allocTrack		= src->allocTrack;

	if (GetVersion() >= NW4R_VERSION(1, 3))
		info->releasePriorityFixFlag = src->releasePriorityFix;
	else
		info->releasePriorityFixFlag = false;

	return true;
}

bool SoundArchiveFileReader::ReadStrmSoundInfo(
	u32 soundId, SoundArchive::StrmSoundInfo *info) const
{
	SoundArchiveFile::StrmSoundInfo const *src = impl_GetStrmSoundInfo(soundId);
	if (!src)
		return false;

	info->startPosition = src->startPosition;
	info->allocTrackFlag = src->allocTrackFlag;

	if (GetVersion() >= NW4R_VERSION(1, 4))
	{
		info->allocChannelCount = src->allocChannelCount;
	}
	else
	{
		info->allocChannelCount = 0;

		// is this meant to be src->allocTrackFlag?
		for (byte2_t bitflag = src->allocChannelCount; bitflag; bitflag >>= 1)
		{
			if (bitflag & 1)
				info->allocChannelCount++;
			else if (bitflag)
				return false;
		}
	}

	return true;
}

bool SoundArchiveFileReader::ReadWaveSoundInfo(
	u32 soundId, SoundArchive::WaveSoundInfo *info) const
{
	SoundArchiveFile::WaveSoundInfo const *src = impl_GetWaveSoundInfo(soundId);
	if (!src)
		return false;

	info->subNo = src->subNo;
	info->channelPriority = src->channelPriority;

	if (GetVersion() >= NW4R_VERSION(1, 3))
		info->releasePriorityFixFlag = src->releasePriorityFix;
	else
		info->releasePriorityFixFlag = false;

	return true;
}

bool SoundArchiveFileReader::ReadBankInfo(u32 bankId,
                                          SoundArchive::BankInfo *info) const
{
	SoundArchiveFile::BankInfo const *src = impl_GetBankInfo(bankId);
	if (!src)
		return false;

	info->fileId = src->fileId;

	return true;
}

bool SoundArchiveFileReader::ReadPlayerInfo(
	u32 playerId, SoundArchive::PlayerInfo *info) const
{
	SoundArchiveFile::PlayerInfo const *src = impl_GetPlayerInfo(playerId);
	if (!src)
		return false;

	info->playableSoundCount	= src->playableSoundCount;
	info->heapSize				= src->heapSize;

	return true;
}

bool SoundArchiveFileReader::ReadGroupInfo(u32 groupId,
                                           SoundArchive::GroupInfo *info) const
{
	SoundArchiveFile::GroupInfo const *src = impl_GetGroupInfo(groupId);
	if (!src)
		return false;

	SoundArchiveFile::GroupItemInfo::Table const *itemTable =
		Util::GetDataRefAddress0(src->itemTableRef, mInfo);
	if (!itemTable)
		return false;

	info->extFilePath =
		Util::GetDataRefAddress0(src->extFilePathRef, mInfo);
	info->offset			= src->offset;
	info->size				= src->size;
	info->waveDataOffset	= src->waveDataOffset;
	info->waveDataSize		= src->waveDataSize;
	info->itemCount			= itemTable->count;

	return true;
}

bool SoundArchiveFileReader::ReadGroupItemInfo(
	u32 groupId, u32 index, SoundArchive::GroupItemInfo *info) const
{
	SoundArchiveFile::GroupInfo const *groupInfo = impl_GetGroupInfo(groupId);
	if (!groupInfo)
		return false;

	SoundArchiveFile::GroupItemInfo::Table const *itemTable =
		Util::GetDataRefAddress0(groupInfo->itemTableRef, mInfo);
	if (!itemTable)
		return false;

	if (index >= itemTable->count)
		return false;

	SoundArchiveFile::GroupItemInfo const *src =
		Util::GetDataRefAddress0(itemTable->items[index], mInfo);
	if (!src)
		return false;

	info->fileId			= src->fileId;
	info->offset			= src->offset;
	info->size				= src->size;
	info->waveDataOffset	= src->waveDataOffset;
	info->waveDataSize		= src->waveDataSize;

	return true;
}

bool SoundArchiveFileReader::ReadSoundArchivePlayerInfo(
	SoundArchive::SoundArchivePlayerInfo *info) const
{
	SoundArchiveFile::SoundArchivePlayerInfo const *src =
		Util::GetDataRefAddress0(mInfo->soundArchivePlayerInfoRef, mInfo);

	// ERRATUM: check info instead of src like the other functions
	if (!info)
		return false;

	info->seqSoundCount		= src->seqSoundCount;
	info->seqTrackCount		= src->seqTrackCount;
	info->strmSoundCount	= src->strmSoundCount;
	info->strmTrackCount	= src->strmTrackCount;
	info->strmChannelCount	= src->strmChannelCount;
	info->waveSoundCount	= src->waveSoundCount;
	info->waveTrackCount	= src->waveTrackCount;

	return true;
}

u32 SoundArchiveFileReader::GetPlayerCount() const
{
	SoundArchiveFile::PlayerInfo::Table const *table =
		Util::GetDataRefAddress0(mInfo->playerTableRef, mInfo);
	if (!table)
		return false;

	return table->count;
}

u32 SoundArchiveFileReader::GetGroupCount() const
{
	SoundArchiveFile::GroupInfo::Table const *table =
		Util::GetDataRefAddress0(mInfo->groupTableRef, mInfo);
	if (!table)
		return false;

	return table->count - 1;
}

// NOTE: Deadstripping
u32 SoundArchiveFileReader::GetBankCount() const
{
	(void)Util::GetDataRefAddress0(mInfo->bankTableRef, mInfo);
	return 0;
}

u32 SoundArchiveFileReader::GetFileCount() const
{
	SoundArchiveFile::FileInfo::Table const *table =
		Util::GetDataRefAddress0(mInfo->fileTableRef, mInfo);
	if (!table)
		return false;

	return table->count;
}

bool SoundArchiveFileReader::ReadFileInfo(u32 fileId,
                                          SoundArchive::FileInfo *info) const
{
	SoundArchiveFile::FileInfo::Table const *table =
		Util::GetDataRefAddress0(mInfo->fileTableRef, mInfo);
	if (!table)
		return false;

	if (fileId >= table->count)
		return false;

	SoundArchiveFile::FileInfo const *fileInfo =
		Util::GetDataRefAddress0(table->items[fileId], mInfo);
	if (!fileInfo)
		return false;

	SoundArchiveFile::FilePos::Table const *filePosTable =
		Util::GetDataRefAddress0(fileInfo->filePosTableRef, mInfo);
	if (!filePosTable)
		return false;

	info->fileSize			= fileInfo->fileSize;
	info->waveDataFileSize	= fileInfo->waveDataFileSize;
	info->extFilePath =
		Util::GetDataRefAddress0(fileInfo->extFilePathRef, mInfo);
	info->filePosCount		= filePosTable->count;

	return true;
}

bool SoundArchiveFileReader::ReadFilePos(u32 fileId, u32 index,
                                         SoundArchive::FilePos *filePos) const
{
	SoundArchiveFile::FileInfo::Table const *table =
		Util::GetDataRefAddress0(mInfo->fileTableRef, mInfo);
	if (!table)
		return false;

	if (fileId >= table->count)
		return false;

	SoundArchiveFile::FileInfo const *fileInfo =
		Util::GetDataRefAddress0(table->items[fileId], mInfo);
	if (!fileInfo)
		return false;

	SoundArchiveFile::FilePos::Table const *filePosTable =
		Util::GetDataRefAddress0(fileInfo->filePosTableRef, mInfo);
	if (!filePosTable)
		return false;

	if (index >= filePosTable->count)
		return false;

	SoundArchive::FilePos const *pos =
		Util::GetDataRefAddress0(filePosTable->items[index], mInfo);

	if (!pos)
		return false;

	*filePos = *pos;

	return true;
}

char const *SoundArchiveFileReader::GetString(u32 id) const
{
	if (id == -1)
		return nullptr;

	if (!mStringTable)
		return nullptr;

	NW4RAssert_Line(508, id < mStringTable->offsetTable.count);

	char const *str = static_cast<char const *>(
		GetPtrConst(mStringBase, mStringTable->offsetTable.items[id]));

	return str;
}

u32 SoundArchiveFileReader::ConvertLabelStringToId(
	SoundArchiveFile::StringTree const *tree, char const *str) const
{
	if (!tree)
		return 0xffffffff;

	if (tree->rootIdx >= tree->nodeTable.count)
		return 0xffffffff;

	SoundArchiveFile::StringTreeNode const *node =
		&tree->nodeTable.items[tree->rootIdx];

	u32 strlen = std::strlen(str);

	while (!(node->flags & 1))
	{
		int pos = node->bit >> 3;
		int bit = node->bit & 7;

		u32 nodeIdx;
		if (pos < static_cast<int>(strlen) && (1 << (7 - bit)) & str[pos])
			nodeIdx = node->rightIdx;
		else
			nodeIdx = node->leftIdx;

		node = &tree->nodeTable.items[nodeIdx];
	}

	char const *str_cmp = GetString(node->strIdx);

	if (std::strcmp(str, str_cmp) == 0)
		return node->id;
	else
		return -1;
}

SoundArchiveFile::SoundCommonInfo const *
SoundArchiveFileReader::impl_GetSoundInfo(u32 soundId) const
{
	SoundArchiveFile::SoundCommonInfo::Table const *table =
		Util::GetDataRefAddress0(mInfo->soundTableRef, mInfo);

	if (!table)
		return nullptr;

	if (soundId >= table->count)
		return nullptr;

	if (GetVersion() >= NW4R_VERSION(1, 1))
	{
		return Util::GetDataRefAddress0(table->items[soundId], mInfo);
	}
	else
	{
		return static_cast<SoundArchiveFile::SoundCommonInfo const *>(
			ut::AddOffsetToPtr(mInfo, table->items[soundId].value));
	}
}

bool SoundArchiveFileReader::impl_GetSoundInfoOffset(
	u32 id, SoundArchiveFile::SoundInfoOffset *soundInfoRef) const
{
	SoundArchiveFile::SoundCommonInfo::Table const *table =
		Util::GetDataRefAddress0(mInfo->soundTableRef, mInfo);

	if (!table)
		return false;

	if (id >= table->count)
		return false;

	if (GetVersion() >= NW4R_VERSION(1, 1))
	{
		SoundArchiveFile::SoundCommonInfo const *soundCommonInfo =
			Util::GetDataRefAddress0(table->items[id], mInfo);

		if (!soundCommonInfo)
			return false;

		*soundInfoRef = soundCommonInfo->soundInfoRef;

		return true;
	}
	else
	{
		SoundArchiveFile::SoundInfoOffset ref;

		// TODO (from ogws): Why is the offset changed?
		ref.refType = table->items[id].refType;
		ref.dataType = table->items[id].dataType;
		ref.value = table->items[id].value + 0x1c;

		*soundInfoRef = ref;

		return true;
	}
}

SoundArchiveFile::SeqSoundInfo const *
SoundArchiveFileReader::impl_GetSeqSoundInfo(u32 soundId) const
{
	if (GetSoundType(soundId) != SOUND_TYPE_SEQ)
		return nullptr;

	SoundArchiveFile::SoundInfoOffset dataRef;
	if (!impl_GetSoundInfoOffset(soundId, &dataRef))
		return nullptr;

	return Util::GetDataRefAddress1(dataRef, mInfo);
}

SoundArchiveFile::StrmSoundInfo const *
SoundArchiveFileReader::impl_GetStrmSoundInfo(u32 soundId) const
{
	if (GetSoundType(soundId) != SOUND_TYPE_STRM)
		return nullptr;

	SoundArchiveFile::SoundInfoOffset dataRef;
	if (!impl_GetSoundInfoOffset(soundId, &dataRef))
		return nullptr;

	return Util::GetDataRefAddress2(dataRef, mInfo);
}

SoundArchiveFile::WaveSoundInfo const *
SoundArchiveFileReader::impl_GetWaveSoundInfo(u32 soundId) const
{
	if (GetSoundType(soundId) != SOUND_TYPE_WAVE)
		return nullptr;

	SoundArchiveFile::SoundInfoOffset dataRef;
	if (!impl_GetSoundInfoOffset(soundId, &dataRef))
		return nullptr;

	return Util::GetDataRefAddress3(dataRef, mInfo);
}

SoundArchiveFile::BankInfo const *SoundArchiveFileReader::impl_GetBankInfo(
	u32 bankId) const
{
	SoundArchiveFile::BankInfo::Table const *table =
		Util::GetDataRefAddress0(mInfo->bankTableRef, mInfo);

	if (!table)
		return nullptr;

	if (bankId >= table->count)
		return nullptr;

	return Util::GetDataRefAddress0(table->items[bankId], mInfo);
}

SoundArchiveFile::PlayerInfo const *SoundArchiveFileReader::impl_GetPlayerInfo(
	u32 playerId) const
{
	SoundArchiveFile::PlayerInfo::Table const *table =
		Util::GetDataRefAddress0(mInfo->playerTableRef, mInfo);

	if (!table)
		return nullptr;

	if (playerId >= table->count)
		return nullptr;

	return Util::GetDataRefAddress0(table->items[playerId], mInfo);
}

SoundArchiveFile::GroupInfo const *SoundArchiveFileReader::impl_GetGroupInfo(
	u32 playerId) const
{
	SoundArchiveFile::GroupInfo::Table const *table =
		Util::GetDataRefAddress0(mInfo->groupTableRef, mInfo);

	if (!table)
		return nullptr;

	if (playerId >= table->count)
		return nullptr;

	return Util::GetDataRefAddress0(table->items[playerId], mInfo);
}

}}} // namespace nw4r::snd::detail
