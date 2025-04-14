#include "nw4r/snd/Voice.h"

/* Original source:
 * kiwi515/ogws
 * src/nw4r/snd/snd_Voice.cpp
 */

/*******************************************************************************
 * headers
 */

#include <decomp.h>
#include <macros.h>
#include <types.h>

#include "nw4r/snd/AxManager.h"
#include "nw4r/snd/AxVoiceManager.h"
#include "nw4r/snd/AxVoice.h"
#include "nw4r/snd/global.h"
#include "nw4r/snd/Util.h"
#include "nw4r/snd/VoiceManager.h"
#include "nw4r/snd/WaveFile.h"

#include "nw4r/ut/inlines.h"
#include "nw4r/ut/lock.h"

#if 0
#include <revolution/AX/AXAlloc.h> // AX_MAX_PRIORITY
#else
#include <context_rvl.h>
#endif

#include "nw4r/NW4RAssert.hpp"

/*******************************************************************************
 * functions
 */

namespace nw4r { namespace snd { namespace detail {

Voice::Voice() :
	mCallback		(nullptr),
	mActiveFlag		(false),
	mStartFlag		(false),
	mStartedFlag	(false),
	mPauseFlag		(false),
	mSyncFlag		(0)
{
	for (int channelIndex = 0; channelIndex < CHANNEL_MAX; channelIndex++)
	{
		for (int voiceOutIndex = 0; voiceOutIndex < VOICES_MAX; voiceOutIndex++)
			mAxVoice[channelIndex][voiceOutIndex] = nullptr;
	}
}

Voice::~Voice()
{
	for (int channelIndex = 0; channelIndex < CHANNEL_MAX; channelIndex++)
	{
		for (int voiceOutIndex = 0; voiceOutIndex < VOICES_MAX; voiceOutIndex++)
		{
			AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
			if (!axVoice)
				continue;

			AxVoiceManager::GetInstance().FreeAxVoice(axVoice);
		}
	}
}

void Voice::InitParam(int channelCount, int voiceOutCount, Callback *callback,
                      void *callbackData)
{
	// specifically not the source variants
	NW4RAssertHeaderClampedLRValue_Line(128, channelCount, CHANNEL_MIN,
	                                    CHANNEL_MAX);
	NW4RAssertHeaderClampedLRValue_Line(129, voiceOutCount, VOICES_MIN,
	                                    VOICES_MAX);

	mChannelCount					= channelCount;
	mVoiceOutCount					= voiceOutCount;
	mCallback						= callback;
	mCallbackData					= callbackData;
	mSyncFlag						= 0;
	mPauseFlag						= false;
	mPausingFlag					= false;
	mStartedFlag					= false;
	mVoiceOutParamPitchDisableFlag	= false;
	mVolume							= 1.0f;
	mVeInitVolume					= 0.0f;
	mVeTargetVolume					= 1.0f;
	mLpfFreq						= 1.0f;
	mBiquadType						= 0;
	mBiquadValue					= 0.0f;
	mPan							= 0.0f;
	mSurroundPan					= 0.0f;
	mOutputLineFlag					= 1;
	mMainOutVolume					= 1.0f;
	mMainSend						= 1.0f;

	for (int i = 0; i < AUX_BUS_NUM; i++)
		mFxSend[i] = 0.0f;

	mPitch							= 1.0f;
	mRemoteFilter					= 0;
	mPanMode						= PAN_MODE_DUAL;
	mPanCurve						= PAN_CURVE_SQRT;
}

void Voice::StopFinished()
{
	if (mActiveFlag && mStartedFlag && IsPlayFinished())
	{
		if (mCallback)
			(*mCallback)(this, CALLBACK_STATUS_FINISH_WAVE, mCallbackData);

		mStartedFlag	= false;
		mStartFlag		= false;
	}
}

void Voice::Calc()
{
	if (!mStartFlag)
		return;

	if (mSyncFlag & SYNC_AX_SRC)
	{
		CalcAxSrc(false);
		mSyncFlag &= ~SYNC_AX_SRC;
	}

	if (mSyncFlag & SYNC_AX_VE)
	{
		CalcAxVe();
		mSyncFlag &= ~SYNC_AX_VE;
	}

	if (mSyncFlag & SYNC_AX_MIX)
	{
		if (!CalcAxMix())
			mSyncFlag &= ~SYNC_AX_MIX;
	}

	if (mSyncFlag & SYNC_AX_LPF)
	{
		CalcAxLpf();
		mSyncFlag &= ~SYNC_AX_LPF;
	}

	if (mSyncFlag & SYNC_AX_BIQUAD)
	{
		CalcAxBiquadFilter();
		mSyncFlag &= ~SYNC_AX_BIQUAD;
	}

	if (mSyncFlag & SYNC_AX_REMOTE)
	{
		CalcAxRemoteFilter();
		mSyncFlag &= ~SYNC_AX_REMOTE;
	}
}

void Voice::Update()
{
	ut::AutoInterruptLock lock;

	if (!mActiveFlag)
		return;

	enum
	{
		NONE,
		RUN,
		STOP,
	} runFlag = NONE;

	if ((mSyncFlag & SYNC_AX_SRC_INITIAL) && mStartFlag && !mStartedFlag)
	{
		CalcAxSrc(true);

		runFlag = RUN;
		mStartedFlag = true;
		mSyncFlag &= ~SYNC_AX_SRC_INITIAL;
		mSyncFlag &= ~SYNC_AX_SRC;
	}

	if (mStartedFlag)
	{
		if ((mSyncFlag & SYNC_AX_VOICE) && mStartFlag)
		{
			if (mPauseFlag)
			{
				mPausingFlag = true;
				runFlag = STOP;
			}
			else
			{
				mPausingFlag = false;
				runFlag = RUN;
			}

			mSyncFlag &= ~SYNC_AX_VOICE;
		}

		SyncAxVoice();
	}

	switch (runFlag)
	{
	case RUN:
		RunAllAxVoice();
		break;

	case STOP:
		StopAllAxVoice();
		break;
	}
}

bool Voice::Acquire(int channelCount, int voiceOutCount, int priority,
                    Callback *callback, void *callbackData)
{
	NW4RAssertHeaderClampedLRValue_Line(336, channelCount, CHANNEL_MIN,
	                                    CHANNEL_MAX);
	channelCount = ut::Clamp(channelCount, CHANNEL_MIN, CHANNEL_MAX);

	NW4RAssertHeaderClampedLRValue_Line(339, voiceOutCount, VOICES_MIN,
	                                    VOICES_MAX);
	voiceOutCount = ut::Clamp(voiceOutCount, VOICES_MIN, VOICES_MAX);

	ut::AutoInterruptLock lock;

	u32 axPriority =
		priority == PRIORITY_MAX ? AX_PRIORITY_MAX : AX_PRIORITY_MAX / 2 + 1;

	NW4RAssert_Line(346, ! mActiveFlag);

	int requiredVoiceCount = channelCount * voiceOutCount;
	AxVoice *voiceTable[CHANNEL_MAX * VOICES_MAX];

	for (int i = 0; i < requiredVoiceCount; i++)
	{
		AxVoice *axVoice = AxVoiceManager::GetInstance().AcquireAxVoice(
			axPriority, &AxVoiceCallbackFunc, this);

		if (!axVoice)
		{
			int restAXVPBCount = requiredVoiceCount - i;

			Voice::LinkList const &voiceList =
				VoiceManager::GetInstance().GetVoiceList();

			NW4R_RANGE_FOR(itr, voiceList)
			{
				if (priority < itr->GetPriority())
					break;

				restAXVPBCount -= itr->GetPhysicalVoiceCount();
				if (restAXVPBCount <= 0)
					break;
			}

			if (restAXVPBCount > 0)
			{
				for (int j = 0; j < i; j++)
					AxVoiceManager::GetInstance().FreeAxVoice(voiceTable[j]);

				return false;
			}

			u32 allocPriority = axPriority == AX_PRIORITY_MAX
			                      ? AX_PRIORITY_MAX
			                      : AX_PRIORITY_MAX / 2 + 2;

			axVoice = AxVoiceManager::GetInstance().AcquireAxVoice(
				allocPriority, &AxVoiceCallbackFunc, this);
		}

		NW4RAssertPointerNonnull_Line(399, axVoice);

		if (!axVoice)
		{
			for (int j = 0; j < i; j++)
				AxVoiceManager::GetInstance().FreeAxVoice(voiceTable[j]);

			return false;
		}

		voiceTable[i] = axVoice;
	}

	int axVoiceIndex = 0;
	for (int channelIndex = 0; channelIndex < channelCount; channelIndex++)
	{
		for (int voiceOutIndex = 0; voiceOutIndex < voiceOutCount;
		     voiceOutIndex++)
		{
			voiceTable[axVoiceIndex]->SetPriority(axPriority);
			mAxVoice[channelIndex][voiceOutIndex] = voiceTable[axVoiceIndex];
			axVoiceIndex++;
		}
	}

	InitParam(channelCount, voiceOutCount, callback, callbackData);

	mActiveFlag = true;
	return true;
}

void Voice::Free()
{
	ut::AutoInterruptLock lock;

	if (!mActiveFlag)
		return;

	for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++)
	{
		for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount;
		     voiceOutIndex++)
		{
			AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
			if (!axVoice)
				continue;

			AxVoiceManager::GetInstance().FreeAxVoice(axVoice);
			mAxVoice[channelIndex][voiceOutIndex] = nullptr;
		}
	}

