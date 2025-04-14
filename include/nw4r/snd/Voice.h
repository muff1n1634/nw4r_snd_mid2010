#ifndef NW4R_SND_VOICE_H
#define NW4R_SND_VOICE_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "AxVoice.h"
#include "global.h"
#include "DisposeCallbackManager.h"

#include "../ut/LinkList.h"

/*******************************************************************************
 * classes and functions
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail { struct WaveInfo; }}}

namespace nw4r { namespace snd { namespace detail
{
	class Voice : public DisposeCallback
	{
	// enums
	public:
		enum VoiceCallbackStatus
		{
			CALLBACK_STATUS_FINISH_WAVE,
			CALLBACK_STATUS_CANCEL,
			CALLBACK_STATUS_DROP_VOICE,
			CALLBACK_STATUS_DROP_DSP,
		};

		enum VoiceSyncFlag
		{
			SYNC_AX_SRC_INITIAL	= 1 << 0,
			SYNC_AX_VOICE		= 1 << 1,
			SYNC_AX_SRC			= 1 << 2,
			SYNC_AX_VE			= 1 << 3,
			SYNC_AX_MIX			= 1 << 4,
			SYNC_AX_LPF			= 1 << 5,
			SYNC_AX_REMOTE		= 1 << 7,
			SYNC_AX_BIQUAD		= 1 << 8,
		};

	// typedefs
	public:
		typedef ut::LinkList<Voice, 236> LinkList;

		typedef void Callback(Voice *dropVoice, VoiceCallbackStatus status,
		                      void *callbackData);

	// methods
	public:
		// cdtors
		Voice();
		virtual ~Voice();

		// virtual function ordering
		// vtable DisposeCallback
		virtual void InvalidateData(const void *, const void *) {}
		virtual void InvalidateWaveData(const void *start,  const void *end);

		// methods
		int GetPriority() const { return mPriority; }
		int GetPhysicalVoiceCount() const
		{
			return mChannelCount * mVoiceOutCount;
		}

		bool IsActive() const { return mAxVoice[0][0] != nullptr; }
		bool IsPlayFinished() const
		{
			return IsActive() && mAxVoice[0][0]->IsPlayFinished();
		}

		void SetVoiceOutParamPitchDisableFlag(bool flag)
		{
			mVoiceOutParamPitchDisableFlag = flag;
		}

		void InitParam(int channelCount, int voiceOutCount, Callback *callback,
		               void *callbackData);
		void StopFinished();

		void Calc();
		void Update();

		bool Acquire(int channelCount, int voiceOutCount, int priority,
		             Callback *callback, void *callbackData);
		void Free();

		void Setup(const WaveInfo &waveParam, u32 startOffset);

		void Start();
		void Stop();
		void Pause(bool flag);

		SampleFormat GetFormat() const;

		void SetVolume(f32 volume);
		void SetVeVolume(f32 target, f32 init);
		void SetPitch(f32 pitch);

		void SetPanMode(PanMode mode);
		void SetPanCurve(PanCurve curve);
		void SetPan(f32 pan);
		void SetSurroundPan(f32 pan);

		void SetLpfFreq(f32 freq);
		void SetBiquadFilter(int type, f32 value);
		void SetRemoteFilter(int filter);
		void SetOutputLine(int flag);

		void SetMainOutVolume(f32 volume);
		void SetMainSend(f32 send);
		void SetFxSend(AuxBus bus, f32 send);

		void SetVoiceOutParam(int voiceOutIndex,
		                      const VoiceOutParam &voiceOutParam);

		void SetPriority(int priority);
		void UpdateVoicesPriority();

		void SetAdpcmLoop(int channelIndex, const AdpcmLoopParam *param);
		u32 GetCurrentPlayingSample() const;
		void SetLoopStart(int channelIndex, const void *baseAddress, u32 samples);
		void SetLoopEnd(int channelIndex, const void *baseAddress, u32 samples);
		void SetLoopFlag(bool loopFlag);
		void StopAtPoint(int channelIndex, const void *baseAddress, u32 samples);
		void SetVoiceType(AxVoice::VoiceType type);

		void CalcAxSrc(bool initialUpdate);
		void CalcAxVe();
		bool CalcAxMix();
		void CalcAxLpf();
		void CalcAxBiquadFilter();
		void CalcAxRemoteFilter();

	private:
		void SyncAxVoice();
		void ResetDelta();

		static void AxVoiceCallbackFunc(AxVoice *dropVoice,
		                                AxVoice::AxVoiceCallbackStatus status,
		                                void *callbackData);

		void TransformDpl2Pan(f32 *outPan, f32 *outSurroundPan, f32 inPan,
		                      f32 inSurroundPan);
		void CalcMixParam(int channelIndex, int voiceOutIndex,
		                  AxVoice::MixParam *mix,
		                  AxVoice::RemoteMixParam *rmtmix);

		void RunAllAxVoice();
		void StopAllAxVoice();

	// early constants
	private:
		static const int PRIORITY_MAX = 255;
		static const int VOICES_MIN = 1;
		static const int VOICES_MAX = 4;

	// members
	private:
		/* base DisposeCallback */								// size 0x00c, offset 0x000
		AxVoice				*mAxVoice[CHANNEL_MAX][VOICES_MAX];	// size 0x020, offset 0x00c
		VoiceOutParam		mVoiceOutParam[VOICES_MAX];			// size 0x060, offset 0x02c
		int					mChannelCount;						// size 0x004, offset 0x08c
		int					mVoiceOutCount;						// size 0x004, offset 0x090
		Callback			*mCallback;							// size 0x004, offset 0x094
		void				*mCallbackData;						// size 0x004, offset 0x098
		bool				mActiveFlag;						// size 0x001, offset 0x09c
		bool				mStartFlag;							// size 0x001, offset 0x09d
		bool				mStartedFlag;						// size 0x001, offset 0x09e
		bool				mPauseFlag;							// size 0x001, offset 0x09f
		bool				mPausingFlag;						// size 0x001, offset 0x0a0
		bool				mVoiceOutParamPitchDisableFlag;		// size 0x001, offset 0x0a1
		u16					mSyncFlag;							// size 0x002, offset 0x0a2
		u8					mRemoteFilter;						// size 0x001, offset 0x0a4
		u8					mBiquadType;						// size 0x001, offset 0x0a5
		/* 2 bytes padding */
		int					mPriority;							// size 0x004, offset 0x0a8
		f32					mPan;								// size 0x004, offset 0x0ac
		f32					mSurroundPan;						// size 0x004, offset 0x0b0
		f32					mLpfFreq;							// size 0x004, offset 0x0b4
		f32					mBiquadValue;						// size 0x004, offset 0x0b8
		int					mOutputLineFlag;					// size 0x004, offset 0x0bc
		f32					mMainOutVolume;						// size 0x004, offset 0x0c0
		f32					mMainSend;							// size 0x004, offset 0x0c4
		f32					mFxSend[AUX_BUS_NUM];				// size 0x00c, offset 0x0c8
		f32					mPitch;								// size 0x004, offset 0x104
		f32					mVolume;							// size 0x004, offset 0x108
		f32					mVeInitVolume;						// size 0x004, offset 0x10c
		f32					mVeTargetVolume;					// size 0x004, offset 0x110
		PanMode				mPanMode;							// size 0x004, offset 0x114
		PanCurve			mPanCurve;							// size 0x004, offset 0x118
	public:
		ut::LinkListNode	mLinkNode;							// size 0x008, offset 0x11c

	// friends
	private:
		friend class VoiceManager;
	}; // size 0x124
}}} // namespace nw4r::snd

#endif // NW4R_SND_VOICE_H
