#ifndef NW4R_SND_GLOBAL_H
#define NW4R_SND_GLOBAL_H

/*******************************************************************************
 * headers
 */

#include <types.h>

/*******************************************************************************
 * types
 */

namespace nw4r { namespace snd
{
	namespace detail
	{
		struct VoiceOutParam
		{
		// methods
		public:
			// cdtors
			VoiceOutParam() :
				volume		(1.0f),
				pitch		(1.0f),
				pan			(0.0f),
				surroundPan	(0.0f),
				fxSend		(0.0f),
				lpf			(0.0f)
				{}

		// members
		public:
			f32 volume;			// size 0x04, offset 0x00
			f32 pitch;			// size 0x04, offset 0x04
			f32 pan;			// size 0x04, offset 0x08
			f32 surroundPan;	// size 0x04, offset 0x0c
			f32 fxSend;			// size 0x04, offset 0x10
			f32 lpf;			// size 0x04, offset 0x14
		}; // size 0x18

		struct AdpcmParam
		{
			u16	coef[16];	// size 0x20, offset 0x00
			u16	gain;		// size 0x02, offset 0x20
			u16	pred_scale;	// size 0x02, offset 0x22
			u16	yn1;		// size 0x02, offset 0x24
			u16	yn2;		// size 0x02, offset 0x26
		}; // size 0x28

		struct AdpcmLoopParam
		{
			u16	loop_pred_scale;	// size 0x02, offset 0x00
			u16	loop_yn1;			// size 0x02, offset 0x02
			u16	loop_yn2;			// size 0x02, offset 0x04
		}; // size 0x06
	} // namespace detail

	static const int CHANNEL_MIN = 1;
	static const int CHANNEL_MAX = 2;

	static const int REMOTE_FILTER_MAX = 127;

	// 12 notes each with 256 microtones
	static const int SEMITONE_MAX = 12;
	static const int MICROTONE_MAX = 256;

	// Volume in range [-90.4db, 6.0db]
	static const f32 VOLUME_MIN_DB = -90.4f;
	static const f32 VOLUME_MAX_DB = 6.0f;
	static const f32 VOLUME_RANGE_DB = VOLUME_MAX_DB - VOLUME_MIN_DB;
	// static_cast required for mwcc to be able to use it constexpr
	static const int VOLUME_RANGE_MB = static_cast<int>(10 * VOLUME_RANGE_DB);

	enum AuxBus
	{
		AUX_A,
		AUX_B,
		AUX_C,

		AUX_BUS_NUM
	};

	enum OutputLineFlag
	{
		OUTPUT_LINE_MAIN		= 1 << 0,
		OUTPUT_LINE_REMOTE_N	= 1 << 1,
	};

	enum OutputMode
	{
		OUTPUT_MODE_STEREO,
		OUTPUT_MODE_SURROUND,
		OUTPUT_MODE_DPL2,
		OUTPUT_MODE_MONO,
	};

	enum PanMode
	{
		PAN_MODE_DUAL,
		PAN_MODE_BALANCE,
	};

	enum PanCurve
	{
		PAN_CURVE_SQRT,
		PAN_CURVE_SQRT_0DB,
		PAN_CURVE_SQRT_0DB_CLAMP,

		PAN_CURVE_SINCOS,
		PAN_CURVE_SINCOS_0DB,
		PAN_CURVE_SINCOS_0DB_CLAMP,

		PAN_CURVE_LINEAR,
		PAN_CURVE_LINEAR_0DB,
		PAN_CURVE_LINEAR_0DB_CLAMP,
	};

	enum SampleFormat
	{
		SAMPLE_FORMAT_PCM_S32,
		SAMPLE_FORMAT_PCM_S16,
		SAMPLE_FORMAT_PCM_S8,
		SAMPLE_FORMAT_DSP_ADPCM,
	};

	struct ChannelParam
	{
		void					*dataAddr;		// size 0x04, offset 0x00
		detail::AdpcmParam		adpcmParam;		// size 0x28, offset 0x04
		detail::AdpcmLoopParam	adpcmLoopParam;	// size 0x06, offset 0x2c
		/* 2 bytes padding */
	}; // size 0x34
}} // namespace nw4r::snd

#endif // NW4R_SND_GLOBAL_H