	mChannelCount = 0;
	VoiceManager::GetInstance().FreeVoice(this);
	mActiveFlag = false;
}

void Voice::Setup(WaveInfo const &waveParam, u32 startOffset)
{
	int sampleRate = waveParam.sampleRate;

	for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++)
	{
		if (!mAxVoice[channelIndex][0])
			continue;

		NW4RAssertPointerNonnull_Line(
			477, waveParam.channelParam[channelIndex].dataAddr);
		void *dataAddr = waveParam.channelParam[channelIndex].dataAddr;

		AdpcmParam adpcmParam;
		if (waveParam.sampleFormat == SAMPLE_FORMAT_DSP_ADPCM)
		{
			adpcmParam = waveParam.channelParam[channelIndex].adpcmParam;
			AxVoice::CalcOffsetAdpcmParam(&adpcmParam.pred_scale,
			                              &adpcmParam.yn1, &adpcmParam.yn2,
			                              startOffset, dataAddr, adpcmParam);
		}

		for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount;
		     voiceOutIndex++)
		{
			AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
			if (!axVoice)
				continue;

			axVoice->Setup(waveParam.channelParam[channelIndex].dataAddr,
			               waveParam.sampleFormat, sampleRate);
			axVoice->SetAddr(waveParam.loopFlag, dataAddr, startOffset,
			                 waveParam.loopStart, waveParam.loopEnd);

			if (waveParam.sampleFormat == SAMPLE_FORMAT_DSP_ADPCM)
			{
				axVoice->SetAdpcm(&adpcmParam);
				axVoice->SetAdpcmLoop(
					&waveParam.channelParam[channelIndex].adpcmLoopParam);
			}

			axVoice->SetSrcType(AxVoice::SRC_4TAP_AUTO, mPitch);
			axVoice->SetVoiceType(AxVoice::VOICE_TYPE_NORMAL);
		}
	}

	for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount; voiceOutIndex++)
	{
		mVoiceOutParam[voiceOutIndex].volume		= 1.0f;
		mVoiceOutParam[voiceOutIndex].pitch			= 1.0f;
		mVoiceOutParam[voiceOutIndex].pan			= 0.0f;
		mVoiceOutParam[voiceOutIndex].surroundPan	= 0.0f;
		mVoiceOutParam[voiceOutIndex].fxSend		= 0.0f;
		mVoiceOutParam[voiceOutIndex].lpf			= 0.0f;
	}

	mPauseFlag		= false;
	mPausingFlag	= false;
	mStartFlag		= false;
	mStartedFlag	= false;

	mSyncFlag |= SYNC_AX_MIX;
	mSyncFlag |= SYNC_AX_VE;
	mSyncFlag |= SYNC_AX_LPF;
}

