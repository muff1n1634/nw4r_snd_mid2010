#ifndef NW4R_SND_STRM_PLAYER_H
#define NW4R_SND_STRM_PLAYER_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "BasicPlayer.h"
#include "global.h"
#include "InstancePool.h"
#include "Task.h"
#include "SoundThread.h"
#include "StrmChannel.h"
#include "StrmFile.h"
#include "Voice.h"

#include "../ut/LinkList.h"

#if 0
#include <revolution/OS/OSMutex.h>
#else
#include <context_rvl.h>
#endif

/*******************************************************************************
 * classes and types
 */

// forward declarations
namespace nw4r { namespace ut { class FileStream; }}

namespace nw4r { namespace snd { namespace detail
{
	class StrmPlayer : public BasicPlayer, public SoundThread::PlayerCallback
	{
	// enums
	public:
		enum StartOffsetType
		{
			START_OFFSET_TYPE_SAMPLE,
			START_OFFSET_TYPE_MILLISEC,
		};

		enum SetupResult
		{
			SETUP_SUCCESS,

			SETUP_ERR_CANNOT_ALLOCATE_BUFFER,
			SETUP_ERR_UNKNOWN,
		};

	// nested types
	public:
		class StrmHeaderLoadTask : public Task
		{
		// methods
		public:
			// cdtors
			StrmHeaderLoadTask();

			// virtual function ordering
			// vtable Task
			virtual void Execute();
			virtual void Cancel();
			virtual void OnCancel();

		// members
		private:
			/* base Task */						// size 0x10, offset 0x00
		public:
			StrmPlayer		*player;			// size 0x04, offset 0x10
			ut::FileStream	*fileStream;		// size 0x04, offset 0x14
			StartOffsetType	startOffsetType;	// size 0x04, offset 0x18
			s32				startOffset;		// size 0x04, offset 0x1c
		}; // size 0x20

		class StrmDataLoadTask : public Task
		{
		// typedefs
		public:
			typedef ut::LinkList<StrmDataLoadTask, 44> LinkList;

		// methods
		public:
			// cdtors
			StrmDataLoadTask();

			// virtual function ordering
			// vtable Task
			virtual void Execute();
			virtual void Cancel();
			virtual void OnCancel();

		// members
		private:
			/* base Task */								// size 0x10, offset 0x00
		public:
			StrmPlayer			*mStrmPlayer;			// size 0x04, offset 0x10
			ut::FileStream		*fileStream;			// size 0x04, offset 0x14
			u32					mSize;					// size 0x04, offset 0x18
			s32					mOffset;				// size 0x04, offset 0x1c
			u32					mBlockSize;				// size 0x04, offset 0x20
			s32					mBufferBlockIndex;		// size 0x04, offset 0x24
			bool				mNeedUpdateAdpcmLoop;	// size 0x01, offset 0x28
			/* 3 bytes padding */
			ut::LinkListNode	link;					// size 0x08, offset 0x2c
		}; // size 0x34

		struct StrmTrack
		{
			bool							mActiveFlag;	// size 0x01, offset 0x00
			Voice							*mVoice;		// size 0x04, offset 0x04
			StrmFileReader::StrmTrackInfo	mTrackInfo;		// size 0x28, offset 0x08
			f32								mVolume;		// size 0x04, offset 0x30
			f32								mPan;			// size 0x04, offset 0x34
		}; // size 0x38

	// methods
	public:
		StrmPlayer();
		virtual ~StrmPlayer();

		// virtual function ordering
		// vtable BasicPlayer
		virtual bool Start();
		virtual void Stop();
		virtual void Pause(bool flag);
		virtual bool IsActive() const { return mActiveFlag; }
		virtual bool IsStarted() const { return mStartedFlag; }
		virtual bool IsPause() const { return mPauseFlag; }

		// vtable PlayerCallback
		virtual void OnUpdateFrameSoundThread() { Update(); }
		virtual void OnUpdateVoiceSoundThread() { UpdateBuffer(); }
		virtual void OnShutdownSoundThread() { Stop(); }

		// methods
		void Update();
		void UpdateBuffer();
		SetupResult Setup(StrmBufferPool *bufferPool, int allocChannelCount,
		                  u16 allocTrackFlag, int voiceOutCount);
		void Shutdown();

		void InitParam();

		bool AllocStrmBuffers();
		void FreeStrmBuffers();
		void FreeVoices();

		bool Prepare(ut::FileStream *fileStream,
		             StartOffsetType startOffsetType, int startOffset);
		bool CalcStartOffset(s32 *startBlockIndex, u32 *startBlockOffset,
		                     s32 *loopCount);
		void UpdatePauseStatus();
		bool LoadHeader(ut::FileStream *, StartOffsetType, int);
		void SetTaskErrorFlag() { mTaskErrorFlag = true; }
		void SetTaskCancelFlag() { mTaskCancelFlag = true; }

		bool LoadStreamData(ut::FileStream *, int, unsigned long, unsigned long,
		                    int, bool);

