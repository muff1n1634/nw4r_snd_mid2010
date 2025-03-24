#ifndef NW4R_SND_MEMORY_SOUND_ARCHIVE_H
#define NW4R_SND_MEMORY_SOUND_ARCHIVE_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "SoundArchive.h"
#include "SoundArchiveFile.h"

#include "../ut/FileStream.h"

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd
{
	class MemorySoundArchive : public SoundArchive
	{
	// nested types
	private:
		class MemoryFileStream;

	// methods
	public:
		// cdtors
		MemorySoundArchive();
		virtual ~MemorySoundArchive();

		// virtual function ordering
		// vtable SoundArchive
		virtual const void *detail_GetFileAddress(u32 id) const;
		virtual const void *detail_GetWaveDataFileAddress(u32 id) const;
		virtual int detail_GetRequiredStreamBufferSize() const;
		virtual ut::FileStream *OpenStream(void *pBuffer, int size, u32 offset,
		                                   u32 length) const;
		virtual ut::FileStream *OpenExtStream(void *pBuffer, int size,
		                                      const char *pExtPath, u32 offset,
		                                      u32 length) const;

		// methods
		bool Setup(const void *soundArchiveData);
		void Shutdown();

	// members
	private:
		/* base SoundArchive */							// size 0x108, offset 0x000
		const void						*mData;			// size 0x004, offset 0x108
		detail::SoundArchiveFileReader	mFileReader;	// size 0x044, offset 0x10c
	}; // size 0x150

	// NOTE: Must be completed after MemorySoundArchive for data ordering
	class MemorySoundArchive::MemoryFileStream : public ut::FileStream
	{
	// methods
	public:
		// cdtors
		MemoryFileStream(const void *buffer, u32 size);
		virtual ~MemoryFileStream() {}

		// weak instantiation ordering
		virtual void Close();
		virtual s32 Read(void *dst, u32 size);
		virtual void Seek(s32 offset, u32 origin);
		virtual bool CanSeek() const { return true; }
		virtual bool CanCancel() const { return true; }
		virtual bool CanAsync() const { return false; }
		virtual bool CanRead() const { return true; }
		virtual bool CanWrite() const { return false; }
		virtual u32 Tell() const { return mOffset; }
		virtual u32 GetSize() const { return mSize; }

	// members
	private:
		/* base FileStream */	// size 0x14, offset 0x00
		const void	*mData;		// size 0x04, offset 0x14
		s32			mSize;		// size 0x04, offset 0x18
		s32			mOffset;	// size 0x04, offset 0x1c
	}; // size 0x20
}} // namespace nw4r::snd

#endif // NW4R_SND_MEMORY_SOUND_ARCHIVE_H
