#ifndef NW4R_SND_SOUND_ARCHIVE_PLAYER_H
#define NW4R_SND_SOUND_ARCHIVE_PLAYER_H

/*******************************************************************************
 * headers
 */

#include <types.h>

// WARNING: DO NOT REORDER these #include directives, data pooling depends on it

// clang-format off
#include "BasicSound.h"
#include "DisposeCallbackManager.h"
#include "MmlParser.h"
#include "NoteOnCallback.h" // specifically this
#include "MmlSeqTrackAllocator.h" // being before this
#include "SeqPlayer.h"
#include "SeqSound.h"
#include "SoundArchive.h"
#include "SoundInstanceManager.h"
#include "WsdPlayer.h" // and this
#include "SoundStartable.h" // being before this
#include "StrmChannel.h"
#include "StrmSound.h"
#include "Util.h"
#include "WaveSound.h"
// clang-format on

#include <nw4r/NW4RAssert.hpp>

/*******************************************************************************
 * classes and functions
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail { class PlayerHeap; }}}
namespace nw4r { namespace snd { namespace detail { class SeqTrackAllocator; }}}
namespace nw4r { namespace snd { class SoundActor; }}
namespace nw4r { namespace snd { class SoundHandle; }}
namespace nw4r { namespace snd { class SoundPlayer; }}

namespace nw4r { namespace snd
{
	class SoundArchivePlayer_FileManager
	{
	// methods
	public:
		// virtual function ordering
		// vtable SoundArchivePlayer_FileManager
		virtual const void *GetFileAddress(u32 fileId) = 0;
		virtual const void *GetFileWaveDataAddress(u32 fileId) = 0;

	// members
	private:
		/* vtable */	// size 0x04, offset 0x00
	}; // size 0x04

	class SoundArchivePlayer : public detail::DisposeCallback,
	                           public SoundStartable
	{
	// early methods
	public:
		// cdtors
		SoundArchivePlayer();
		virtual ~SoundArchivePlayer();

		// virtual function ordering
		// vtable DisposeCallback
		virtual void InvalidateData(const void *start, const void *end);
		virtual void InvalidateWaveData(const void *start, const void *end);
		virtual StartResult detail_SetupSound(SoundHandle *handle, u32 soundId,
		                                      bool holdFlag,
		                                      const StartInfo *startInfo);

		// vtable SoundStartable
		virtual u32 detail_ConvertLabelStringToSoundId(const char *label)
		{
			// specifically not the header version
			NW4RAssertPointerNonnull_FileLine("SoundArchivePlayer.h", 194,
			                                  mSoundArchive);

			return mSoundArchive->ConvertLabelStringToSoundId(label);
		}

	// nested types
	private:
		struct GroupAddress
		{
		// typedefs
		public:
			typedef detail::Util::Table<GroupAddress> Table;

		// members
		public:
			const void	*address;			// size 0x04, offset 0x00
			const void	*waveDataAddress;	// size 0x04, offset 0x04
		}; // size 0x08

		struct FileAddress
		{
		// typedefs
		public:
			typedef detail::Util::Table<FileAddress> Table;

		// members
		public:
			const void	*address;			// size 0x04, offset 0x00
			const void	*waveDataAddress;	// size 0x04, offset 0x04
		}; // size 0x08

		class SeqNoteOnCallback : public detail::NoteOnCallback
		{
		// methods
		public:
			// cdtors
			SeqNoteOnCallback(const SoundArchivePlayer &player):
				mSoundArchivePlayer	(player)
				{}

			// virtual function ordering
			// vtable NoteOnCallback
			virtual detail::Channel *NoteOn(
				detail::SeqPlayer *seqPlayer, int bankNo,
				const detail::NoteOnInfo &noteOnInfo);

		// members
		private:
			/* base NoteOnCallback */							// size 0x04, offset 0x00
			const SoundArchivePlayer	&mSoundArchivePlayer;	// size 0x04, offset 0x04
		}; // size 0x08

		class WsdCallback : public detail::WsdPlayer::WsdCallback
		{
		// methods
		public:
			// cdtors
			WsdCallback(const SoundArchivePlayer &player):
				mSoundArchivePlayer	(player)
				{}

			// virtual function ordering
			// vtable detail::WsdPlayer::WsdCallback
			virtual bool GetWaveSoundData(detail::WaveSoundInfo *info,
			                              detail::WaveSoundNoteInfo *noteInfo,
			                              detail::WaveInfo *waveData,
			                              const void *waveSoundData, int index,
			                              int noteIndex,
			                              register_t userData) const;

		// members
		private:
			/* base detail::WsdPlayer::WsdCallback */		// size 0x04, offset 0x00
			const SoundArchivePlayer &mSoundArchivePlayer;	// size 0x04, offset 0x04
		}; // size 0x08

	// more methods
	public:
		bool Setup(const SoundArchive *arc, void *buffer, u32 size,
		           void *strmBuffer, u32 strmBufferSize);
		bool IsAvailable() const;
		void Update();
		void Shutdown();

		bool SetupMram(const SoundArchive *arc, void *buffer, u32 size);
		bool SetupStrmBuffer(const SoundArchive *arc, void *buffer, u32 size);

		u32 GetSoundPlayerCount() const { return mSoundPlayerCount; }
		u32 GetRequiredMemSize(const SoundArchive *arc);
		u32 GetRequiredStrmBufferSize(const SoundArchive *arc);

		bool SetupSoundPlayer(const SoundArchive *arc, void **buffer,
		                      void *endp);
		bool CreateGroupAddressTable(const SoundArchive *arc, void **buffer,
		                             void *endp);
		bool CreateFileAddressTable(const SoundArchive *arc, void **buffer,
		                            void *endp);

		bool SetupSeqSound(const SoundArchive *arc, int numSounds, void **buffer, void *endp);
		bool SetupWaveSound(const SoundArchive *arc, int numSounds, void **buffer, void *endp);
		bool SetupStrmSound(const SoundArchive *arc, int numSounds, void **buffer, void *endp);
		bool SetupSeqTrack(const SoundArchive *arc, int numTracks, void **buffer, void *endp);

		void UpdateCommonSoundParam(detail::BasicSound *sound,
		                            const SoundArchive::SoundInfo *commonInfo);

		detail::PlayerHeap *CreatePlayerHeap(void **buffer, void *endp,
		                                     u32 heapSize);
		SoundPlayer &GetSoundPlayer(u32 playerId);
		const SoundArchive &GetSoundArchive() const;
		const void *detail_GetFileAddress(u32 fileId) const;
		const void *detail_GetFileWaveDataAddress(u32 fileId) const;
		const void *GetGroupAddress(u32 groupId) const;
		const void *GetGroupWaveDataAddress(u32 groupId) const;
		const void *GetFileAddress(u32 fileId) const;
		const void *GetFileWaveDataAddress(u32 fileId) const;

		SoundStartable::StartResult detail_SetupSoundImpl(
			nw4r::snd::SoundHandle *handle, u32 soundId,
			detail::BasicSound::AmbientInfo *ambientArgInfo, SoundActor *actor,
			bool holdFlag, const SoundStartable::StartInfo *startInfo);

		SoundStartable::StartResult PrepareSeqImpl(
			detail::SeqSound *sound, const SoundArchive::SoundInfo *commonInfo,
			const SoundArchive::SeqSoundInfo *info,
			SoundStartable::StartInfo::StartOffsetType startOffsetType,
			int startOffset, const void *externalSeqDataAddress,
			const char *externalSeqStartLabel);

		SoundStartable::StartResult PrepareStrmImpl(
			detail::StrmSound *sound, const SoundArchive::SoundInfo *commonInfo,
			const SoundArchive::StrmSoundInfo *info,
			SoundStartable::StartInfo::StartOffsetType startOffsetType,
			int startOffset);

		SoundStartable::StartResult PrepareWaveSoundImpl(
			detail::WaveSound *sound, const SoundArchive::SoundInfo *commonInfo,
			const SoundArchive::WaveSoundInfo *info,
			SoundStartable::StartInfo::StartOffsetType startOffsetType,
			int startOffset);

		template <typename Sound>
		Sound *AllocSound(
			detail::SoundInstanceManager<Sound> *manager, u32 soundId,
			int priority, int ambientPriority,
			detail::BasicSound::AmbientInfo *ambientArgInfo);

	// members
	private:
		/* base DisposeCallback */													// size 0x0c, offset 0x00
		/* base SoundStartable */													// size 0x04, offset 0x0c
		const SoundArchive								*mSoundArchive;				// size 0x04, offset 0x10
		GroupAddress::Table								*mGroupTable;				// size 0x04, offset 0x14
		FileAddress::Table								*mFileTable;				// size 0x04, offset 0x18
		SoundArchivePlayer_FileManager					*mFileManager;				// size 0x04, offset 0x1c
		SeqNoteOnCallback								mSeqCallback;				// size 0x08, offset 0x20
		WsdCallback										mWsdCallback;				// size 0x08, offset 0x28
		detail::SeqTrackAllocator						*mSeqTrackAllocator;		// size 0x04, offset 0x30
		detail::SeqPlayer::SeqUserprocCallback			*mSeqUserprocCallback;		// size 0x04, offset 0x34
		void											*mSeqUserprocCallbackArg;	// size 0x04, offset 0x38
		u32												mSoundPlayerCount;			// size 0x04, offset 0x3c
		SoundPlayer										*mSoundPlayers;				// size 0x04, offset 0x40
		detail::SoundInstanceManager<detail::SeqSound>	mSeqSoundInstanceManager;	// size 0x28, offset 0x44
		detail::SoundInstanceManager<detail::StrmSound>	mStrmSoundInstanceManager;	// size 0x28, offset 0x6c
		detail::SoundInstanceManager<detail::WaveSound>	mWaveSoundInstanceManager;	// size 0x28, offset 0x94
		detail::MmlSeqTrackAllocator					mMmlSeqTrackAllocator;		// size 0x0c, offset 0xbc
		detail::StrmBufferPool							mStrmBufferPool;			// size 0x18, offset 0xc8
		detail::MmlParser								mMmlParser;					// size 0x04, offset 0xe0
		void											*mSetupBufferAddress;		// size 0x04, offset 0xe4
		u32												mSetupBufferSize;			// size 0x04, offset 0xe8
	}; // size 0xec

}} // namespace nw4r::snd

#endif // NW4R_SND_SOUND_ARCHIVE_PLAYER_H
