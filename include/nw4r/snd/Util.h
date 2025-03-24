#ifndef NW4R_SND_UTIL_H
#define NW4R_SND_UTIL_H

/*******************************************************************************
 * headers
 */

#include <macros.h>
#include <types.h>

#include "global.h"

#include "nw4r/NW4RAssert.hpp"

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail { namespace Util
{
	enum RefType
	{
		REFTYPE_ADDRESS,
		REFTYPE_OFFSET,
	};

	template <typename T0, typename T1 = void, typename T2 = void,
	          typename T3 = void>
	struct DataRef
	{
		u8	refType;	// size 0x01, offset 0x00
		u8	dataType;	// size 0x01, offset 0x01
		u16	reserved;	// size 0x02, offset 0x02
		u32	value;		// size 0x04, offset 0x04
	}; // size 0x08

	template <typename T>
	struct Table
	{
		u32	count;		// size 0x04, offset 0x00
		T	items[1];	// size T,    offset 0x04
	}; // size 0x04 + T

	struct PanInfo
	{
		PanCurve	curve;			// size 0x04, offset 0x00
		bool		centerZeroFlag;	// size 0x01, offset 0x04
		bool		zeroClampFlag;	// size 0x01, offset 0x05
		/* 2 bytes padding */
	}; // size 0x08

	const void *GetDataRefAddressImpl(RefType refType, u32 value,
	                                  const void *baseAddress);

// The only way these asserts happen one line after the other is with macros
#define DEF_GET_DATA_REF_ADDRESS_(index_, line_)								\
	template <typename T0, typename T1, typename T2, typename T3>				\
	inline const CONCAT(T, index_) *CONCAT(GetDataRefAddress, index_)(			\
		const Util::DataRef<T0, T1, T2, T3> &ref, const void *baseAddress)		\
	{																			\
		/* specifically not the header variant */								\
		NW4RAssert_Line(line_, ref.dataType == index_);							\
																				\
		return static_cast<const CONCAT(T, index_) *>(GetDataRefAddressImpl(	\
			static_cast<RefType>(ref.refType), ref.value, baseAddress));		\
	}																			\
	static_assert(true, "") // swallow semicolon

	DEF_GET_DATA_REF_ADDRESS_(0, 141);
	DEF_GET_DATA_REF_ADDRESS_(1, 142);
	DEF_GET_DATA_REF_ADDRESS_(2, 143);
	DEF_GET_DATA_REF_ADDRESS_(3, 144);

#undef DEF_GET_DATA_REF_ADDRESS_

	f32 CalcPitchRatio(int pitch);
	f32 CalcVolumeRatio(f32 dB);
	f32 CalcPanRatio(f32 pan, const PanInfo &info);
	f32 CalcSurroundPanRatio(f32 pan, const PanInfo &info);

	u16 CalcLpfFreq(f32 scale);
	u16 CalcRandom();
	void GetRemoteFilterCoefs(int filter, u16 *b0, u16 *b1, u16 *b2, u16 *a1,
	                          u16 *a2);

	inline byte2_t ReadBigEndian(byte2_t x)
	{
		return x;
	}

	inline byte4_t ReadBigEndian(byte4_t x)
	{
		return x;
	}
}}}} // namespace nw4r::snd::detail::Util

#endif // NW4R_SND_UTIL_H
