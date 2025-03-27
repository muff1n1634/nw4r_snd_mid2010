#ifndef NW4R_SND_SEQ_TRACK_H
#define NW4R_SND_SEQ_TRACK_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "Channel.h"
#include "global.h"
#include "Lfo.h"
#include "MoveValue.h"

/*******************************************************************************
 * types
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail { class SeqPlayer; }}}

namespace nw4r { namespace snd
{
	enum ParseResult
	{
		PARSE_RESULT_CONTINUE,
		PARSE_RESULT_FINISH,
	};

	enum SeqMute
	{
		MUTE_OFF,
		MUTE_NO_STOP,
		MUTE_RELEASE,
		MUTE_STOP,
	};
}} // namespace nw4r::snd

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	class SeqTrack
	{
	// nested types
	public:
		struct CallStack
		{
			bool			loopFlag;	// size 0x01, offset 0x00
			u8				loopCount;	// size 0x01, offset 0x01
			/* 2 bytes padding */
			byte_t const	*address;	// size 0x04, offset 0x04
		}; // size 0x08

		struct ParserTrackParam
		{
			byte_t const		*baseAddr;				// (0x1c) size 0x04, offset 0x00
			byte_t const		*currentAddr;			// (0x20) size 0x04, offset 0x04
			bool				cmpFlag;				// (0x24) size 0x01, offset 0x08
			bool				noteWaitFlag;			// (0x25) size 0x01, offset 0x09
			bool				tieFlag;				// (0x26) size 0x01, offset 0x0a
			bool				monophonicFlag;			// (0x27) size 0x01, offset 0x0b
			CallStack			callStack[3];			// (0x28) size 0x18, offset 0x0c
			u8					callStackDepth;			// (0x40) size 0x01, offset 0x24
			s32					wait;					// (0x44) size 0x04, offset 0x28
			bool				muteFlag;				// (0x48) size 0x01, offset 0x2c
			bool				silenceFlag;			// (0x49) size 0x01, offset 0x2d
			bool				noteFinishWait;			// (0x4a) size 0x01, offset 0x2e
			bool				portaFlag;				// (0x4b) size 0x01, offset 0x2f
			bool				damperFlag;				// (0x4c) size 0x01, offset 0x30
			int					bankNo;					// (0x50) size 0x04, offset 0x34
			int					prgNo;					// (0x54) size 0x04, offset 0x38
			LfoParam			lfoParam;				// (0x58) size 0x10, offset 0x3c
			u8					lfoTarget;				// (0x68) size 0x01, offset 0x4c
			f32					sweepPitch;				// (0x6c) size 0x04, offset 0x50
			MoveValue<u8, s16>	volume;					// (0x70) size 0x06, offset 0x54
			MoveValue<s8, s16>	pan;					// (0x76) size 0x06, offset 0x5a
			MoveValue<s8, s16>	surroundPan;			// (0x7c) size 0x06, offset 0x60
			u8					volume2;				// (0x82) size 0x01, offset 0x66
			u8					velocityRange;			// (0x83) size 0x01, offset 0x67
			s8					pitchBend;				// (0x84) size 0x01, offset 0x68
			u8					bendRange;				// (0x85) size 0x01, offset 0x69
			s8					initPan;				// (0x86) size 0x01, offset 0x6a
			s8					transpose;				// (0x87) size 0x01, offset 0x6b
			u8					priority;				// (0x88) size 0x01, offset 0x6c
			u8					portaKey;				// (0x89) size 0x01, offset 0x6d
			u8					portaTime;				// (0x8a) size 0x01, offset 0x6e
			u8					attack;					// (0x8b) size 0x01, offset 0x6f
			u8					decay;					// (0x8c) size 0x01, offset 0x70
			u8					sustain;				// (0x8d) size 0x01, offset 0x71
			u8					release;				// (0x8e) size 0x01, offset 0x72
			s16					envHold;				// (0x90) size 0x02, offset 0x74
			u8					mainSend;				// (0x92) size 0x01, offset 0x76
			u8					fxSend[AUX_BUS_NUM];	// (0x93) size 0x03, offset 0x77
			u8					biquadType;				// (0x96) size 0x01, offset 0x7a
			f32					lpfFreq;				// (0x98) size 0x04, offset 0x7c
			f32					biquadValue;			// (0x9c) size 0x04, offset 0x80
		}; // size 0x84

	// methods
	public:
		// cdtors
		SeqTrack();
		virtual ~SeqTrack();

		// virtual function ordering
		// vtable SeqTrack
		virtual ParseResult Parse(bool doNoteOn) = 0;

		void SetSeqData(const void *seqBase, s32 seqOffset);
		void Open();
		void Close();

		void ReleaseAllChannel(int release);
		void FreeAllChannel();
		void StopAllChannel();

		int GetPlayerTrackNo() const { return mPlayerTrackNo; }

		SeqPlayer *GetSeqPlayer()
			{ return mSeqPlayer; }

		void SetSeqPlayer(SeqPlayer *player) { mSeqPlayer = player; }

		ParserTrackParam &GetParserTrackParam()
			{
				return mParserTrackParam;
			}

		volatile s16 *GetVariablePtr(int i);

		Channel* NoteOn(int key, int velocity, s32 length, bool tieFlag);

		void SetMute(SeqMute mute);

		void PauseAllChannel(bool flag);
		void UpdateChannelLength();
		int ParseNextTick(bool doNoteOn);
		bool IsOpened() const { return mOpenFlag; }
		void SetPlayerTrackNo(int playerTrackNo);
		void UpdateChannelParam();

		// declarations for PTMFs in snd_SeqPlayer.cpp
		void SetSilence(bool, int);
		void SetVolume(float);
		void SetPitch(float);
		void SetPan(float);
		void SetSurroundPan(float);
		void SetLpfFreq(float);
		void SetBiquadFilter(int, float);
		void SetPanRange(float);
		void SetModDepth(float);
		void SetModSpeed(float);

		void InitParam();
		void UpdateChannelRelease(Channel *channel);

		static void ChannelCallbackFunc(Channel *dropChannel,
		                                Channel::ChannelCallbackStatus status,
		                                register_t userData);

		Channel *GetLastChannel() const { return mChannelList; }
		void AddChannel(Channel *);

	public:
		static const int VARIABLE_NUM = 16;
		static const int PRGNO_MAX = 0xffff;

		static const int DEFAULT_VARIABLE_VALUE = -1;

	// members
	private:
		/* vtable */										// size 0x04, offset 0x00
		u8					mPlayerTrackNo;					// size 0x01, offset 0x04
		bool				mOpenFlag;						// size 0x01, offset 0x05
		/* 2 bytes padding */
		f32					mExtVolume;						// size 0x04, offset 0x08
		f32					mExtPitch;						// size 0x04, offset 0x0c
		f32					mExtPan;						// size 0x04, offset 0x10
		f32					mExtSurroundPan;				// size 0x04, offset 0x14
		f32					mPanRange;						// size 0x04, offset 0x18
		ParserTrackParam	mParserTrackParam;				// size 0x84, offset 0x1c
		s16 volatile		mTrackVariable[VARIABLE_NUM];	// size 0x20, offset 0xa0
		SeqPlayer			*mSeqPlayer;					// size 0x04, offset 0xc0
		Channel				*mChannelList;					// size 0x04, offset 0xc4
	}; // size 0xc8
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_SEQ_TRACK_H
