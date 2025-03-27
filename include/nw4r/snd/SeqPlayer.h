#ifndef NW4R_SND_SEQ_PLAYER_H
#define NW4R_SND_SEQ_PLAYER_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "BasicPlayer.h"
#include "DisposeCallbackManager.h" // DisposeCallback
#include "SoundThread.h"

#include "../ut/lock.h"

/*******************************************************************************
 * types
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail { class Channel; }}}
namespace nw4r { namespace snd { namespace detail { class NoteOnCallback; }}}
namespace nw4r { namespace snd { namespace detail { struct NoteOnInfo; }}}
namespace nw4r { namespace snd { namespace detail { class SeqTrack; }}}
namespace nw4r { namespace snd { namespace detail { class SeqTrackAllocator; }}}

namespace nw4r { namespace snd
{
	struct SeqUserprocCallbackParam
	{
		s16 volatile	*localVariable;		// size 0x04, offset 0x00
		s16 volatile	*globalVariable;	// size 0x04, offset 0x04
		s16 volatile	*trackVariable;		// size 0x04, offset 0x08
		bool			cmpFlag;			// size 0x01, offset 0x0C
		/* 3 bytes padding */
	}; // size 0x10
}} // namespace nw4r::snd

/*******************************************************************************
 * classes
 */

