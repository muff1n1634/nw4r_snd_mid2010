#ifndef NW4R_SND_AX_VOICE_H
#define NW4R_SND_AX_VOICE_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "global.h"

#include "../ut/inlines.h" // ut::Min
#include "../ut/LinkList.h"

#if 0
#include <revolution/AX/AX.h>
#include <revolution/AX/AXAlloc.h> // AXSetVoicePriority
#include <revolution/AX/AXVPB.h>
#else
#include <context_rvl.h>
#endif

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2ae88
	class AxVoiceParamBlock
	{
	// methods
	public:
		// cdtors
		AxVoiceParamBlock();

		// conversion operators
		operator AXVPB *() { return mVpb; }

		// methods
		bool IsAvailable() const { return mVpb != nullptr; }

		bool IsRun() const
		{
			return IsAvailable() && mVpb->pb.state == AX_VOICE_RUN;
		}

		bool IsLpfEnable() const
		{
			return IsAvailable() && mVpb->pb.lpf.on == AX_PB_LPF_ON;
		}

		bool IsBiquadEnable() const
		{
			return IsAvailable() && mVpb->pb.biquad.on == AX_PB_BIQUAD_ON;
		}

		u32 GetCurrentAddress() const
		{
			if (!IsAvailable())
				return 0;

			return (mVpb->pb.addr.currentAddressHi << 16)
			     + mVpb->pb.addr.currentAddressLo;
		}

		u32 GetLoopAddress() const
		{
			if (!IsAvailable())
				return 0;

			return (mVpb->pb.addr.loopAddressHi << 16)
			     + mVpb->pb.addr.loopAddressLo;
		}

		u32 GetEndAddress() const
		{
			if (!IsAvailable())
				return 0;

			return (mVpb->pb.addr.endAddressHi << 16)
			     + mVpb->pb.addr.endAddressLo;
		}

		void SetVoiceAddr(const AXPBADDR &addr)
		{
			if (!IsAvailable())
				return;

			// AXSetVoiceAddr doesn't actually modify the object
			AXSetVoiceAddr(mVpb, const_cast<AXPBADDR *>(&addr));
		}

		void SetVoicePriority(u32 priority)
		{
			if (!IsAvailable())
				return;

			AXSetVoicePriority(mVpb, priority);
		}

		void SetVoiceStateRun()
		{
			if (!IsAvailable())
				return;

			AXSetVoiceState(mVpb, AX_VOICE_RUN);
		}

		void SetVoiceStateStop()
		{
			if (!IsRun())
				return;

			AXSetVoiceState(mVpb, AX_VOICE_STOP);
		}

		void Sync();
		bool IsRmtIirEnable() const;

		void Set(AXVPB *vpb);
		void Clear();

		void SetVoiceType(u16 type);
		void SetVoiceVe(u16 volume, u16 initVolume);
		void SetVoiceMix(const AXPBMIX &mix, bool immediatelySync);
		void SetVoiceLoop(u16 loop);
		void SetVoiceLoopAddr(u32 addr);
		void SetVoiceEndAddr(u32 addr);
		void SetVoiceAdpcm(const AXPBADPCM &adpcm);
		void SetVoiceSrcType(u32 type);
		void SetVoiceSrc(const AXPBSRC &src);
		void SetVoiceSrcRatio(f32 ratio);
		void SetVoiceAdpcmLoop(const AXPBADPCMLOOP &adpcmloop);
		void SetVoiceLpf(const AXPBLPF &lpf);
		void SetVoiceLpfCoefs(u16 a0, u16 b0);
		void SetVoiceBiquad(const AXPBBIQUAD &biquad);
		void SetVoiceBiquadCoefs(u16 b0, u16 b1, u16 b2, u16 a1, u16 a2);
		void SetVoiceRmtOn(u16 on);
		void SetVoiceRmtMix(const AXPBRMTMIX &mix);
		void SetVoiceRmtIIR(const __AXPBRMTIIR &iir);
		void SetVoiceRmtIIRCoefs(u16 type, ...);
		void UpdateDelta();

	// members
	private:
		AXVPB			*mVpb;				// size 0x04, offset 0x00
		u32				mSync;				// size 0x04, offset 0x04
		volatile AXPBVE	mPrevVeSetting;		// size 0x04, offset 0x08
		bool			mFirstVeUpdateFlag;	// size 0x01, offset 0x0c
		/* 1 byte padding */
		u16				mVolume;			// size 0x02, offset 0x0e
	}; // size 0x10

	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b45f
	class AxVoice
	{
	// enums
	public:
		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b187
		enum AxVoiceCallbackStatus
		{
			CALLBACK_STATUS_CANCEL,
			CALLBACK_STATUS_DROP_DSP,
		};

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b23a
		enum VoiceType
		{
			VOICE_TYPE_NORMAL,
			VOICE_TYPE_STREAM,
		};

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b3dd
		enum SrcType
		{
			SRC_NONE,
			SRC_LINEAR,
			SRC_4TAP_8K,
			SRC_4TAP_12K,
			SRC_4TAP_16K,
			SRC_4TAP_AUTO,
		};

	// typedefs
	public:
		typedef ut::LinkList<AxVoice, 64> LinkList;

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b1ef
		typedef void Callback(AxVoice *dropVoice, AxVoiceCallbackStatus status,
		                      void *callbackData);

		// nested types
	public:
		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2afaa
		struct MixParam
		{
			u16	vL;		// size 0x02, offset 0x00
			u16	vR;		// size 0x02, offset 0x02
			u16	vS;		// size 0x02, offset 0x04
			u16	vAuxAL;	// size 0x02, offset 0x06
			u16	vAuxAR;	// size 0x02, offset 0x08
			u16	vAuxAS;	// size 0x02, offset 0x0a
			u16	vAuxBL;	// size 0x02, offset 0x0c
			u16	vAuxBR;	// size 0x02, offset 0x0e
			u16	vAuxBS;	// size 0x02, offset 0x10
			u16	vAuxCL;	// size 0x02, offset 0x12
			u16	vAuxCR;	// size 0x02, offset 0x14
			u16	vAuxCS;	// size 0x02, offset 0x16
		}; // size 0x18

		// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b28a
		struct RemoteMixParam
		{
			u16	vMain0;	// size 0x02, offset 0x00
			u16	vAux0;	// size 0x02, offset 0x02
			u16	vMain1;	// size 0x02, offset 0x04
			u16	vAux1;	// size 0x02, offset 0x06
			u16	vMain2;	// size 0x02, offset 0x08
			u16	vAux2;	// size 0x02, offset 0x0a
			u16	vMain3;	// size 0x02, offset 0x0c
			u16	vAux3;	// size 0x02, offset 0x0e
		}; // size 0x10

	// methods
	public:
		// cdtors
		AxVoice();
		~AxVoice();

		// methods
		void Setup(const void *waveAddr, SampleFormat format, int sampleRate);

		void Run() { mVpb.SetVoiceStateRun(); }
		bool IsRun() const { return mVpb.IsRun(); }
		void Stop() { mVpb.SetVoiceStateStop(); }
		void Sync() { mVpb.Sync(); }

		bool IsPlayFinished() const;

		void SetLoopStart(const void *baseAddress, u32 samples);
		void SetLoopEnd(const void *baseAddress, u32 samples);
		void SetLoopFlag(bool loopFlag);
		void StopAtPoint(const void *baseAddress, u32 samples);

		/* NOTE: covered misspelled as coverd */
		bool IsDataAddressCoverd(const void *beginAddress,
		                         const void *endAddress) const;
		u32 GetCurrentPlayingSample() const;
		u32 GetCurrentPlayingDspAddress() const;
		u32 GetLoopEndDspAddress() const;
		SampleFormat GetFormat() const { return mFormat; }
		f32 GetDspRatio(f32 ratio) const
		{
			return ratio * mSampleRate / AX_SAMPLE_RATE;
		}

		void SetPriority(u32 priority);
		void SetVoiceType(VoiceType type);
		void ResetDelta();
		void SetAddr(bool loopFlag, const void *waveAddr, u32 startOffset,
		             u32 loopStart, u32 loopEnd);
		void SetSrcType(SrcType type, f32 pitch);
		void SetAdpcm(const AdpcmParam *param);
		bool IsNeedNextUpdate(const MixParam &param) const;
		void SetAdpcmLoop(const AdpcmLoopParam *param);
		bool SetMix(const MixParam &param);
		void SetSrc(f32 ratio, bool initialUpdate);
		void SetVe(f32 volume, f32 initVolume);
		void SetLpf(u16 freq);
		void SetBiquad(u8 filterType, f32 value);
		void SetRemoteFilter(u8 filter);

		static u32 GetDspAddressBySample(const void *baseAddress, u32 samples,
		                                  SampleFormat format);
		static u32 GetSampleByDspAddress(const void *baseAddress, u32 addr,
		                                  SampleFormat format);

		static u16 GetAxFormatFromSampleFormat(SampleFormat sampleFormat);

		static void CalcOffsetAdpcmParam(u16 *outPredScale, u16 *outYn1,
		                                 u16 *outYn2, u32 offset,
		                                 void *dataAddr,
		                                 const AdpcmParam &adpcmParam);

	// callbacks
	private:
		static void VoiceCallback(void *callbackData);

	// methods
	private:
		AxVoiceParamBlock	mVpb;					// size 0x10, offset 0x00
		const void			*mWaveData;				// size 0x04, offset 0x10
		SampleFormat		mFormat;				// size 0x04, offset 0x14
		int					mSampleRate;			// size 0x04, offset 0x18
		bool				mFirstMixUpdateFlag;	// size 0x01, offset 0x1c
		bool				mReserveForFreeFlag;	// size 0x01, offset 0x1d
		/* 2 bytes padding */
		MixParam			mMixPrev;				// size 0x18, offset 0x20
		Callback			*mCallback;				// size 0x04, offset 0x38
		void				*mCallbackData;			// size 0x04, offset 0x3c
	public:
		ut::LinkListNode	node;					// size 0x08, offset 0x40

	// friends
	private:
		friend class AxVoiceManager;
	}; // size 0x48

	inline int CalcAxvpbDelta(u16 initValue, u16 targetValue)
	{
		if (initValue == targetValue)
			return 0;

		return (targetValue - initValue) / AX_SAMPLES_PER_FRAME;
	}

	inline u16 CalcMixVolume(f32 volume)
	{
		if (volume <= 0.0f)
			return 0;

		return ut::Min<u32>(0xffff, AX_MAX_VOLUME * volume);
	}
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_AX_VOICE_H
