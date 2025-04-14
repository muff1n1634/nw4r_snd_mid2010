#ifndef NW4R_SND_STRM_SOUND_H
#define NW4R_SND_STRM_SOUND_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "BasicSound.h"
#include "debug.h"
#include "MoveValue.h"
#include "StrmPlayer.h"

#include "../ut/LinkList.h"

/*******************************************************************************
 * classes and functions
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail
{
	template <class> class SoundInstanceManager;
}}} // namespace nw4r::snd::detail
namespace nw4r { namespace snd { class StrmSoundHandle; }}
namespace nw4r { namespace snd { namespace detail { class StrmBufferPool; }}}

namespace nw4r { namespace ut { namespace detail { class RuntimeTypeInfo; }}}
namespace nw4r { namespace ut { class FileStream; }}

namespace nw4r { namespace snd { namespace detail
{
	class StrmSound : public BasicSound
	{
	// typedefs
	public:
		// need to redeclare with this class instead of BasicSound
		typedef ut::LinkList<StrmSound, 0xe0> PriorityLinkList;

	// methods
	public:
		// cdtors
		StrmSound(SoundInstanceManager<StrmSound> *manager, int priority,
		          int ambientPriority);

		// virtual function ordering
		// vtable BasicSound
		virtual const ut::detail::RuntimeTypeInfo *GetRuntimeTypeInfo() const
		{
			return &typeInfo;
		}
		virtual ~StrmSound() {}
		virtual void Shutdown();
		virtual bool IsPrepared() const { return mStrmPlayer.IsPrepared(); }
		virtual bool IsAttachedTempSpecialHandle();
		virtual void DetachTempSpecialHandle();
		virtual void InitParam();
		virtual BasicPlayer &GetBasicPlayer() { return mStrmPlayer; }
		virtual const BasicPlayer &GetBasicPlayer() const { return mStrmPlayer; }
		virtual void OnUpdatePlayerPriority();
		virtual void UpdateMoveValue();
		virtual void UpdateParam();

		// methods
		StrmPlayer::SetupResult Setup(StrmBufferPool *bufferPool,
		                              int allocChannelCount,
		                              byte2_t allocTrackFlag);
		bool Prepare(StrmPlayer::StartOffsetType startOffsetType, s32 offset,
		             ut::FileStream *fileStream);

		void *GetFileStreamBuffer() { return mFileStreamBuffer; }
		s32 GetFileStreamBufferSize() { return sizeof mFileStreamBuffer; }

		static DebugSoundType GetSoundType() { return DEBUG_SOUND_TYPE_STRMSOUND; }

	// members
	private:
		/* base BasicSound */									// size 0x100, offset 0x000
		StrmPlayer						mStrmPlayer;			// size 0xd08, offset 0x100
		StrmSoundHandle					*mTempSpecialHandle;	// size 0x004, offset 0xe08
		SoundInstanceManager<StrmSound>	*mManager;				// size 0x004, offset 0xe0c
		MoveValue<f32, int>				mTrackVolume[8];		// size 0x080, offset 0xe10
		ut::FileStream					*mFileStream;			// size 0x004, offset 0xe90
		int								mFileStreamBuffer[128];	// size 0x200, offset 0xe94

	// static members
	public:
		static ut::detail::RuntimeTypeInfo typeInfo;

	// friends
	private:
		friend class snd::StrmSoundHandle;
	}; // size 0x1094
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_STRM_SOUND_H
