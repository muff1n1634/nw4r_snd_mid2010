#include "nw4r/snd/debug.h"

/*******************************************************************************
 * headers
 */

#include <types.h> // byte4_t

#include <nw4r/NW4RAssert.hpp>

/*******************************************************************************
 * local function declarations
 */

namespace nw4r { namespace snd { namespace
{
	static inline byte4_t GetWarningBitFlag(DebugWarningFlag warning)
	{
		byte4_t bitFlag = 0;

		switch (warning)
		{
		case DEBUG_WARNING_NOT_ENOUGH_INSTANCE:
			bitFlag = 0x1f; // 1 through 5
			break;

		case DEBUG_WARNING_NOT_ENOUGH_SEQSOUND:
			bitFlag = 1 << 0;
			break;

		case DEBUG_WARNING_NOT_ENOUGH_STRMSOUND:
			bitFlag = 1 << 1;
			break;

		case DEBUG_WARNING_NOT_ENOUGH_WAVESOUND:
			bitFlag = 1 << 2;
			break;

		case DEBUG_WARNING_NOT_ENOUGH_SEQTRACK:
			bitFlag = 1 << 3;
			break;

		case DEBUG_WARNING_NOT_ENOUGH_STRMCHANNEL:
			bitFlag = 1 << 4;
			break;
		}

		return bitFlag;
	}
}}} // namespace nw4r::snd::(unnamed)

/*******************************************************************************
 * variables
 */

namespace nw4r { namespace snd { namespace
{
	static byte4_t gWarningFlag = 0x1f; // 1 through 5
}}} // namespace nw4r::snd::(unnamed)

/*******************************************************************************
 * functions
 */

namespace nw4r { namespace snd { namespace detail {

bool Debug_GetWarningFlag(DebugWarningFlag warning)
{
	byte4_t bitFlag = GetWarningBitFlag(warning);

	return (bitFlag & gWarningFlag) == bitFlag;
}

DebugWarningFlag Debug_GetDebugWarningFlagFromSoundType(DebugSoundType type)
{
	switch (type)
	{
	case DEBUG_SOUND_TYPE_SEQSOUND:
		return DEBUG_WARNING_NOT_ENOUGH_SEQSOUND;

	case DEBUG_SOUND_TYPE_STRMSOUND:
		return DEBUG_WARNING_NOT_ENOUGH_STRMSOUND;

	case DEBUG_SOUND_TYPE_WAVESOUND:
		return DEBUG_WARNING_NOT_ENOUGH_WAVESOUND;

	default:
		NW4RAssert_Line(91, false);
		return DEBUG_WARNING_NOT_ENOUGH_SEQSOUND;
	}
}

const char *Debug_GetSoundTypeString(DebugSoundType type)
{
	switch (type)
	{
	case DEBUG_SOUND_TYPE_SEQSOUND:
		return "seq";

	case DEBUG_SOUND_TYPE_STRMSOUND:
		return "strm";

	case DEBUG_SOUND_TYPE_WAVESOUND:
		return "wave";

	default:
		NW4RAssert_Line(107, false);
		return "";
	}
}

}}} // namespace nw4r::snd::detail