void Voice::Start()
{
	mStartFlag	= true;
	mPauseFlag	= false;

	mSyncFlag |= SYNC_AX_SRC_INITIAL;
}

void Voice::Stop()
{
	if (mStartedFlag)
	{
		StopAllAxVoice();
		mStartedFlag = false;
	}

	mPausingFlag	= false;
	mPauseFlag		= false;
	mStartFlag		= false;
}

void Voice::Pause(bool flag)
{
	if (mPauseFlag != flag)
	{
		mPauseFlag = flag;

		mSyncFlag |= SYNC_AX_VOICE;
	}
}

SampleFormat Voice::GetFormat() const
{
	NW4RAssert_Line(583, IsActive());

	if (IsActive())
		return mAxVoice[0][0]->GetFormat();
	else
		return SAMPLE_FORMAT_PCM_S16;
}

void Voice::SetVolume(f32 volume)
{
	volume = ut::Clamp(volume, 0.0f, 1.0f);

	if (volume != mVolume)
	{
		mVolume = volume;

		mSyncFlag |= SYNC_AX_VE;
	}
}

void Voice::SetVeVolume(f32 target, f32 init)
{
	target = ut::Clamp(target, 0.0f, 1.0f);
	init = ut::Clamp(init, 0.0f, 1.0f);

	if (init < 0.0f)
	{
		// ERRATUM: unreachable (init was clamped)

		if (target == mVeTargetVolume)
			return;

		mVeTargetVolume = target;

		mSyncFlag |= SYNC_AX_VE;
		return;
	}

	if (init != mVeInitVolume || target != mVeTargetVolume)
	{
		mVeInitVolume = init;
		mVeTargetVolume = target;

		mSyncFlag |= SYNC_AX_VE;
	}
}