namespace nw4r { namespace snd { namespace detail
{
	class SeqPlayer : public BasicPlayer,
	                  public DisposeCallback,
	                  public SoundThread::PlayerCallback
	{
	// enums
	public:
		enum OffsetType
		{
			OFFSET_TYPE_TICK,
			OFFSET_TYPE_MILLISEC,
		};

		enum SetupResult
		{
			SETUP_SUCCESS,

			SETUP_ERR_CANNOT_ALLOCATE_TRACK,
			SETUP_ERR_UNKNOWN,
		};

	// nested types
	public:
		struct ParserPlayerParam
		{
			u8				volume;		// size 0x01, offset 0x00
			u8				priority;	// size 0x01, offset 0x01
			u8				timebase;	// size 0x01, offset 0x02
			/* 1 byte padding */
			u16				tempo;		// size 0x02, offset 0x04
			/* 2 bytes padding */
			NoteOnCallback	*callback;	// size 0x04, offset 0x08
		}; // size 0x0c

	// typedefs
	public:
		typedef void SeqUserprocCallback(u16, SeqUserprocCallbackParam *,
		                                 void *);

		// methods
	public:
		// cdtors
		SeqPlayer();
		virtual ~SeqPlayer();

		// virtual function ordering
		// vtable BasicPlayer
		virtual bool Start();
		virtual void Stop();
		virtual void Pause(bool flag);
		virtual bool IsActive() const { return mActiveFlag; }
		virtual bool IsStarted() const { return mStartedFlag; }
		virtual bool IsPause() const { return mPauseFlag; }
		virtual void InvalidateData(void const *start, void const *end);
		virtual void InvalidateWaveData(void const *, void const *) {}

		virtual void ChannelCallback(Channel *) {}
		virtual void OnUpdateFrameSoundThread() { Update(); }
		virtual void OnShutdownSoundThread() { Stop(); }

		// methods
		void InitParam(int voices, NoteOnCallback *pCallback);

		SetupResult Setup(SeqTrackAllocator *allocator, u32 allocTrackFlags,
		                  int voices, NoteOnCallback *callback);
		void SetSeqData(void const *seqBase, s32 seqOffset);

		void Skip(OffsetType type, int offset);

		void SetTempoRatio(f32 tempo);
		void SetChannelPriority(int priority);
		void SetReleasePriorityFix(bool flag);

		void SetLocalVariable(int i, s16 value);
		static void SetGlobalVariable(int i, s16 value);

		void SetTrackVolume(u32 trackFlags, f32 volume);
		void SetTrackPitch(u32 trackFlags, f32 pitch);

		SeqTrack *GetPlayerTrack(int i);
		s16 volatile *GetVariablePtr(int i);
		void Update();

		Channel *NoteOn(int bankNo, NoteOnInfo const &info);

		template <typename T>
		void SetTrackParam(u32 trackFlags, void (SeqTrack::*setter)(T), T param)
		{
			ut::AutoInterruptLock lock;

			for (int i = 0; i < TRACK_NUM && trackFlags; trackFlags >>= 1, i++)
			{
				if (trackFlags & 1)
				{
					SeqTrack *track = GetPlayerTrack(i);

					if (track)
						(track->*setter)(param);
				}
			}
		}

		bool IsReleasePriorityFix() const { return mReleasePriorityFixFlag; }
		f32 GetPanRange() const { return mPanRange; }
		f32 GetBaseTempo() const
		{
			return mTempoRatio
			     * (mParserParam.timebase * mParserParam.tempo) / 60000.0f;
		}
		int GetVoiceOutCount() const { return mVoiceOutCount; }

		ParserPlayerParam &GetParserPlayerParam() { return mParserParam; }
		void CallSeqUserprocCallback(u16 procId, SeqTrack *track);

		void Shutdown();

		void SetSeqUserprocCallback(SeqUserprocCallback *callback, void *arg);

		static void InitSeqPlayer();

	private:
		void CloseTrack(int i);
		void SetPlayerTrack(int i, SeqTrack *track);

		void FinishPlayer();
		void UpdateChannelParam();
		int ParseNextTick(bool doNoteOn);

		void UpdateTick(int msec);
		void SkipTick();

	// static members
	public:
		static int const LOCAL_VARIABLE_NUM = 16;
		static int const GLOBAL_VARIABLE_NUM = 16;
		static int const VARIABLE_NUM =
			LOCAL_VARIABLE_NUM + GLOBAL_VARIABLE_NUM;
		static int const TRACK_NUM = 16;

	// members
	private:
		/* base BasicPlayer */									// size 0x0a4, offset 0x000
		/* base DisposeCallback */								// size 0x00c, offset 0x0a4
		/* base PlayerCallback */								// size 0x00c, offset 0x0b0
		bool				mActiveFlag;						// size 0x001, offset 0x0ec
		bool				mStartedFlag;						// size 0x001, offset 0x0ed
		bool				mPauseFlag;							// size 0x001, offset 0x0ee
		bool				mReleasePriorityFixFlag;			// size 0x001, offset 0x0ef
		f32					mPanRange;							// size 0x004, offset 0x0f0
		f32					mTempoRatio;						// size 0x004, offset 0x0f4
		f32					mTickFraction;						// size 0x004, offset 0x0f8
		u32					mSkipTickCounter;					// size 0x004, offset 0x0fc
		f32					mSkipTimeCounter;					// size 0x004, offset 0x100
		s32					mVoiceOutCount;						// size 0x004, offset 0x104
		ParserPlayerParam	mParserParam;						// size 0x00c, offset 0x108
		SeqTrackAllocator	*mSeqTrackAllocator;				// size 0x004, offset 0x114
		SeqUserprocCallback	*mSeqUserprocCallback;				// size 0x004, offset 0x118
		void				*mSeqUserprocCallbackArg;			// size 0x004, offset 0x11c
		SeqTrack			*mTracks[TRACK_NUM];				// size 0x040, offset 0x120
		volatile s16		mLocalVariable[LOCAL_VARIABLE_NUM];	// size 0x020, offset 0x160
		u32					mTickCounter;						// size 0x004, offset 0x180

	// static members
	private:
		static volatile s16 mGlobalVariable[GLOBAL_VARIABLE_NUM];

		static int const DEFAULT_TEMPO = 120;
		static int const DEFAULT_TIMEBASE = 48;
		static int const DEFAULT_VOLUME = 127;
		static int const DEFAULT_PRIORITY = 64;
		static int const DEFAULT_VARIABLE_VALUE = -1;

		static int const MAX_SKIP_TICK_PER_FRAME = 768;
	}; // size 0x154 // TODO
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_SEQ_PLAYER_H
