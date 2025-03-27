#ifndef NW4R_SND_MML_PARSER_H
#define NW4R_SND_MML_PARSER_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "SeqTrack.h"

/*******************************************************************************
 * classes and functions
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail { class Channel; }}}
namespace nw4r { namespace snd { namespace detail { class MmlSeqTrack; }}}
namespace nw4r { namespace snd { namespace detail { class SeqPlayer; } } }

namespace nw4r { namespace snd { namespace detail
{
	class MmlParser
	{
	// enums
	public:
		enum SeqArgType
		{
			SEQ_ARG_NONE,

			SEQ_ARG_U8,
			SEQ_ARG_S16,
			SEQ_ARG_VMIDI,
			SEQ_ARG_RANDOM,
			SEQ_ARG_VARIABLE,
		};

	private:
		enum MmlSeqData
		{
			MML_CMD_MIN = 0x80, // <80 -> MML note, not a command

			MML_WAIT = 0x80,
			MML_SET_PRGNO,
			MML_OPENTRACK = 0x88,
			MML_JUMP,
			MML_CALL,

			MML_RNDPARAM = 0xa0,
			MML_INDPARAM,
			MML_EXECIF,
			MML_CMD_A3,
			MML_CMD_A4,
			MML_CMD_A5,

			MML_SET_TIMEBASE = 0xb0,
			MML_SET_ENV_HOLD,
			MML_SET_MONOPHONIC,
			MML_SET_TRACK_VELOCITY_RANGE,
			MML_SET_BIQUAD_TYPE,
			MML_SET_BIQUAD_VALUE,

			MML_SET_PAN = 0xc0,
			MML_SET_TRACK_VOLUME,
			MML_SET_PLAYER_VOLUME,
			MML_SET_TRANSPOSE,
			MML_SET_PITCHBEND,
			MML_SET_BENDRANGE,
			MML_SET_PRIORITY,
			MML_SET_NOTEWAIT,
			MML_SET_TIE,
			MML_SET_PORTAMENTO,
			MML_SET_LFODEPTH,
			MML_SET_LFOSPEED,
			MML_SET_LFOTARGET,
			MML_SET_LFORANGE,
			MML_SET_PORTASPEED,
			MML_SET_PORTATIME,

			MML_SET_ATTACK = 0xd0,
			MML_SET_DECAY,
			MML_SET_SUSTAIN,
			MML_SET_RELEASE,
			MML_LOOP_START,
			MML_SET_TRACK_VOLUME2,
			MML_PRINTVAR,
			MML_SET_SURROUNDPAN,
			MML_SET_LPFFREQ,
			MML_SET_FXSEND_A,
			MML_SET_FXSEND_B,
			MML_SET_MAINSEND,
			MML_SET_INITPAN,
			MML_SET_MUTE,
			MML_SET_FXSEND_C,
			MML_SET_DAMPER,

			MML_SET_LFODELAY = 0xe0,
			MML_SET_TEMPO,
			MML_SET_SWEEPPITCH = 0xe3,

			MML_RESET_ADSR = 0xfb,
			MML_LOOP_END,
			MML_RET,
			MML_ALLOCTRACK,
			MML_EOF,

			MML_CMD_MASK = 0x80,
			MML_CMD_SET_MASK = 0xf0,

			MML_CMD_MAX = 0xff,

			MML_EX_COMMAND = 0xf0,

			MML_EX_ARITHMETIC = 0x80,
			MML_EX_SET = 0x80,
			MML_EX_APL,
			MML_EX_AMI,
			MML_EX_AMU,
			MML_EX_ADV,
			MML_EX_ALS,
			MML_EX_RND,
			MML_EX_AAD,
			MML_EX_AOR,
			MML_EX_AER,
			MML_EX_ACO,
			MML_EX_AMD,

			MML_EX_LOGIC = 0x90,
			MML_EX_EQ = 0x90,
			MML_EX_GE,
			MML_EX_GT,
			MML_EX_LE,
			MML_EX_LT,
			MML_EX_NE,

			MML_EX_USERPROC = 0xe0,

			MML_EX_CMD_MAX = 0xffff
		};

	// methods
	public:
		virtual void CommandProc(MmlSeqTrack *track, u32 command, s32 arg1,
		                         s32 arg2) const;
		virtual Channel *NoteOnCommandProc(MmlSeqTrack *track, int key,
		                                   int velocity, s32 length,
		                                   bool tie) const;

		// methods
		ParseResult Parse(MmlSeqTrack *track, bool doNoteOn) const;

		volatile s16 *GetVariablePtr(SeqPlayer *player, SeqTrack *track,
		                             int varNo) const;

		static void EnablePrintVar(bool enable)
		{
			mPrintVarEnabledFlag = enable;
		}

		static u32 ParseAllocTrack(const void *baseAddress, u32 seqOffset,
		                           byte4_t *allocTrack);

	// reading stuff
	private:
		byte1_t ReadByte(const byte_t **ptr) const { return *(*ptr)++; }
		byte2_t Read16(const byte_t **ptr) const;
		byte4_t Read24(const byte_t **ptr) const;
		s32 ReadVar(const byte_t **ptr) const;
		s32 ReadArg(const byte_t **ptr, SeqPlayer *player, SeqTrack *track,
		            SeqArgType type) const;

	// static members
	public:
		static const int CALL_STACK_MAX_DEPTH = 3;

	private:
		static bool mPrintVarEnabledFlag;

	// members
	private:
		/* vtable */	// size 0x04, offset 0x00
	}; // size 0x04
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_MML_PARSER_H