void Voice::SetPitch(f32 pitch)
{
	if (pitch != mPitch)
	{
		mPitch = pitch;

		mSyncFlag |= SYNC_AX_SRC;
	}
}

void Voice::SetPanMode(PanMode mode)
{
	if (mode != mPanMode)
	{
		mPanMode = mode;

		mSyncFlag |= SYNC_AX_MIX;
	}
}

void Voice::SetPanCurve(PanCurve curve)
{
	if (curve != mPanCurve)
	{
		mPanCurve = curve;

		mSyncFlag |= SYNC_AX_MIX;
	}
}

void Voice::SetPan(f32 pan)
{
	if (pan != mPan)
	{
		mPan = pan;

		mSyncFlag |= SYNC_AX_MIX;
	}
}

void Voice::SetSurroundPan(f32 pan)
{
	if (pan != mSurroundPan)
	{
		mSurroundPan = pan;

		mSyncFlag |= SYNC_AX_MIX;
	}
}

void Voice::SetLpfFreq(f32 freq)
{
	if (freq != mLpfFreq)
	{
		mLpfFreq = freq;

		mSyncFlag |= SYNC_AX_LPF;
	}
}

void Voice::SetBiquadFilter(int type, f32 value)
{
	// specifically not the source variant
	NW4RAssertHeaderClampedLRValue_Line(680, type, 0, 127);

	value = ut::Clamp(value, 0.0f, 1.0f);

	bool isUpdate = false;

	if (type != mBiquadType)
	{
		mBiquadType = type;
		isUpdate = true;
	}

	if (value != mBiquadValue)
	{
		mBiquadValue = value;
		isUpdate = true;
	}

	if (isUpdate)
		mSyncFlag |= SYNC_AX_BIQUAD;
}

void Voice::SetRemoteFilter(int filter)
{
	filter = ut::Clamp(filter, 0, REMOTE_FILTER_MAX);

	if (filter != mRemoteFilter)
	{
		mRemoteFilter = filter;

		mSyncFlag |= SYNC_AX_REMOTE;
	}
}

void Voice::SetOutputLine(int flag)
{
	if (flag != mOutputLineFlag)
	{
		mOutputLineFlag = flag;

		mSyncFlag |= SYNC_AX_MIX;
	}
}

void Voice::SetMainOutVolume(f32 volume)
{
	volume = ut::Clamp(volume, 0.0f, 1.0f);

	if (volume != mMainOutVolume)
	{
		mMainOutVolume = volume;

		mSyncFlag |= SYNC_AX_MIX;
	}
}

void Voice::SetMainSend(f32 send)
{
	send += 1.0f;
	send = ut::Clamp(send, 0.0f, 1.0f);

	if (send != mMainSend)
	{
		mMainSend = send;

		mSyncFlag |= SYNC_AX_MIX;
	}
}

void Voice::SetFxSend(AuxBus bus, f32 send)
{
	// specifically not the source variant
	NW4RAssertHeaderClampedLValue_Line(748, bus, AUX_A, AUX_BUS_NUM);

	send = ut::Clamp(send, 0.0f, 1.0f);

	if (send != mFxSend[bus])
	{
		mFxSend[bus] = send;

		mSyncFlag |= SYNC_AX_MIX;
	}
}

void Voice::SetVoiceOutParam(int voiceOutIndex,
                             VoiceOutParam const &voiceOutParam)
{
	// specifically not the source variant
	NW4RAssertHeaderClampedLRValue_Line(820, voiceOutIndex, 0, VOICES_MAX);

	mVoiceOutParam[voiceOutIndex] = voiceOutParam;

	mSyncFlag |= SYNC_AX_SRC | SYNC_AX_VE | SYNC_AX_MIX | SYNC_AX_LPF;
}

void Voice::SetPriority(int priority)
{
	// specifically not the source variant
	NW4RAssertHeaderClampedLRValue_Line(828, priority, 0, PRIORITY_MAX);

	mPriority = priority;
	VoiceManager::GetInstance().ChangeVoicePriority(this);

	if (mPriority != AX_PRIORITY_MIN)
		return;

	for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++)
	{
		for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount;
		     voiceOutIndex++)
		{
			AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
			if (!axVoice)
				continue;

			axVoice->SetPriority(AX_PRIORITY_MAX / 2);
		}
	}
}

void Voice::UpdateVoicesPriority()
{
	if (mPriority == AX_PRIORITY_MIN)
		return;

	for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++)
	{
		for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount;
		     voiceOutIndex++)
		{
			AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
			if (!axVoice)
				continue;

			axVoice->SetPriority(AX_PRIORITY_MAX / 2 + 1);
		}
	}
}

