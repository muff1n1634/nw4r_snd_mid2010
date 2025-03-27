#include "nw4r/snd/MmlParser.h"

/* Original source:
 * kiwi515/ogws
 * src/nw4r/snd/snd_MmlParser.cpp
 */

/*******************************************************************************
 * headers
 */

#include <macros.h>
#include <types.h>

#include "nw4r/snd/global.h"
#include "nw4r/snd/Lfo.h"
#include "nw4r/snd/MmlSeqTrack.h"
#include "nw4r/snd/MoveValue.h"
#include "nw4r/snd/SeqPlayer.h"
#include "nw4r/snd/SeqTrack.h"
#include "nw4r/snd/Util.h"

#include "nw4r/ut/inlines.h"

#if 0
#include <revolution/OS/OSReport.h>
#else
#include <context_rvl.h>
#endif

#include "nw4r/NW4RAssert.hpp"

/*******************************************************************************
 * variables
 */

namespace nw4r { namespace snd { namespace detail
{
	// .sbss
	bool MmlParser::mPrintVarEnabledFlag = false;
}}} // namespace nw4r::snd::detail

/*******************************************************************************
 * functions
 */

namespace nw4r { namespace snd { namespace detail {

ParseResult MmlParser::Parse(MmlSeqTrack *track, bool doNoteOn) const
{
	NW4RAssertPointerNonnull_Line(49, track);

	SeqPlayer *player = track->GetSeqPlayer();
	NW4RAssertPointerNonnull_Line(51, player);

	SeqTrack::ParserTrackParam &trackParam = track->GetParserTrackParam();
	SeqPlayer::ParserPlayerParam &playerParam ATTR_UNUSED =
		player->GetParserPlayerParam();

	SeqArgType argType;
	SeqArgType argType2 = SEQ_ARG_NONE;

	bool useArgType = false;
	bool doExecCommand = true;

	u32 cmd = ReadByte(&trackParam.currentAddr);

	if (cmd == MML_EXECIF)
	{
		cmd = ReadByte(&trackParam.currentAddr);
		doExecCommand = trackParam.cmpFlag != false;
	}

	if (cmd == MML_CMD_A3)
	{
		cmd = ReadByte(&trackParam.currentAddr);
		argType2 = SEQ_ARG_S16;
	}
	else if (cmd == MML_CMD_A4)
	{
		cmd = ReadByte(&trackParam.currentAddr);
		argType2 = SEQ_ARG_RANDOM;
	}
	else if (cmd == MML_CMD_A5)
	{
		cmd = ReadByte(&trackParam.currentAddr);
		argType2 = SEQ_ARG_VARIABLE;
	}

	if (cmd == MML_RNDPARAM)
	{
		cmd = ReadByte(&trackParam.currentAddr);
		argType = SEQ_ARG_RANDOM;
		useArgType = true;
	}
	else if (cmd == MML_INDPARAM)
	{
		cmd = ReadByte(&trackParam.currentAddr);
		argType = SEQ_ARG_VARIABLE;
		useArgType = true;
	}

	if (!(cmd & MML_CMD_MASK))
	{
		// MML note data, not a command
		u8 velocity = ReadByte(&trackParam.currentAddr);

		s32 length = ReadArg(&trackParam.currentAddr, player, track,
		                     useArgType ? argType : SEQ_ARG_VMIDI);

		int key = cmd + trackParam.transpose;

		if (!doExecCommand)
			return PARSE_RESULT_CONTINUE;

		key = ut::Clamp(key, 0, 127);

		if (!trackParam.muteFlag && doNoteOn)
		{
			NoteOnCommandProc(track, key, velocity, length > 0 ? length : -1,
			                  trackParam.tieFlag);
		}

		if (trackParam.noteWaitFlag)
		{
			trackParam.wait = length;

			if (length == 0)
				trackParam.noteFinishWait = true;
		}
	}
	else
	{
		// MML command
		s32 commandArg1 = 0;
		s32 commandArg2 = 0;

		switch (static_cast<int>(cmd & MML_CMD_SET_MASK))
		{
		case 0x80:
		{
			switch (cmd)
			{
			case MML_WAIT:
			{
				s32 arg = ReadArg(&trackParam.currentAddr, player, track,
				                   useArgType ? argType : SEQ_ARG_VMIDI);

				if (doExecCommand)
					trackParam.wait = arg;
			}
				break;

			case MML_SET_PRGNO:
				commandArg1 = ReadArg(&trackParam.currentAddr, player, track,
				               useArgType ? argType : SEQ_ARG_VMIDI);

				if (doExecCommand)
					CommandProc(track, cmd, commandArg1, commandArg2);

				break;

			case MML_OPENTRACK:
			{
				u8 trackNo = ReadByte(&trackParam.currentAddr);
				u32 offset = Read24(&trackParam.currentAddr);

				if (doExecCommand)
				{
					commandArg1 = trackNo;
					commandArg2 = offset;
					CommandProc(track, cmd, commandArg1, commandArg2);
				}
			}
				break;

			case MML_JUMP:
			{
				u32 offset = Read24(&trackParam.currentAddr);

				if (doExecCommand)
				{
					commandArg1 = offset;
					CommandProc(track, cmd, commandArg1, commandArg2);
				}
			}
				break;

			case MML_CALL:
			{
				u32 offset = Read24(&trackParam.currentAddr);

				if (doExecCommand)
				{
					commandArg1 = offset;
					CommandProc(track, cmd, commandArg1, commandArg2);
				}
			}
				break;
			}

			break;
		}

		case 0xb0:
		case 0xc0:
		case 0xd0:
		{
			u8 arg = ReadArg(&trackParam.currentAddr, player, track,
			                 useArgType ? argType : SEQ_ARG_U8);

			if (argType2 != SEQ_ARG_NONE)
			{
				commandArg2 =
					ReadArg(&trackParam.currentAddr, player, track, argType2);
			}

			if (!doExecCommand)
				break;

			switch (cmd)
			{
			case MML_SET_TRANSPOSE:
			case MML_SET_PITCHBEND:
				commandArg1 = *reinterpret_cast<s8 *>(&arg);
				break;

			default:
				commandArg1 = *reinterpret_cast<u8 *>(&arg);
				break;
			}

			CommandProc(track, cmd, commandArg1, commandArg2);
			break;
		}

		case 0x90:
			if (doExecCommand)
				CommandProc(track, cmd, commandArg1, commandArg2);

			break;

		case 0xe0:
			commandArg1 =
				static_cast<s16>(ReadArg(&trackParam.currentAddr, player, track,
			                             useArgType ? argType : SEQ_ARG_S16));

			if (doExecCommand)
				CommandProc(track, cmd, commandArg1, commandArg2);

			break;

		case 0xf0:
		{
			switch (cmd)
			{
			case MML_ALLOCTRACK:
				Read16(&trackParam.currentAddr);
				NW4RAssertMessage_Line(
					312, false, "seq: must use alloctrack in startup code");

				break;

			case MML_EOF:
				if (doExecCommand)
					return PARSE_RESULT_FINISH;

				break;

			case MML_EX_COMMAND:
			{
				u32 cmdex = ReadByte(&trackParam.currentAddr);

				switch (cmdex & MML_CMD_SET_MASK)
				{
				case MML_EX_USERPROC:
					commandArg1 = static_cast<u16>(
						ReadArg(&trackParam.currentAddr, player, track,
						        useArgType ? argType : SEQ_ARG_S16));

					if (doExecCommand)
					{
						CommandProc(track, (cmd << 8) + cmdex, commandArg1,
						            commandArg2);
					}

					break;

				case MML_EX_ARITHMETIC:
				case MML_EX_LOGIC:
					commandArg1 = ReadByte(&trackParam.currentAddr);
					commandArg2 = static_cast<s16>(
						ReadArg(&trackParam.currentAddr, player, track,
						        useArgType ? argType : SEQ_ARG_S16));

					if (doExecCommand)
					{
						CommandProc(track, (cmd << 8) + cmdex, commandArg1,
						            commandArg2);
					}

					break;
				}
			}
				ATTR_FALLTHROUGH;

			default:
				if (doExecCommand)
					CommandProc(track, cmd, commandArg1, commandArg2);

				break;
			}
		}
			break;

		case 0xa0:
			NW4RWarningMessage_Line(392, "Invalid seqdata command: %d", cmd);
			break;
		}

	}

	return PARSE_RESULT_CONTINUE;
}

void MmlParser::CommandProc(MmlSeqTrack *track, byte4_t command, s32 arg1,
                            s32 arg2) const
{
	NW4RAssertPointerNonnull_Line(421, track);

	SeqPlayer* player = track->GetSeqPlayer();
	NW4RAssertPointerNonnull_Line(423, player);

	SeqTrack::ParserTrackParam &trackParam = track->GetParserTrackParam();
	SeqPlayer::ParserPlayerParam &playerParam = player->GetParserPlayerParam();

	if (command <= MML_CMD_MAX)
	{
		switch (command)
		{
		case MML_SET_TEMPO:
			playerParam.tempo = ut::Clamp<int>(arg1, 0, 0x3ff);
			break;

		case MML_SET_TIMEBASE:
			playerParam.timebase = arg1;
			break;

		case MML_SET_PRGNO:
			if (arg1 < SeqTrack::PRGNO_MAX + 1)
			{
				trackParam.prgNo = arg1 & SeqTrack::PRGNO_MAX;
			}
			else
			{
				NW4RWarningMessage_Line(
					449, "nw4r::snd::MmlParser: too large prg No. %d", arg1);
			}

			break;

		case MML_SET_MUTE:
			track->SetMute(static_cast<SeqMute>(arg1));
			break;

		case MML_SET_TRACK_VOLUME:
			trackParam.volume.SetTarget(arg1, arg2);
			break;

		case MML_SET_TRACK_VOLUME2:
			trackParam.volume2 = arg1;
			break;

		case MML_SET_TRACK_VELOCITY_RANGE:
			trackParam.velocityRange = arg1;
			break;

		case MML_SET_PLAYER_VOLUME:
			playerParam.volume = arg1;
			break;

		case MML_SET_TRANSPOSE:
			trackParam.transpose = arg1;
			break;

		case MML_SET_PITCHBEND:
			trackParam.pitchBend = arg1;
			break;

		case MML_SET_BENDRANGE:
			trackParam.bendRange = arg1;
			break;

		case MML_SET_PAN:
			trackParam.pan.SetTarget(arg1 - 64, arg2);
			break;

		case MML_SET_INITPAN:
			trackParam.initPan = arg1 - 64;
			break;

		case MML_SET_SURROUNDPAN:
			trackParam.surroundPan.SetTarget(arg1, arg2);
			break;

		case MML_SET_PRIORITY:
			trackParam.priority = arg1;
			break;

		case MML_SET_NOTEWAIT:
			trackParam.noteWaitFlag = arg1;
			break;

		case MML_SET_PORTATIME:
			trackParam.portaTime = arg1;
			break;

		case MML_SET_LFODEPTH:
			trackParam.lfoParam.depth = static_cast<u8>(arg1) / 128.0f;
			break;

		case MML_SET_LFOSPEED:
			trackParam.lfoParam.speed =
				static_cast<u8>(arg1) * (100.0f / 256.0f);
			break;

		case MML_SET_LFOTARGET:
			trackParam.lfoTarget = arg1;
			break;

		case MML_SET_LFORANGE:
			trackParam.lfoParam.range = arg1;
			break;

		case MML_SET_LFODELAY:
			trackParam.lfoParam.delay = arg1 * 5;
			break;

		case MML_SET_SWEEPPITCH:
			trackParam.sweepPitch = arg1 / 64.0f;
			break;

		case MML_SET_ATTACK:
			trackParam.attack = arg1;
			break;

		case MML_SET_DECAY:
			trackParam.decay = arg1;
			break;

		case MML_SET_SUSTAIN:
			trackParam.sustain = arg1;
			break;

		case MML_SET_RELEASE:
			trackParam.release = arg1;
			break;

		case MML_SET_ENV_HOLD:
			trackParam.envHold = arg1 & 0xff;
			break;

		case MML_RESET_ADSR:
			trackParam.attack = 0xff;
			trackParam.decay = 0xff;
			trackParam.sustain = 0xff;
			trackParam.release = 0xff;
			trackParam.envHold = 0xff;

			break;

		case MML_SET_DAMPER:
			trackParam.damperFlag = static_cast<u8>(arg1) >= 64;
			break;

		case MML_SET_TIE:
			trackParam.tieFlag = arg1;
			track->ReleaseAllChannel(-1);
			track->FreeAllChannel();
			break;

		case MML_SET_MONOPHONIC:
			trackParam.monophonicFlag = arg1;

			if (trackParam.monophonicFlag)
			{
				track->ReleaseAllChannel(-1);
				track->FreeAllChannel();
			}

			break;

		case MML_SET_PORTAMENTO:
			trackParam.portaKey = arg1 + trackParam.transpose;
			trackParam.portaFlag = true;
			break;

		case MML_SET_PORTASPEED:
			trackParam.portaFlag = arg1 != 0;
			break;

		case MML_SET_LPFFREQ:
			trackParam.lpfFreq = (arg1 - 64) / 64.0f;
			break;

		case MML_SET_BIQUAD_TYPE:
			trackParam.biquadType = arg1;
			break;

		case MML_SET_BIQUAD_VALUE:
			trackParam.biquadValue = arg1 / 127.0f;
			break;

		case MML_SET_FXSEND_A:
			trackParam.fxSend[AUX_A] = arg1;
			break;

		case MML_SET_FXSEND_B:
			trackParam.fxSend[AUX_B] = arg1;
			break;

		case MML_SET_FXSEND_C:
			trackParam.fxSend[AUX_C] = arg1;
			break;

		case MML_SET_MAINSEND:
			trackParam.mainSend = arg1;
			break;

		case MML_PRINTVAR:
			if (mPrintVarEnabledFlag)
			{
				s16 const volatile * const varPtr =
					GetVariablePtr(player, track, arg1);

				OSReport("#%08x[%d]: printvar %sVAR_%d(%d) = %d\n", player,
				         track->GetPlayerTrackNo(),
				         arg1 >= SeqPlayer::VARIABLE_NUM         ? "T"
				         : arg1 >= SeqPlayer::LOCAL_VARIABLE_NUM ? "G"
				                                                 : "",
				         arg1 >= SeqPlayer::VARIABLE_NUM
				             ? arg1 - SeqPlayer::VARIABLE_NUM
				         : arg1 >= SeqPlayer::LOCAL_VARIABLE_NUM
				             ? arg1 - SeqPlayer::LOCAL_VARIABLE_NUM
				             : arg1,
				         arg1, *varPtr);
			}

			break;

		case MML_OPENTRACK:
		{
			SeqTrack *newTrack = player->GetPlayerTrack(arg1);

			if (!newTrack)
			{
				NW4RWarningMessage_Line(
					644,
					"nw4r::snd::MmlParser: opentrack for not allocated track");
				break;
			}

			if (newTrack == track)
			{
				NW4RWarningMessage_Line(
					649,
					"nw4r::snd::MmlParser: opentrack for self track");
				break;
			}

			newTrack->Close();
			newTrack->SetSeqData(trackParam.baseAddr, arg2);
			newTrack->Open();
		}
			break;

		case MML_JUMP:
			trackParam.currentAddr = trackParam.baseAddr + arg1;
			break;

		case MML_CALL:
		{
			if (trackParam.callStackDepth >= CALL_STACK_MAX_DEPTH)
			{
				NW4RWarningMessage_Line(665,
				                        "nw4r::snd::MmlParser: cannot \'call\' "
				                        "because already too deep");
				break;
			}

			SeqTrack::CallStack *callStack =
				&trackParam.callStack[trackParam.callStackDepth];

			callStack->address = trackParam.currentAddr;
			callStack->loopFlag = false;

			trackParam.callStackDepth++;
			trackParam.currentAddr = trackParam.baseAddr + arg1;
			break;
		}

		case MML_RET:
		{
			SeqTrack::CallStack *callStack = nullptr;

			while (trackParam.callStackDepth)
			{
				trackParam.callStackDepth--;

				if (!trackParam.callStack[trackParam.callStackDepth].loopFlag)
				{
					callStack = &trackParam.callStack[trackParam.callStackDepth];
					break;
				}
			}

			if (!callStack)
			{
				NW4RWarningMessage_Line(
					688,
					"nw4r::snd::MmlParser: unmatched sequence command \'ret\'");

				break;
			}

			trackParam.currentAddr = callStack->address;
		}
			break;

		case MML_LOOP_START:
		{
			if (trackParam.callStackDepth >= CALL_STACK_MAX_DEPTH)
			{
				NW4RWarningMessage_Line(
					698, "nw4r::snd::MmlParser: cannot \'loop_start\' because "
				         "already too deep");

				break;
			}

			SeqTrack::CallStack *callStack =
				&trackParam.callStack[trackParam.callStackDepth];

			callStack->address = trackParam.currentAddr;
			callStack->loopCount = arg1;
			callStack->loopFlag = true;

			trackParam.callStackDepth++;
		}
			break;

		case MML_LOOP_END:
		{
			if (trackParam.callStackDepth == 0)
			{
				NW4RWarningMessage_Line(713, "nw4r::snd::MmlParser: unmatched "
				                             "sequence command \'loop_end\'");
				break;
			}

			SeqTrack::CallStack *callStack =
				&trackParam.callStack[trackParam.callStackDepth - 1];

			if (!callStack->loopFlag)
			{
				NW4RWarningMessage_Line(719, "nw4r::snd::MmlParser: unmatched "
				                             "sequence command \'loop_end\'");
				break;
			}

			u8 count = callStack->loopCount;

			if (count != 0 && --count == 0)
			{
				trackParam.callStackDepth--;
			}
			else
			{
				callStack->loopCount = count;

				trackParam.currentAddr = callStack->address;
			}
		}
			break;
		}
	}
	else if (command <= MML_EX_CMD_MAX)
	{
		byte4_t cmd = command >> 8;
		byte4_t cmdex = command & 0xff;

		NW4RAssert_Line(742, cmd == MML_EX_COMMAND);

		volatile s16 *varPtr = nullptr;

		if ((cmdex & 0xf0) == MML_EX_ARITHMETIC
		    || (cmdex & 0xf0) == MML_EX_LOGIC)
		{
			varPtr = GetVariablePtr(player, track, arg1);
			if (!varPtr)
				return;
		}

		switch (cmdex)
		{
		case MML_EX_SET:
			*varPtr = arg2;
			break;

		case MML_EX_APL:
			*varPtr += arg2;
			break;

		case MML_EX_AMI:
			*varPtr -= arg2;
			break;

		case MML_EX_AMU:
			*varPtr *= arg2;
			break;

		case MML_EX_ADV:
			if (arg2 != 0)
				*varPtr /= arg2;

			break;

		case MML_EX_ALS:
			if (arg2 >= 0)
				*varPtr <<= arg2;
			else
				*varPtr >>= -arg2;

			break;

		case MML_EX_RND:
		{
			bool minus_flag = false;

			if (arg2 < 0)
			{
				minus_flag = true;
				arg2 = static_cast<s16>(-arg2);
			}

			s32 rand = Util::CalcRandom();
			rand *= arg2 + 1;
			rand >>= 16;

			if (minus_flag)
				rand = -rand;

			*varPtr = rand;
			break;
		}

		case MML_EX_AAD:
			*varPtr &= arg2;
			break;

		case MML_EX_AOR:
			*varPtr |= arg2;
			break;

		case MML_EX_AER:
			*varPtr ^= arg2;
			break;

		case MML_EX_ACO:
			*varPtr = ~static_cast<u16>(arg2);
			break;

		case MML_EX_AMD:
			if (arg2 != 0)
				*varPtr %= arg2;

			break;

		case MML_EX_EQ:
			trackParam.cmpFlag = *varPtr == arg2;
			break;

		case MML_EX_GE:
			trackParam.cmpFlag = *varPtr >= arg2;
			break;

		case MML_EX_GT:
			trackParam.cmpFlag = *varPtr > arg2;
			break;

		case MML_EX_LE:
			trackParam.cmpFlag = *varPtr <= arg2;
			break;

		case MML_EX_LT:
			trackParam.cmpFlag = *varPtr < arg2;
			break;

		case MML_EX_NE:
			trackParam.cmpFlag = *varPtr != arg2;
			break;

		case MML_EX_USERPROC:
			player->CallSeqUserprocCallback(arg1, track);
			break;
		}
	}
}

Channel *MmlParser::NoteOnCommandProc(MmlSeqTrack *track, int key, int velocity,
                                      s32 length, bool tieFlag) const
{
	return track->NoteOn(key, velocity, length, tieFlag);
}

byte2_t MmlParser::Read16(byte_t const **ptr) const
{
	byte2_t ret = ReadByte(ptr);

	ret <<= 8;
	ret |= ReadByte(ptr);

	return ret;
}

byte4_t MmlParser::Read24(byte_t const **ptr) const
{
	byte4_t ret = ReadByte(ptr);

	ret <<= 8;
	ret |= ReadByte(ptr);

	ret <<= 8;
	ret |= ReadByte(ptr);

	return ret;
}

s32 MmlParser::ReadVar(byte_t const **ptr) const
{
	u32 ret = 0;
	u8 b;

	for (int i = 0; true; i++)
	{
		NW4RAssert_Line(940, i < 4);

		b = ReadByte(ptr);
		ret <<= 7;
		ret |= b & 0x7f;

		if (!(b & 0x80))
			break;
	}

	return ret;
}

s32 MmlParser::ReadArg(byte_t const **ptr, SeqPlayer *player, SeqTrack *track,
                       SeqArgType argType) const
{
	s32 var;

	switch (argType)
	{
	case SEQ_ARG_U8:
		var = ReadByte(ptr);
		break;

	case SEQ_ARG_S16:
		var = Read16(ptr);
		break;

	case SEQ_ARG_VMIDI:
		var = ReadVar(ptr);
		break;

	case SEQ_ARG_VARIABLE:
	{
		u8 varNo = ReadByte(ptr);

		s16 const volatile *varPtr = GetVariablePtr(player, track, varNo);

		// ERRATUM: if varPtr is not valid then ReadArg returns garbage
		if (varPtr)
			var = *varPtr;
	}
		break;

	case SEQ_ARG_RANDOM:
	{
		s32 rand;

		s16 min = Read16(ptr);
		s16 max = Read16(ptr);

		rand = Util::CalcRandom();
		rand *= max - min + 1;
		rand >>= 16;
		rand += min;

		var = rand;
	}
		break;
	}

	return var;
}

s16 volatile *MmlParser::GetVariablePtr(SeqPlayer *player, SeqTrack *track,
                                        int varNo) const
{
	NW4RAssertHeaderClampedLRValue_Line(1014, varNo, 0, 48);

	if (varNo < SeqPlayer::VARIABLE_NUM)
		return player->GetVariablePtr(varNo);

	if (varNo < SeqPlayer::VARIABLE_NUM + SeqTrack::VARIABLE_NUM)
		return track->GetVariablePtr(varNo - SeqPlayer::VARIABLE_NUM);

	return nullptr;
}

u32 MmlParser::ParseAllocTrack(void const *baseAddress, u32 seqOffset,
                               byte4_t *allocTrack)
{
	NW4RAssertPointerNonnull_Line(1051, baseAddress);
	NW4RAssertPointerNonnull_Line(1052, allocTrack);

	byte_t const *ptr = static_cast<byte_t const *>(
		ut::AddOffsetToPtr<u32>(baseAddress, seqOffset));

	if (*ptr != MML_ALLOCTRACK)
	{
		*allocTrack = 1;
		return seqOffset;
	}
	else
	{
		u32 tracks = *++ptr;

		tracks <<= 8;
		tracks |= *++ptr;

		*allocTrack = tracks;
		return seqOffset + 3;
	}
}

}}} // namespace nw4r::snd::detail
