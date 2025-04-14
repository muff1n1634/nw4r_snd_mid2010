#ifndef NW4R_SND_ADPCM_H
#define NW4R_SND_ADPCM_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#if 0
#include <revolution/AX/AXVPB.h>
#else
#include <context_rvl.h>
#endif

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	s16 DecodeDspAdpcm(AXPBADPCM *adpcm, u8 bits);
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_ADPCM_H