// Voice::GetAxVoice? seems reasonable to me
DECOMP_FORCE(NW4RAssert_String(channelIndex < CHANNEL_MAX));

void Voice::SetAdpcmLoop(int channelIndex, AdpcmLoopParam const *param)
{
	for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount; voiceOutIndex++)
	{
		AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
		if (!axVoice)
			continue;

		axVoice->SetAdpcmLoop(param);
	}
}

u32 Voice::GetCurrentPlayingSample() const
{
	if (IsActive())
		return mAxVoice[0][0]->GetCurrentPlayingSample();
	else
		return 0;
}

void Voice::SetLoopStart(int channelIndex, void const *baseAddress, u32 samples)
{
	for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount; voiceOutIndex++)
	{
		AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
		if (!axVoice)
			continue;

		axVoice->SetLoopStart(baseAddress, samples);
	}
}

void Voice::SetLoopEnd(int channelIndex, void const *baseAddress, u32 samples)
{
	for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount; voiceOutIndex++)
	{
		AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
		if (!axVoice)
			continue;

		axVoice->SetLoopEnd(baseAddress, samples);
	}
}

void Voice::SetLoopFlag(bool loopFlag)
{
	for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++)
	{
		for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount;
		     voiceOutIndex++)
		{
			AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
			if (!axVoice)
				continue;

			axVoice->SetLoopFlag(loopFlag);
		}
	}
}

void Voice::StopAtPoint(int channelIndex, void const *baseAddress, u32 samples)
{
	for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount; voiceOutIndex++)
	{
		AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
		if (!axVoice)
			continue;

		axVoice->StopAtPoint(baseAddress, samples);
	}
}

void Voice::SetVoiceType(AxVoice::VoiceType type)
{
	for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++)
	{
		for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount;
		     voiceOutIndex++)
		{
			AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
			if (!axVoice)
				continue;

			axVoice->SetVoiceType(type);
		}
	}
}

void Voice::CalcAxSrc(bool initialUpdate)
{
	for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount; voiceOutIndex++)
	{
		f32 ratio =  mPitch;
		if (!mVoiceOutParamPitchDisableFlag)
			ratio *= mVoiceOutParam[voiceOutIndex].pitch;

		for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++)
		{
			AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
			if (!axVoice)
				continue;

			axVoice->SetSrc(ratio, initialUpdate);
		}
	}
}

void Voice::CalcAxVe()
{
	f32 baseVolume = 1.0f;
	baseVolume *= mVolume;
	baseVolume *= AxManager::GetInstance().GetOutputVolume();

	for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount; voiceOutIndex++)
	{
		f32 volume = baseVolume * mVoiceOutParam[voiceOutIndex].volume;
		f32 targetVolume = volume * mVeTargetVolume;
		f32 initVolume = volume * mVeInitVolume;

		for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++)
		{
			AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
			if (!axVoice)
				continue;

			axVoice->SetVe(targetVolume, initVolume);
		}
	}
}

bool Voice::CalcAxMix()
{
	bool nextUpdateFlag = false;

	AxVoice::MixParam mix;
	AxVoice::RemoteMixParam rmtmix ATTR_UNUSED;

	for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++)
	{
		for (int voiceOutCount = 0; voiceOutCount < mVoiceOutCount;
		     voiceOutCount++)
		{
			AxVoice *axVoice = mAxVoice[channelIndex][voiceOutCount];
			if (!axVoice)
				continue;

			CalcMixParam(channelIndex, voiceOutCount, &mix, &rmtmix);
			nextUpdateFlag |= axVoice->SetMix(mix);
		}
	}

	return nextUpdateFlag;
}

void Voice::CalcAxLpf()
{
	for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount; voiceOutIndex++)
	{
		u16 freq =
			Util::CalcLpfFreq(mLpfFreq + mVoiceOutParam[voiceOutIndex].lpf);

		for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++)
		{
			AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
			if (!axVoice)
				continue;

			axVoice->SetLpf(freq);
		}
	}
}

void Voice::CalcAxBiquadFilter()
{
	for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount; voiceOutIndex++)
	{
		for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++)
		{
			AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
			if (!axVoice)
				continue;

			axVoice->SetBiquad(mBiquadType, mBiquadValue);
		}
	}
}

void Voice::CalcAxRemoteFilter()
{
	for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount; voiceOutIndex++)
	{
		for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++)
		{
			AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
			if (!axVoice)
				continue;

			axVoice->SetRemoteFilter(mRemoteFilter);
		}
	}
}

void Voice::SyncAxVoice()
{
	// why reversed?
	for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++)
	{
		for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount;
		     voiceOutIndex++)
		{
			AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
			if (!axVoice)
				continue;

			axVoice->Sync();
		}
	}
}