		StrmTrack *GetPlayerTrack(int trackNo);
		void SetTrackVolume(byte4_t trackBitFlag, f32 volume);
		int CalcLoadingBufferBlockCount() const;
		void SetLoopEndToZeroBuffer(int endBufferBlockIndex);
		StrmChannel *GetTrackChannel(const StrmTrack &track, int channelIndex);
		void UpdatePlayingBlockIndex();
		void UpdateLoadingBlockIndex();
		void UpdateDataLoopAddress(s32 endBlockBufferIndex);
		void UpdateLoopAddress(u32 loopStartSamples, u32 loopEndSamples);
		bool IsAdpcm() const
		{
			return mStrmInfo.sampleFormat == SAMPLE_FORMAT_DSP_ADPCM;
		}
		bool CheckDiskDriveError() const;
		bool AllocVoices(int voiceOutCount);
		void UpdateVoiceParams(StrmTrack *track);
		bool SetupPlayer();
		void SetAdpcmLoopContext(int channelNum, u16 *predScale);
		bool IsPrepared() const { return mPreparedFlag; }

		static void VoiceCallbackFunc(Voice *voice,
		                              Voice::VoiceCallbackStatus status,
		                              void *arg);
		static u32 GetSampleByByte(u32 byte, SampleFormat format);

	// static members
	public:
		static int const BUFFER_BLOCK_COUNT_MAX = 32;
		static u32 const LOAD_BUFFER_SIZE = 0x4000;
		static int const STRM_CHANNEL_NUM = 16;
		static int const STRM_TRACK_NUM = 8;

	private:
		static int const DATA_BLOCK_COUNT_MIN = 4;
		static int const DATA_BLOCK_COUNT_MAX = 32;
		static int const DATA_BLOCK_SIZE_MAX = 0x2000;

		static bool sStaticInitFlag;
		static OSMutex sLoadBufferMutex;
		static byte_t sLoadBuffer[LOAD_BUFFER_SIZE];

	// members
	private:
		/* base BasicPlayer */															// size 0x0a4, offset 0x000
		/* base PlayerCallback */														// size 0x00c, offset 0x0a4
		StrmFileReader::StrmInfo		mStrmInfo;										// size 0x040, offset 0x0b0
		bool							mSetupFlag;										// size 0x001, offset 0x0f0
		bool volatile					mActiveFlag;									// size 0x001, offset 0x0f1
		bool							mStartedFlag;									// size 0x001, offset 0x0f2
		bool							mPreparedFlag;									// size 0x001, offset 0x0f3
		bool							mTaskErrorFlag;									// size 0x001, offset 0x0f4
		bool							mTaskCancelFlag;								// size 0x001, offset 0x0f5
		bool							mLoadingDelayFlag;								// size 0x001, offset 0x0f6
		bool							mPauseFlag;										// size 0x001, offset 0x0f7
		bool							mPauseStatus;									// size 0x001, offset 0x0f8
		bool							mLoadWaitFlag;									// size 0x001, offset 0x0f9
		bool							mNoRealtimeLoadFlag;							// size 0x001, offset 0x0fa
		bool							mSkipUpdateAdpcmLoop;							// size 0x001, offset 0x0fb
		bool							mValidAdpcmLoop;								// size 0x001, offset 0x0fc
		bool							mPlayFinishFlag;								// size 0x001, offset 0x0fd
		bool							mLoadFinishFlag;								// size 0x001, offset 0x0fe
		bool							mAllocStrmBufferFlag;							// size 0x001, offset 0x0ff
		s32								mLoopCounter;									// size 0x004, offset 0x100
		int								mPrepareCounter;								// size 0x004, offset 0x104
		int								mChangeNumBlocks;								// size 0x004, offset 0x108
		int								mDataBlockSize;									// size 0x004, offset 0x10c
		int								mBufferBlockCount;								// size 0x004, offset 0x110
		int								mBufferBlockCountBase;							// size 0x004, offset 0x114
		int								mLoadingBufferBlockCount;						// size 0x004, offset 0x118
		int								mLoadingBufferBlockIndex;						// size 0x004, offset 0x11c
		int								mLoadingDataBlockIndex;							// size 0x004, offset 0x120
		int								mPlayingBufferBlockCount;						// size 0x004, offset 0x124
		int								mPlayingBufferBlockIndex;						// size 0x004, offset 0x128
		int								mPlayingDataBlockIndex;							// size 0x004, offset 0x12c
		int								mLoopStartBlockIndex;							// size 0x004, offset 0x130
		int								mLastBlockIndex;								// size 0x004, offset 0x134
		int								mLoadWaitCount;									// size 0x004, offset 0x138
		StartOffsetType					mStartOffsetType;								// size 0x004, offset 0x13c
		int								mStartOffset;									// size 0x004, offset 0x140
		StrmHeaderLoadTask				mStrmHeaderLoadTask;							// size 0x020, offset 0x144
		StrmDataLoadTask::LinkList		mStrmDataLoadTaskList;							// size 0x00c, offset 0x164
		InstancePool<StrmDataLoadTask>	mStrmDataLoadTaskPool;							// size 0x004, offset 0x170
		StrmDataLoadTask				mStrmDataLoadTaskArea[BUFFER_BLOCK_COUNT_MAX];	// size 0x680, offset 0x174
		StrmBufferPool					*mBufferPool;									// size 0x004, offset 0x7f4
		ut::FileStream					*mFileStream;									// size 0x004, offset 0x7f8
		s32								mTrackCount;									// size 0x004, offset 0x7fc
		s32								mChannelCount;									// size 0x004, offset 0x800
		s32								mVoiceOutCount;									// size 0x004, offset 0x804
		StrmChannel						mChannels[STRM_CHANNEL_NUM];					// size 0x340, offset 0x808
		StrmTrack						mTracks[STRM_TRACK_NUM];						// size 0x1c0, offset 0xb48
	}; // size 0xd08
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_STRM_PLAYER_H
