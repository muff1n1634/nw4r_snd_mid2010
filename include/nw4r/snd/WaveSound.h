#ifndef NW4R_SND_WAVE_SOUND_H
#define NW4R_SND_WAVE_SOUND_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "BasicSound.h"
#include "debug.h"
#include "WsdPlayer.h"

#include "../ut/LinkList.h"

/*******************************************************************************
 * classes and functions
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail
{
	template <class> class SoundInstanceManager;
}}} // namespace nw4r::snd::detail
namespace nw4r { namespace snd { class WaveSoundHandle; }}

namespace nw4r { namespace ut { namespace detail { class RuntimeTypeInfo; }}}

namespace nw4r { namespace snd { namespace detail
{
	class WaveSound : public BasicSound
	{
	// typedefs
	public:
		// need to redeclare with this class instead of BasicSound
		typedef ut::LinkList<WaveSound, 0xe0> PriorityLinkList;

	// methods
	public:
		// cdtors
		WaveSound(SoundInstanceManager<WaveSound> *manager, int priority,
		          int ambientPriority);

		// virtual function ordering
		// vtable BasicSound
		virtual const ut::detail::RuntimeTypeInfo *GetRuntimeTypeInfo() const
			{ return &typeInfo; }
		virtual ~WaveSound() {}
		virtual void Shutdown();
		virtual bool IsPrepared() const { return mPreparedFlag; }
		virtual bool IsAttachedTempSpecialHandle();
		virtual void DetachTempSpecialHandle();
		virtual void InitParam();
		virtual BasicPlayer &GetBasicPlayer() { return mWsdPlayer; }
		virtual const BasicPlayer &GetBasicPlayer() const { return mWsdPlayer; }
		virtual void OnUpdatePlayerPriority();
		virtual void UpdateMoveValue();
		virtual void UpdateParam();

		// gethods
		const WsdPlayer &GetPlayer() const { return mWsdPlayer; }

		// methods
		bool Prepare(const void *waveSoundBase, s32 waveSoundOffset,
		             WsdPlayer::StartOffsetType startOffsetType, s32 offset,
		             const WsdPlayer::WsdCallback *callback,
		             register_t callbackData);

		void SetChannelPriority(int priority);
		void SetReleasePriorityFix(bool flag);

		static DebugSoundType GetSoundType() { return DEBUG_SOUND_TYPE_WAVESOUND; }

	// static members
	public:
		static ut::detail::RuntimeTypeInfo const typeInfo;

	// members
	private:
		/* base BasicSound */									// size 0x100, offset 0x000
		WsdPlayer						mWsdPlayer;				// size 0x138, offset 0x100
		WaveSoundHandle					*mTempSpecialHandle;	// size 0x004, offset 0x238
		SoundInstanceManager<WaveSound>	*mManager;				// size 0x004, offset 0x23c
		bool							mPreparedFlag;			// size 0x001, offset 0x240
		/* 3 bytes padding */

	// friends
	private:
		friend class snd::WaveSoundHandle;
	}; // size 0x244
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_WAVE_SOUND_H
