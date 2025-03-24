#ifndef NW4R_SND_SEQ_SOUND_H
#define NW4R_SND_SEQ_SOUND_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "BasicSound.h"
#include "debug.h"
#include "SeqPlayer.h"
#include "Task.h"

#include "../ut/LinkList.h"

/*******************************************************************************
 * classes and functions
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail { class NoteOnCallback; }}}
namespace nw4r { namespace snd { namespace detail { class SeqSoundHandle; }}}
namespace nw4r { namespace snd { namespace detail { class SeqTrackAllocator; }}}
namespace nw4r { namespace snd { namespace detail
{
	template <class> class SoundInstanceManager;
}}} // namespace nw4r::snd::detail

namespace nw4r { namespace ut { namespace detail { class RuntimeTypeInfo; }}}
namespace nw4r { namespace ut { class FileStream; }}

namespace nw4r { namespace snd { namespace detail
{
	class SeqSound : public BasicSound
	{
	// typedefs
	public:
		// need to redeclare with this class instead of BasicSound
		typedef ut::LinkList<SeqSound, 0xe0> PriorityLinkList;

	// nested types
	public:
		class SeqLoadTask : public Task
		{
		// typedefs
		public:
			typedef void Callback(bool result, const void *seqBase,
			                      void *userData);

		// methods
		public:
			// cdtors
			SeqLoadTask();
			virtual ~SeqLoadTask() {}

			// virtual function ordering
			// vtable Task
			virtual void Execute();
			virtual void Cancel();
			virtual void OnCancel();

		// members
		public:
			/* base Task */					// size 0x10, offset 0x00
			ut::FileStream	*mFileStream;	// size 0x04, offset 0x10
			void			*mBuffer;		// size 0x04, offset 0x14
			int				mBufferSize;	// size 0x04, offset 0x18
			Callback		*mCallback;		// size 0x04, offset 0x1c
			void			*mCallbackData;	// size 0x04, offset 0x20
		}; // size 0x24

	// methods
	public:
		// cdtors
		SeqSound(SoundInstanceManager<SeqSound> *manager, int priority,
		         int ambientPriority);
		/* virtual ~SeqSound(); */ // virtual function ordering

		// virtual function ordering
		// vtable BasicSound
		virtual const ut::detail::RuntimeTypeInfo *GetRuntimeTypeInfo() const
		{
			return &typeInfo;
		}
		virtual ~SeqSound() {}
		virtual void Shutdown();
		virtual bool IsPrepared() const { return mPreparedFlag; }
		virtual bool IsAttachedTempSpecialHandle();
		virtual void DetachTempSpecialHandle();
		virtual void InitParam();
		virtual BasicPlayer &GetBasicPlayer() { return mSeqPlayer; }
		virtual const BasicPlayer &GetBasicPlayer() const { return mSeqPlayer; }
		virtual void OnUpdatePlayerPriority();
		// virtual void UpdateMoveValue();
		virtual void UpdateParam();

		// methods
		SeqPlayer::SetupResult Setup(SeqTrackAllocator *trackAllocator,
		                             u32 allocTracks,
		                             NoteOnCallback *callback);

		void Prepare(const void *seqBase, s32 seqOffset,
		             SeqPlayer::OffsetType startOffsetType, int startOffset);

		void Prepare(ut::FileStream *fileStream, s32 seqOffset,
		             SeqPlayer::OffsetType startOffsetType, int startOffset);

		static void NotifyLoadAsyncEndSeqData(bool result, const void *seqBase,
		                               void *userData);

		void Skip(SeqPlayer::OffsetType offsetType, int offset);

		bool LoadData(SeqLoadTask::Callback *callback,
		              void *callbackArg);

		void SetChannelPriority(int priority);
		void SetReleasePriorityFix(bool fix);
		void SetSeqUserprocCallback(SeqPlayer::SeqUserprocCallback *callback,
		                            void *arg);

		void *GetFileStreamBuffer() { return mFileStreamBuffer; }
		u32 GetFileStreamBufferSize() const { return sizeof mFileStreamBuffer; }

		static DebugSoundType GetSoundType()
		{
			return DEBUG_SOUND_TYPE_SEQSOUND;
		}

	// static members
	public:
		static ut::detail::RuntimeTypeInfo const typeInfo;

	// members
	private:
		/* base BasicSound */									// size 0x100, offset 0x000
		SeqPlayer						mSeqPlayer;				// size 0x154, offset 0x100
		SeqSoundHandle					*mTempSpecialHandle;	// size 0x004, offset 0x254
		SoundInstanceManager<SeqSound>	*mManager;				// size 0x004, offset 0x258
		s32								mSeqOffset;				// size 0x004, offset 0x25c
		SeqPlayer::OffsetType			mStartOffsetType;		// size 0x004, offset 0x260
		int								mStartOffset;			// size 0x004, offset 0x264
		bool							mLoadingFlag;			// size 0x001, offset 0x268
		bool volatile					mPreparedFlag;			// size 0x001, offset 0x269
		/* 2 bytes padding */
		ut::FileStream					*mFileStream;			// size 0x004, offset 0x29c
		int								mFileStreamBuffer[128];	// size 0x200, offset 0x2a0
		SeqLoadTask						mSeqLoadTask;			// size 0x024, offset 0x4a0

	// friends
	private:
		friend class SeqSoundHandle;
	}; // size 0x4c4
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_SEQ_SOUND_H
