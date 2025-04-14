#ifndef NW4R_SND_LFO_H
#define NW4R_SND_LFO_H

/*******************************************************************************
 * headers
 */

#include <types.h>

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	struct LfoParam
	{
	// methods
	public:
		// cdtors
		LfoParam() { Init(); }

		// methods
		void Init();

	// members
	public:
		f32		depth;	// size 0x04, offset 0x00
		f32		speed;	// size 0x04, offset 0x04
		u32		delay;	// size 0x04, offset 0x08
		u8		range;	// size 0x01, offset 0x0c
		byte_t	padding[3];
	}; // size 0x10

	class Lfo
	{
	// methods
	public:
		// cdtors
		Lfo(): mDelayCounter(0), mCounter(0.0f) {}

		// gethods
		LfoParam &GetParam() { return mParam; }
		f32 GetValue() const;

		// sethods
		void SetParam(const LfoParam &param) { mParam = param; }

		// methods
		void Reset();
		void Update(int msec);

	private:
		static s8 GetSinIdx(int i);

	// static members
	private:
		static const int TABLE_SIZE = 32;

	// members
	private:
		LfoParam	mParam;			// size 0x10, offset 0x00
		u32			mDelayCounter;	// size 0x04, offset 0x10
		f32			mCounter;		// size 0x04, offset 0x14
	}; // size 0x18
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_LFO_H
