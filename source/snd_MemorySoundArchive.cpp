#include "nw4r/snd/MemorySoundArchive.h"

/* Original source:
 * kiwi515/ogws
 * src/nw4r/snd/snd_MemorySoundArchive.cpp
 */

/*******************************************************************************
 * headers
 */

#include <cstring> // std::memcpy
#include <new>

#include <types.h>

#include "nw4r/snd/SoundArchive.h"
#include "nw4r/snd/SoundArchiveFile.h" // SoundArchiveFileReader

#include "nw4r/ut/FileStream.h"
#include "nw4r/ut/inlines.h"

#include "nw4r/NW4RAssert.hpp"

/*******************************************************************************
 * functions
 */

namespace nw4r { namespace snd {

MemorySoundArchive::MemorySoundArchive() :
	mData(nullptr)
{}

MemorySoundArchive::~MemorySoundArchive() {}

bool MemorySoundArchive::Setup(void const *soundArchiveData)
{
	NW4RAssertPointerNonnull_Line(65, soundArchiveData);
	NW4RAssertAligned_Line(66, soundArchiveData, 4);

	mFileReader.Init(soundArchiveData);
	SoundArchive::Setup(&mFileReader);

	void const *infoChunk =
		ut::AddOffsetToPtr(soundArchiveData, mFileReader.GetInfoChunkOffset());

	mFileReader.SetInfoChunk(infoChunk, mFileReader.GetInfoChunkSize());

	void const *stringChunk = ut::AddOffsetToPtr(
		soundArchiveData, mFileReader.GetLabelStringChunkOffset());

	mFileReader.SetStringChunk(stringChunk,
	                           mFileReader.GetLabelStringChunkSize());

	mData = soundArchiveData;
	return true;
}

void MemorySoundArchive::Shutdown()
{
	mData = nullptr;
	SoundArchive::Shutdown();
}

void const *MemorySoundArchive::detail_GetFileAddress(u32 id) const
{
	SoundArchive::FilePos pos;
	if (!detail_ReadFilePos(id, 0, &pos))
		return nullptr;

	SoundArchive::GroupInfo groupInfo;
	if (!detail_ReadGroupInfo(pos.groupId, &groupInfo))
		return nullptr;

	SoundArchive::GroupItemInfo itemInfo;
	if (!detail_ReadGroupItemInfo(pos.groupId, pos.index, &itemInfo))
		return nullptr;

	if (!groupInfo.extFilePath)
		return ut::AddOffsetToPtr(mData, groupInfo.offset + itemInfo.offset);
	else
		return nullptr;
}

void const *MemorySoundArchive::detail_GetWaveDataFileAddress(u32 id) const
{
	SoundArchive::FilePos pos;
	if (!detail_ReadFilePos(id, 0, &pos))
		return nullptr;

	SoundArchive::GroupInfo groupInfo;
	if (!detail_ReadGroupInfo(pos.groupId, &groupInfo))
		return nullptr;

	SoundArchive::GroupItemInfo itemInfo;
	if (!detail_ReadGroupItemInfo(pos.groupId, pos.index, &itemInfo))
		return nullptr;

	if (!groupInfo.extFilePath)
	{
		return ut::AddOffsetToPtr(mData, groupInfo.waveDataOffset
		                                     + itemInfo.waveDataOffset);
	}
	else
	{
		return nullptr;
	}
}

MemorySoundArchive::MemoryFileStream::MemoryFileStream(void const *buffer,
                                                       u32 size) :
	mData	(buffer),
	mSize	(size),
	mOffset	(0)
{}

ut::FileStream *MemorySoundArchive::OpenStream(void *buffer, int size,
                                               u32 offset, u32 length) const
{
	if (!mData)
		return nullptr;

	if (size < sizeof(MemoryFileStream))
		return nullptr;

	return new (buffer)
		MemoryFileStream(ut::AddOffsetToPtr(mData, offset), length);
}

ut::FileStream *MemorySoundArchive::OpenExtStream(void *, int, char const *,
                                                  u32, u32) const
{
	NW4RWarningMessage_Line(187,
	                        "Cannot OpenExtStream for MemorySoundArchive\n");

	return nullptr;
}

int MemorySoundArchive::detail_GetRequiredStreamBufferSize() const
{
	return sizeof(MemoryFileStream);
}

void MemorySoundArchive::MemoryFileStream::Close()
{
	mData = nullptr;
	mSize = 0;
	mOffset = 0;
}

s32 MemorySoundArchive::MemoryFileStream::Read(void *dst, u32 size)
{
	u32 bytesRead = ut::Min<u32>(size, mSize - mOffset);
	std::memcpy(dst, ut::AddOffsetToPtr(mData, mOffset), bytesRead);

	return bytesRead;
}

void MemorySoundArchive::MemoryFileStream::Seek(s32 offset, u32 origin)
{
	switch (origin)
	{
	case FileStream::SEEK_ORIGIN_SET:
		mOffset = offset;
		break;

	case FileStream::SEEK_ORIGIN_CUR:
		mOffset += offset;
		break;

	case FileStream::SEEK_ORIGIN_END:
		mOffset = mSize - offset;
		break;

	default:
		NW4RPanicMessage_Line(234, "Unsupported Seek origin");
		break;
	}
}

}} // namespace nw4r::snd