void Voice::ResetDelta()
{
	for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount; voiceOutIndex++)
	{
		for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++)
		{
			AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
			if (!axVoice)
				continue;

			axVoice->ResetDelta();
		}
	}
}

void Voice::AxVoiceCallbackFunc(AxVoice *dropVoice,
                                AxVoice::AxVoiceCallbackStatus status,
                                void *callbackData)
{
	Voice *voice = static_cast<Voice *>(callbackData);
	NW4RAssertPointerNonnull_Line(1165, voice);

	VoiceCallbackStatus voiceStatus;
	bool freeDropVoice = false;

	switch (status)
	{
	case AxVoice::CALLBACK_STATUS_CANCEL:
		voiceStatus = CALLBACK_STATUS_CANCEL;
		break;

	case AxVoice::CALLBACK_STATUS_DROP_DSP:
		voiceStatus = CALLBACK_STATUS_DROP_DSP;
		freeDropVoice = true;
		break;
	}

	for (int channelIndex = 0; channelIndex < voice->mChannelCount;
	     channelIndex++)
	{
		for (int voiceOutIndex = 0; voiceOutIndex < voice->mVoiceOutCount;
		     voiceOutIndex++)
		{
			AxVoice *axVoice = voice->mAxVoice[channelIndex][voiceOutIndex];
			if (!axVoice)
				continue;

			if (axVoice == dropVoice)
			{
				if (!freeDropVoice)
					AxVoiceManager::GetInstance().FreeAxVoice(axVoice);
			}
			else
			{
				axVoice->Stop();
				AxVoiceManager::GetInstance().FreeAxVoice(axVoice);
			}

			voice->mAxVoice[channelIndex][voiceOutIndex] = nullptr;
		}
	}

	voice->mPauseFlag = false;
	voice->mStartFlag = false;
	voice->mChannelCount = 0;

	if (freeDropVoice)
		voice->Free();

	if (voice->mCallback)
		(*voice->mCallback)(voice, voiceStatus, voice->mCallbackData);
}

void Voice::TransformDpl2Pan(f32 *outPan, f32 *outSurroundPan, f32 inPan,
                             f32 inSurroundPan)
{
	inSurroundPan -= 1.0f;

	if (ut::Abs(inPan) <= ut::Abs(inSurroundPan))
	{
		if (inSurroundPan <= 0.0f)
		{
			*outPan = inPan;
			*outSurroundPan = -0.12f + 0.88f * inSurroundPan;
		}
		else
		{
			*outPan = 0.5f * inPan;
			*outSurroundPan = -0.12f + 1.12f * inSurroundPan;
		}
	}
	else if (inPan >= 0.0f)
	{
		if (inSurroundPan <= 0.0f)
		{
			*outPan = (0.85f + (1.0f - 0.85f) * (-inSurroundPan / inPan))
			        * ut::Abs(inPan);
			*outSurroundPan = -0.12f + (2.0f * inSurroundPan + 0.88f * inPan);
		}
		else
		{
			*outPan = (0.85f + (1.0f - 0.65f) * (-inSurroundPan / inPan))
			        * ut::Abs(inPan);
			*outSurroundPan = -0.12f + 1.12f * inPan;
		}
	}
	else
	{
		if (inSurroundPan <= 0.0f)
		{
			*outPan = ((1.0f - 0.85f) * (-inSurroundPan / inPan) - 0.85f)
			        * ut::Abs(inPan);
			*outSurroundPan = -0.12f + (2.0f * inSurroundPan - 1.12f * inPan);
		}
		else
		{
			*outPan = ((1.0f - 0.65f) * (-inSurroundPan / inPan) - 0.85f)
			        * ut::Abs(inPan);
			*outSurroundPan = -0.12f + 1.12f * -inPan;
		}
	}

	*outSurroundPan += 1.0f;
}

