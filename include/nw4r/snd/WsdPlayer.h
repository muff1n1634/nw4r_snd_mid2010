#ifndef NW4R_SND_WSD_PLAYER_H
#define NW4R_SND_WSD_PLAYER_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "BasicPlayer.h"
#include "Channel.h"
#include "DisposeCallbackManager.h"
#include "Lfo.h"
#include "SoundThread.h"
#include "WaveFile.h"

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	class WsdPlayer : public BasicPlayer,
					  public DisposeCallback,
					  public SoundThread::PlayerCallback
	{
	// enums
	public:
		enum StartOffsetType
		{
			START_OFFSET_TYPE_SAMPLE,
			START_OFFSET_TYPE_MILLISEC,
		};

	// nested types
	public:
		class WsdCallback
		{
		// methods
		public:
			// cdtors
			virtual ~WsdCallback() {}

			// virtual function ordering
			// vtable WsdCallback
			virtual bool GetWaveSoundData(WaveSoundInfo *info,
			                              WaveSoundNoteInfo *noteInfo,
			                              WaveInfo *waveData,
			                              void const *waveSoundData, int index,
			                              int noteIndex,
			                              register_t userData) const = 0;

			// members
		private:
			/* vtable */	// size 0x04, offset 0x00
		}; // size 0x04

	// methods
	public:
		// cdtors
		WsdPlayer();

		// virtual function ordering
		// vtable BasicPlayer
		virtual bool Start();
		virtual void Stop();
		virtual void Pause(bool flag);
		virtual bool IsActive() const { return mActiveFlag != false; }
		virtual bool IsStarted() const { return mStartedFlag != false; }
		virtual bool IsPause() const { return mPauseFlag != false; }

		// vtable DisposeCallback
		virtual void InvalidateData(void const *start, void const *end);
		virtual void InvalidateWaveData(void const *, void const *) {}

		// vtable PlayerCallback
		virtual void OnUpdateFrameSoundThread() { Update(); }
		virtual void OnShutdownSoundThread() { Stop(); }

		// gethods
		f32 GetPanRange() const { return mPanRange; }
		int GetVoiceOutCount() const { return mVoiceOutCount; }
		int GetChannelPriority() const { return mPriority; }
		void const *GetWsdDataAddress() const { return mWsdData; }

		// methods
		bool IsChannelActive() const
		{
			return mChannel != nullptr && mChannel->IsActive(); }

		void InitParam(int voiceOutCount, WsdCallback const *callback,
	                   register_t callbackData);

		bool Prepare(void const *waveSoundBase, int index,
	                 StartOffsetType startOffsetType, int startOffset,
	                 int voiceOutCount, WsdCallback const *callback,
	                 register_t callbackData);

		void Update();
		void FinishPlayer();
		bool StartChannel(WsdCallback const *callback, register_t callbackData);
		void CloseChannel();
		void UpdateChannel();
		u32 GetPlaySamplePosition() const;

		void SetChannelPriority(int priority);
		void SetReleasePriorityFix(bool flag);

	// static methods
	private:
		static void ChannelCallbackFunc(Channel *dropChannel,
		                                Channel::ChannelCallbackStatus status,
		                                register_t userData);

	// static members
	private:
		static int const DEFAULT_PRIORITY = 64;

	// members
	private:
		/* base BasicPlayer */							// size 0x0a4, offset 0x000
		/* base DisposeCallback */						// size 0x00c, offset 0x0a4
		/* base PlayerCallback */						// size 0x00c, offset 0x0b0
		bool				mActiveFlag;				// size 0x001, offset 0x0bc
		bool				mStartedFlag;				// size 0x001, offset 0x0bd
		bool				mPauseFlag;					// size 0x001, offset 0x0be
		bool				mWavePlayFlag;				// size 0x001, offset 0x0bf
		bool				mReleasePriorityFixFlag;	// size 0x001, offset 0x0c0
		/* 3 bytes padding */
		f32					mPanRange;					// size 0x004, offset 0x0c4
		int					mVoiceOutCount;				// size 0x004, offset 0x0c8
		u8					mPriority;					// size 0x001, offset 0x0cc
		/* 3 bytes padding */
		WsdCallback const	*mCallback;					// size 0x004, offset 0x0d0
		register_t			mCallbackData;				// size 0x004, offset 0x0d4
		void const			*mWsdData;					// size 0x004, offset 0x0d8
		int					mWsdIndex;					// size 0x004, offset 0x0dc
		StartOffsetType		mStartOffsetType;			// size 0x004, offset 0x0e0
		int					mStartOffset;				// size 0x004, offset 0x0e4
		LfoParam			mLfoParam;					// size 0x010, offset 0x0e8
		WaveSoundInfo		mWaveSoundInfo;				// size 0x00c, offset 0x0f8
		Channel				*mChannel;					// size 0x004, offset 0x104
	}; // size 0x108
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_WSD_PLAYER_H