void Voice::CalcMixParam(int channelIndex, int voiceOutIndex,
                         AxVoice::MixParam *mix,
                         AxVoice::RemoteMixParam *rmtmix)
{
	NW4RAssertPointerNonnull_Line(1284, mix);

	f32 mainVolume = 0.0f;
	f32 mainSend = 0.0f;

	f32 fxSendA = 0.0f;
	f32 fxSendB = 0.0f;
	f32 fxSendC = 0.0f;

	if (mOutputLineFlag & OUTPUT_LINE_MAIN)
	{
		mainVolume = mMainOutVolume;
		mainSend = mMainSend;

		fxSendA = ut::Clamp(
			mFxSend[AUX_A] + mVoiceOutParam[voiceOutIndex].fxSend, 0.0f, 1.0f);
		fxSendB = mFxSend[AUX_B];
		fxSendC = mFxSend[AUX_C];
	}

	f32 main = mainVolume * mainSend;
	f32 fx_a = mainVolume * fxSendA;
	f32 fx_b = mainVolume * fxSendB;
	f32 fx_c = mainVolume * fxSendC;

	f32 left, right, surround, lrMixed;
	f32 front, rear;

	Util::PanInfo panInfo;
	panInfo.curve = PAN_CURVE_SQRT;
	panInfo.centerZeroFlag = false;
	panInfo.zeroClampFlag = false;

	switch (mPanCurve)
	{
	case PAN_CURVE_SQRT:
		panInfo.curve = PAN_CURVE_SQRT;
		break;

	case PAN_CURVE_SQRT_0DB:
		panInfo.curve = PAN_CURVE_SQRT;
		panInfo.centerZeroFlag = true;
		break;

	case PAN_CURVE_SQRT_0DB_CLAMP:
		panInfo.curve = PAN_CURVE_SQRT;
		panInfo.centerZeroFlag = true;
		panInfo.zeroClampFlag = true;
		break;

	case PAN_CURVE_SINCOS:
		panInfo.curve = PAN_CURVE_SQRT_0DB;
		break;

	case PAN_CURVE_SINCOS_0DB:
		panInfo.curve = PAN_CURVE_SQRT_0DB;
		panInfo.centerZeroFlag = true;
		break;

	case PAN_CURVE_SINCOS_0DB_CLAMP:
		panInfo.curve = PAN_CURVE_SQRT_0DB;
		panInfo.centerZeroFlag = true;
		panInfo.zeroClampFlag = true;
		break;

	case PAN_CURVE_LINEAR:
		panInfo.curve = PAN_CURVE_SQRT_0DB_CLAMP;
		break;

	case PAN_CURVE_LINEAR_0DB:
		panInfo.curve = PAN_CURVE_SQRT_0DB_CLAMP;
		panInfo.centerZeroFlag = true;
		break;

	case PAN_CURVE_LINEAR_0DB_CLAMP:
		panInfo.curve = PAN_CURVE_SQRT_0DB_CLAMP;
		panInfo.centerZeroFlag = true;
		panInfo.zeroClampFlag = true;
		break;

	default:
		panInfo.curve = PAN_CURVE_SQRT;
	}

	if (mChannelCount > 1 && mPanMode == PAN_MODE_BALANCE)
	{
		f32 pan = mPan + mVoiceOutParam[voiceOutIndex].pan;
		f32 surroundPan =
			mSurroundPan + mVoiceOutParam[voiceOutIndex].surroundPan;

		if (channelIndex == 0)
		{
			left = Util::CalcPanRatio(pan, panInfo);
			right = 0.0f;
		}
		else if (channelIndex == 1)
		{
			left = 0.0f;
			right = Util::CalcPanRatio(-pan, panInfo);
		}

		/* ERRATUM: left and right are used uninitialized if channelIndex is
		 * neither 0 nor 1
		 */

		front = Util::CalcSurroundPanRatio(surroundPan, panInfo);
		rear = Util::CalcSurroundPanRatio(2.0f - surroundPan, panInfo);
	}
	else
	{
		f32 voicePan = 0.0f;
		f32 pan, surroundPan;

		if (mChannelCount == 2)
		{
			if (channelIndex == 0)
				voicePan = -1.0f;
			if (channelIndex == 1)
				voicePan = 1.0f;
		}

		switch (AxManager::GetInstance().GetOutputMode())
		{
		case OUTPUT_MODE_DPL2:
			TransformDpl2Pan(
				&pan, &surroundPan,
				mPan + voicePan + mVoiceOutParam[voiceOutIndex].pan,
				mSurroundPan + mVoiceOutParam[voiceOutIndex].surroundPan);
			break;

		case OUTPUT_MODE_STEREO:
		case OUTPUT_MODE_SURROUND:
		case OUTPUT_MODE_MONO:
		default:
			pan = mPan + voicePan + mVoiceOutParam[voiceOutIndex].pan;
			surroundPan =
				mSurroundPan + mVoiceOutParam[voiceOutIndex].surroundPan;
			break;
		}

		left = Util::CalcPanRatio(pan, panInfo);
		right = Util::CalcPanRatio(-pan, panInfo);
		front = Util::CalcSurroundPanRatio(surroundPan, panInfo);
		rear = Util::CalcSurroundPanRatio(2.0f - surroundPan, panInfo);
	}

	surround = Util::CalcVolumeRatio(-3.0f);
	lrMixed = 0.5f * (left + right);

	f32 m_l, m_r, m_s;
	f32 a_l, a_r, a_s;
	f32 b_l, b_r, b_s;
	f32 c_l, c_r, c_s;

	f32 &m_sl = m_s;
	f32 &m_sr = c_l;

	f32 &a_sl = a_s;
	f32 &a_sr = c_r;

	f32 &b_sl = b_s;
	f32 &b_sr = c_s;

	switch (AxManager::GetInstance().GetOutputMode())
	{
	case OUTPUT_MODE_STEREO:
		m_l = main * left;
		m_r = main * right;
		m_s = 0.0f;

		a_l = fx_a * left;
		a_r = fx_a * right;
		a_s = 0.0f;

		b_l = fx_b * left;
		b_r = fx_b * right;
		b_s = 0.0f;

		c_l = fx_c * left;
		c_r = fx_c * right;
		c_s = 0.0f;

		break;

	case OUTPUT_MODE_MONO:
		m_l = main * lrMixed;
		m_r = main * lrMixed;
		m_s = 0.0f;

		a_l = fx_a * lrMixed;
		a_r = fx_a * lrMixed;
		a_s = 0.0f;

		b_l = fx_b * lrMixed;
		b_r = fx_b * lrMixed;
		b_s = 0.0f;

		c_l = fx_c * lrMixed;
		c_r = fx_c * lrMixed;
		c_s = 0.0f;

		break;

	case OUTPUT_MODE_SURROUND:
	{
		f32 fl = left * front;
		f32 fr = right * front;
		f32 rs = surround * rear;

		m_l = main * fl;
		m_r = main * fr;
		m_s = main * rs;

		a_l = fx_a * fl;
		a_r = fx_a * fr;
		a_s = fx_a * rs;

		b_l = fx_b * fl;
		b_r = fx_b * fr;
		b_s = fx_b * rs;

		c_l = fx_c * fl;
		c_r = fx_c * fr;
		c_s = fx_c * rs;
	}
		break;

	case OUTPUT_MODE_DPL2:
	{
		f32 fl = left * front;
		f32 fr = right * front;
		f32 rl = left * rear;
		f32 rr = right * rear;

		m_l = main * fl;
		m_r = main * fr;
		m_sl = main * rl;
		m_sr = main * rr;

		a_l = fx_a * fl;
		a_r = fx_a * fr;
		a_sl = fx_a * rl;
		a_sr = fx_a * rr;

		b_l = fx_b * fl;
		b_r = fx_b * fr;
		b_sl = fx_b * rl;
		b_sr = fx_b * rr;
	}
		break;

	default:
		break;
	}

	mix->vL		= CalcMixVolume(m_l);
	mix->vR		= CalcMixVolume(m_r);
	mix->vS		= CalcMixVolume(m_s);
	mix->vAuxAL	= CalcMixVolume(a_l);
	mix->vAuxAR	= CalcMixVolume(a_r);
	mix->vAuxAS	= CalcMixVolume(a_s);
	mix->vAuxBL	= CalcMixVolume(b_l);
	mix->vAuxBR	= CalcMixVolume(b_r);
	mix->vAuxBS	= CalcMixVolume(b_s);
	mix->vAuxCL	= CalcMixVolume(c_l);
	mix->vAuxCR	= CalcMixVolume(c_r);
	mix->vAuxCS	= CalcMixVolume(c_s);

	rmtmix->vMain0	= 0;
	rmtmix->vAux0	= 0;
	rmtmix->vMain1	= 0;
	rmtmix->vAux1	= 0;
	rmtmix->vMain2	= 0;
	rmtmix->vAux2	= 0;
	rmtmix->vMain3	= 0;
	rmtmix->vAux3	= 0;
}

void Voice::RunAllAxVoice()
{
	for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++)
	{
		for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount;
		     voiceOutIndex++)
		{
			AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
			if (!axVoice)
				continue;

			axVoice->Run();
		}
	}
}

void Voice::StopAllAxVoice()
{
	for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++)
	{
		for (int voiceOutIndex = 0; voiceOutIndex < mVoiceOutCount;
		     voiceOutIndex++)
		{
			AxVoice *axVoice = mAxVoice[channelIndex][voiceOutIndex];
			if (!axVoice)
				continue;

			axVoice->Stop();
		}
	}
}

void Voice::InvalidateWaveData(void const *start, void const *end)
{
	bool disposeFlag = false;

	for (int channelIndex = 0; channelIndex < mChannelCount; channelIndex++)
	{
		AxVoice *axVoice = mAxVoice[channelIndex][0];

		if (axVoice && axVoice->IsDataAddressCoverd(start, end))
		{
			disposeFlag = true;
			break;
		}
	}

	if (disposeFlag)
	{
		Stop();

		if (mCallback)
			(*mCallback)(this, CALLBACK_STATUS_CANCEL, mCallbackData);
	}
}
}}} // namespace nw4r::snd::detail
