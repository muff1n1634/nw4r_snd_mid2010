#ifndef CONTEXT_RVL_H
#define CONTEXT_RVL_H

#include <macros.h>
#include <types.h>

#ifdef __cplusplus
	extern "C" {
#endif

/* Contains the context of the other Revolution SDK libraries that the WPAD
 * library needs to compile.
 *
 * Ideally, this file's usages should be replaced with the headers in your
 * project that declare these symbols.
 */

__attribute__((weak)) void OSReport(const char *msg, ...);
__attribute__((weak)) void OSPanic(const char *file, int line, const char *msg, ...);

#if !defined(NDEBUG)
# define OSAssertMessage_FileLine(file_, line_, expr_, ...)	\
	(void)((expr_) || (OSPanic(file_, line_, __VA_ARGS__), 0))
#else
# define OSAssertMessage_FileLine(file_, line_, expr_, ...)	\
	((void)0)
#endif

// defined in terms of OSAssertMessage_FileLine
#define OSAssertMessage_Line(line_, expr_, ...)	\
	OSAssertMessage_FileLine(__FILE__, line_, expr_, __VA_ARGS__)
#define OSAssert_Line(line_, expr_)	\
	OSAssertMessage_FileLine(__FILE__, line_, expr_, "Failed assertion " #expr_)

// OSError
#define OSError_FileLine(file_, line_, ...)	\
	OSPanic(file_, line_, __VA_ARGS__)

// defined in terms of OSError_FileLine
#define OSError_Line(line_, ...)	\
	OSError_FileLine(__FILE__, line_, __VA_ARGS__)

typedef s64 OSTime;
typedef u32 OSTick;

void OSRegisterVersion(const char *version);

static inline u8 __OSf32tou8(register f32 arg)
{
	f32 a;
	register f32 *ptr = &a;

	u8 r;

#ifdef __clang__
	__asm__ ("psq_st %0, 0(%1), 1, 2" :: "f"(arg), "r"(ptr));
#else
	asm { psq_st arg, 0(ptr), TRUE, 2 };
#endif // __clang__

	r = *(u8 *)ptr;

	return r;
}

static inline u16 __OSf32tou16(register f32 arg)
{
	f32 a;
	register f32 *ptr = &a;

	u16 r;

#ifdef __clang__
	__asm__ ("psq_st %0, 0(%1), 1, 3" :: "f"(arg), "r"(ptr));
#else
	asm { psq_st arg, 0(ptr), TRUE, 3 };
#endif // __clang__

	r = *(u16 *)ptr;

	return r;
}

static inline s16 __OSf32tos16(register f32 arg)
{
	f32 a;
	register f32 *ptr = &a;

	s16 r;

#ifdef __clang__
	__asm__ ("psq_st %0, 0(%1), 1, 5" :: "f"(arg), "r"(ptr));
#else
	asm { psq_st arg, 0(ptr), TRUE, 5 };
#endif // __clang__

	r = *(s16 *)ptr;

	return r;
}

static inline f32 __OSu16tof32(const u16 *arg)
{
	f32 ret;
	__asm__ ("psq_l %0, 0(%1), 1, 3" : "=f"(ret) : "r"(arg));

	return ret;
}

static inline void OSf32tou8(const f32 *in, u8 *out)
{
	*out = __OSf32tou8(*in);
}

static inline void OSf32tou16(const f32 *in, u16 *out)
{
	*out = __OSf32tou16(*in);
}

static inline void OSf32tos16(const f32 *in, s16 *out)
{
	*out = __OSf32tos16(*in);
}

static inline void OSu16tof32(const u16 *in, f32 *out)
{
	*out = __OSu16tof32(in);
}

#if defined(NDEBUG)
# define OSPhysicalToCached(addr)	(void *)((u32)addr + 0x80000000)
# define OSCachedToPhysical(addr)	(void *)((u32)addr - 0x80000000)
#endif

void *(OSPhysicalToCached)(void *addr);
void *(OSCachedToPhysical)(void *addr);

typedef struct OSAlarm OSAlarm;
typedef struct OSContext OSContext;

// [SPQE7T]/ISpyD.elf:.debug_info::0x295167 (type only)
typedef void OSAlarmHandler(OSAlarm *alarm, OSContext *context);

// [SPQE7T]/ISpyD.elf:.debug_info::0x2950d7
struct OSAlarm
{
	OSAlarmHandler	*handler;	// size 0x04, offset 0x00
	u32				tag;		// size 0x04, offset 0x04
	OSTime			fire;		// size 0x08, offset 0x08
	OSAlarm			*prev;		// size 0x04, offset 0x10
	OSAlarm			*next;		// size 0x04, offset 0x14
	OSTime			period;		// size 0x08, offset 0x18
	OSTime			start;		// size 0x08, offset 0x20
	void			*userData;	// size 0x04, offset 0x28
	/* 4 bytes padding */
}; // size 0x30

void OSCreateAlarm(OSAlarm *alarm);
void OSSetAlarm(OSAlarm *alarm, OSTime tick, OSAlarmHandler handler);
void OSSetPeriodicAlarm(OSAlarm *alarm, OSTime tick, OSTime period,
                        OSAlarmHandler handler);
void OSCancelAlarm(OSAlarm *alarm);
void OSSetAlarmUserData(OSAlarm *alarm, void *userData);
void *OSGetAlarmUserData(const OSAlarm *alarm);

#define OSGetAlarmUserDataAny(type_, alarm_)	\
	((type_)(OSGetAlarmUserData(alarm_)))

#define OSSetAlarmUserDataAny(alarm_, data_)	\
	OSSetAlarmUserData(alarm_, (void *)(data_))

// [SPQE7T]/ISpyD.elf:.debug_info::0x3135
struct OSContext
{
	register_t	gpr[32];			// size 0x080, offset 0x000
	register_t	cr;					// size 0x004, offset 0x080
	register_t	lr;					// size 0x004, offset 0x084
	register_t	ctr;				// size 0x004, offset 0x088
	register_t	xer;				// size 0x004, offset 0x08c
	f64			fpr[32];			// size 0x100, offset 0x090
	u32			fpscr_pad;			// size 0x004, offset 0x190
	register_t	fpscr;				// size 0x004, offset 0x194
	register_t	srr0;				// size 0x004, offset 0x198
	register_t	srr1;				// size 0x004, offset 0x19c
	u16			mode;				// size 0x002, offset 0x1a0
	u16			state;				// size 0x002, offset 0x1a2
	register_t	gqr[8];				// size 0x020, offset 0x1a4
	u32			psf_pad;			// size 0x004, offset 0x1c4
	f64			psf[32];			// size 0x100, offset 0x1c8
}; // size 0x2c8

void DCInvalidateRange(const void *buf, u32 len);
void DCFlushRange(const void *buf, u32 len);

BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL intrStatus);

#define OSSendMessageAny(msgQueue_, msg_, flags_)	\
	OSSendMessage(msgQueue_, (OSMessage)(msg_), flags_)

#define OSReceiveMessageAny(msgQueue_, msgOut_, flags_)	\
	OSReceiveMessage(msgQueue_, (OSMessage *)(msgOut_), flags_)

#define OSJamMessageAny(msgQueue_, msg_, flags_)	\
	OSJamMessage(msgQueue_, (OSMessage)(msg_), flags_)

typedef void *OSMessage;

typedef u32 OSMessageFlags;
enum OSMessageFlags_et
{
	OS_MESSAGE_NO_FLAGS			= 0,

	OS_MESSAGE_FLAG_PERSISTENT	= 1 << 0,
};

typedef struct OSMutex OSMutex;
typedef struct OSThread OSThread;

// [SPQE7T]/ISpyD.elf:.debug_info::0x333a
typedef struct OSMutexLink
{
	OSMutex	*next;	// size 0x04, offset 0x00
	OSMutex	*prev;	// size 0x04, offset 0x04
} OSMutexLink; // size 0x08

typedef enum OSThreadState
{
	OS_THREAD_STATE_EXITED		= 0,
	OS_THREAD_STATE_READY		= 1 << 0,
	OS_THREAD_STATE_RUNNING		= 1 << 1,
	OS_THREAD_STATE_SLEEPING	= 1 << 2,
	OS_THREAD_STATE_MORIBUND	= 1 << 3,
} OSThreadState;

typedef u16 OSThreadFlags;
enum OSThreadFlags_et
{
	OS_THREAD_NO_FLAGS	= 0,

	OS_THREAD_DETACHED	= 1 << 0,
};

typedef void OSSwitchThreadCallback(OSThread *curThread, OSThread *newThread);
typedef void *OSThreadFunction(void *arg);

// [SPQE7T]/ISpyD.elf:.debug_info::0x2fb2
typedef struct OSThreadQueue
{
	OSThread	*head;	// size 0x04, offset 0x00
	OSThread	*tail;	// size 0x04, offset 0x04
} OSThreadQueue; // size 0x08

// [SPQE7T]/ISpyD.elf:.debug_info::0x328b
typedef struct OSThreadLink
{
	OSThread	*next;	// size 0x04, offset 0x00
	OSThread	*prev;	// size 0x04, offset 0x04
} OSThreadLink; // size 0x08

// [SPQE7T]/ISpyD.elf:.debug_info::0x32cf
typedef struct OSMutexQueue
{
	OSMutex	*head;	// size 0x04, offset 0x00
	OSMutex	*tail;	// size 0x04, offset 0x04
} OSMutexQueue; // size 0x08

#define OS_THREAD_STACK_MAGIC	0xdeadbabe /* name known fron asserts */

// [SPQE7T]/ISpyD.elf:.debug_info::0x2feb
struct OSThread
{
	OSContext		context;		// size 0x2c8, offset 0x000
	u16				state;			// size 0x002, offset 0x2c8
	u16				attr;			// size 0x002, offset 0x2ca
	s32				suspend;		// size 0x004, offset 0x2cc
	s32				priority;		// size 0x004, offset 0x2d0
	s32				base;			// size 0x004, offset 0x2d4
	void			*val;			// size 0x004, offset 0x2d8
	OSThreadQueue	*queue;			// size 0x004, offset 0x2dc
	OSThreadLink	link;			// size 0x008, offset 0x2e0
	OSThreadQueue	queueJoin;		// size 0x008, offset 0x2e8
	OSMutex			*mutex;			// size 0x004, offset 0x2f0
	OSMutexQueue	queueMutex;		// size 0x008, offset 0x2f4
	OSThreadLink	linkActive;		// size 0x008, offset 0x2fc
	u8				*stackBase;		// size 0x004, offset 0x304
	u32				*stackEnd;		// size 0x004, offset 0x308
	s32				error;			// size 0x004, offset 0x30c
	void			*specific[2];	// size 0x008, offset 0x310
}; // size 0x318

void OSInitThreadQueue(OSThreadQueue *queue);
BOOL OSCreateThread(OSThread *thread, OSThreadFunction *func, void *funcArg,
                   void *stackBegin, u32 stackSize, int prio,
                   OSThreadFlags flags);
BOOL OSJoinThread(OSThread *thread, void *val);
s32 OSResumeThread(OSThread *thread);
void OSSleepThread(OSThreadQueue *queue);
void OSWakeupThread(OSThreadQueue *queue);

// [SPQE7T]/ISpyD.elf:.debug_info::0x23508c
typedef struct OSMessageQueue
{
	OSThreadQueue	queueSend;		// size 0x08, offset 0x00
	OSThreadQueue	queueReceive;	// size 0x08, offset 0x08
	OSMessage		*msgArray;		// size 0x04, offset 0x10
	s32				msgCount;		// size 0x04, offset 0x14
	s32				firstIndex;		// size 0x04, offset 0x18
	s32				usedCount;		// size 0x04, offset 0x1c
} OSMessageQueue; // size 0x20

void OSInitMessageQueue(OSMessageQueue *msgQueue, OSMessage *buffer,
                        int capacity);
int OSSendMessage(OSMessageQueue *msgQueue, OSMessage msg,
                  OSMessageFlags flags);
int OSReceiveMessage(OSMessageQueue *msgQueue, OSMessage *msgOut,
                     OSMessageFlags flags);
int OSJamMessage(OSMessageQueue *msgQueue, OSMessage msg, OSMessageFlags flags);

// [SPQE7T]/ISpyD.elf:.debug_info::0x2f63
struct OSMutex
{
	OSThreadQueue	queue;		// size 0x08, offset 0x00
	OSThread		*thread;	// size 0x04, offset 0x08
	s32				count;		// size 0x04, offset 0x0c
	OSMutexLink		link;		// size 0x08, offset 0x10
}; // size 0x18

void OSInitMutex(OSMutex *mutex);
void OSLockMutex(OSMutex *mutex);
void OSUnlockMutex(OSMutex *mutex);

extern u32 OS_BUS_CLOCK ATTR_ADDRESS(0x800000f8);

#define OS_TIMER_CLOCK	(OS_BUS_CLOCK / 4)

// clang-format off
#define OSTicksToSeconds(ticks)			((ticks)        / (OS_TIMER_CLOCK         )       )
#define OSTicksToMilliseconds(ticks)	((ticks)        / (OS_TIMER_CLOCK / 1000  )       )
#define OSTicksToMicroseconds(ticks)	((ticks) * 8    / (OS_TIMER_CLOCK / 125000)       )
#define OSTicksToNanoseconds(ticks)		((ticks) * 8000 / (OS_TIMER_CLOCK / 125000)       )

#define OSSecondsToTicks(sec)			((  sec)        * (OS_TIMER_CLOCK         )       )
#define OSMillisecondsToTicks(msec)		(( msec)        * (OS_TIMER_CLOCK / 1000  )       )
#define OSMicrosecondsToTicks(usec)		(( usec)        * (OS_TIMER_CLOCK / 125000) / 8   )
#define OSNanosecondsToTicks(nsec)		(( nsec)        * (OS_TIMER_CLOCK / 125000) / 8000)

#define OSDiffTick(tick1, tick0)		((OSTick)(tick1) - (OSTick)(tick0))
// clang-format on

OSTime OSGetTime(void);
OSTick OSGetTick(void);

typedef void AIDMACallback(void);

BOOL AICheckInit(void);
void AIInit(void *stack);

// [SPQE7T]/ISpyD.elf:.debug_info::0x119a90
typedef struct ARCHandle
{
	void		*archiveStartAddr;	// size 0x04, offset 0x00
	void		*FSTStart;			// size 0x04, offset 0x04
	void		*fileStart;			// size 0x04, offset 0x08
	u32			entryNum;			// size 0x04, offset 0x0c // more accurately entryCount
	const char	*FSTStringStart;	// size 0x04, offset 0x10
	u32			FSTLength;			// size 0x04, offset 0x14
	u32			currDir;			// size 0x04, offset 0x18 // more accurately what ARCDir calls entryNum
} ARCHandle; // size 0x1c

// [SPQE7T]/ISpyD.elf:.debug_info::0x368e0b
typedef struct ARCFileInfo
{
	ARCHandle	*handle;		// size 0x04, offset 0x00
	u32			startOffset;	// size 0x04, offset 0x04
	u32			length;			// size 0x04, offset 0x08
} ARCFileInfo; // size 0x0c

// [SPQE7T]/ISpyD.elf:.debug_info::0x3749e4
typedef struct ARCDir
{
	ARCHandle	*handle;	// size 0x04, offset 0x00
	u32			entryNum;	// size 0x04, offset 0x04
	u32			location;	// size 0x04, offset 0x08
	u32			next;		// size 0x04, offset 0x0c
} ARCDir; // size 0x10

// [SPQE7T]/ISpyD.elf:.debug_info::0x374a3e
typedef struct ARCDirEntry
{
	ARCHandle	*handle;	// size 0x04, offset 0x00
	u32			entryNum;	// size 0x04, offset 0x04
	BOOL		isDir;		// size 0x04, offset 0x08
	const char	*name;		// size 0x04, offset 0x0c
} ARCDirEntry; // size 0x10

BOOL ARCInitHandle(void *bin, ARCHandle *handle);
BOOL ARCOpen(ARCHandle *handle, const char *filename, ARCFileInfo *af);
BOOL ARCFastOpen(ARCHandle *handle, int entrynum, ARCFileInfo *af);
s32 ARCConvertPathToEntrynum(ARCHandle *handle, const char *path);
void *ARCGetStartAddrInMem(ARCFileInfo *af);
u32 ARCGetLength(ARCFileInfo *af);
BOOL ARCClose(ARCFileInfo *af);
BOOL ARCChangeDir(ARCHandle *handle, const char *dirname);
BOOL ARCOpenDir(ARCHandle *handle, const char *dirname, ARCDir *dir);
BOOL ARCReadDir(ARCDir *dir, ARCDirEntry *direntry);
BOOL ARCCloseDir(ARCDir *dir);

#define AX_MAX_VOICES					0x60	/* name known from asserts */
#define AX_SAMPLES_PER_FRAME			0x60
#define AX_MAX_VOLUME					0x8000
#define AX_SAMPLE_RATE					32000
#define AX_ADPCM_FRAME_SIZE				8
#define AX_ADPCM_SAMPLE_BYTES_PER_FRAME	(AX_ADPCM_FRAME_SIZE - 1)
#define AX_ADPCM_SAMPLES_PER_BYTE		2
#define AX_ADPCM_SAMPLES_PER_FRAME		(AX_ADPCM_SAMPLE_BYTES_PER_FRAME * AX_ADPCM_SAMPLES_PER_BYTE)
#define AX_ADPCM_NIBBLES_PER_FRAME		(AX_ADPCM_FRAME_SIZE * 2)
#define AX_FRAME_SIZE					(AX_SAMPLES_PER_FRAME * AX_SAMPLE_DEPTH_BYTES)

#define AX_PRIORITY_STACKS	32

#define AX_PRIORITY_FREE	0
#define AX_PRIORITY_MIN		1
#define AX_PRIORITY_MAX		(AX_PRIORITY_STACKS - 1)

// [SPQE7T]/ISpyD.elf:.debug_info::0x36a6f8 (type only)
typedef void AXFrameCallback(void);

// [SPQE7T]/ISpyD.elf:.debug_info::0x36c40e (type only)
typedef void AXAuxCallback(void *data, void *context);

typedef u16 AXVoiceState;
enum AXVoiceState_et
{
	AX_VOICE_STOP,
	AX_VOICE_RUN,
};

enum
{
	AX_PB_OFF		= 0,
	AX_PB_LPF_ON	= 1 << 0,
	AX_PB_BIQUAD_ON	= 1 << 1,
};

typedef enum AXMixerCtrlFlags
{
	AX_MIXER_CTRL_L			= 1 << 0,
	AX_MIXER_CTRL_R			= 1 << 1,
	AX_MIXER_CTRL_DELTA		= 1 << 2,
	AX_MIXER_CTRL_S			= 1 << 3,
	AX_MIXER_CTRL_DELTA_S	= 1 << 4,

	AX_MIXER_CTRL_A_L		= 1 << 16,
	AX_MIXER_CTRL_A_R		= 1 << 17,
	AX_MIXER_CTRL_A_DELTA	= 1 << 18,
	AX_MIXER_CTRL_A_S		= 1 << 19,
	AX_MIXER_CTRL_A_DELTA_S	= 1 << 20,

	AX_MIXER_CTRL_B_L		= 1 << 21,
	AX_MIXER_CTRL_B_R		= 1 << 22,
	AX_MIXER_CTRL_B_DELTA	= 1 << 23,
	AX_MIXER_CTRL_B_S		= 1 << 24,
	AX_MIXER_CTRL_B_DELTA_S	= 1 << 25,

	AX_MIXER_CTRL_C_L		= 1 << 26,
	AX_MIXER_CTRL_C_R		= 1 << 27,
	AX_MIXER_CTRL_C_DELTA	= 1 << 28,
	AX_MIXER_CTRL_C_S		= 1 << 29,
	AX_MIXER_CTRL_C_DELTA_S	= 1 << 30,
} AXMixerCtrlFlags;

typedef u32 AXVPBSrcType;
enum AXVPBSrcType_et
{
	AX_SRC_TYPE_0,
	AX_SRC_TYPE_1,
	AX_SRC_TYPE_2,
	AX_SRC_TYPE_3,
	AX_SRC_TYPE_4TAP_16K,	/* name known from asserts */
};

typedef u32 AXVPBSyncFlags;
enum AXVPBSyncFlags_et
{
	AX_VPB_SYNC_FLAG_SRC_TYPE				= (1 <<  0),
	AX_VPB_SYNC_FLAG_MIXER_CTRL				= (1 <<  1),
	AX_VPB_SYNC_FLAG_STATE					= (1 <<  2),
	AX_VPB_SYNC_FLAG_TYPE					= (1 <<  3),
	AX_VPB_SYNC_FLAG_MIX					= (1 <<  4),
	AX_VPB_SYNC_FLAG_ITD					= (1 <<  5),
	AX_VPB_SYNC_FLAG_ITD_TARGET				= (1 <<  6),
	AX_VPB_SYNC_FLAG_DPOP					= (1 <<  7),
	AX_VPB_SYNC_FLAG_VE						= (1 <<  8),
	AX_VPB_SYNC_FLAG_VE_DELTA				= (1 <<  9),
	AX_VPB_SYNC_FLAG_ADDR					= 1 << 10,
	AX_VPB_SYNC_FLAG_ADDR_LOOP_FLAG			= 1 << 11,
	AX_VPB_SYNC_FLAG_ADDR_LOOP_ADDR			= 1 << 12,
	AX_VPB_SYNC_FLAG_ADDR_END_ADDR			= 1 << 13,
	AX_VPB_SYNC_FLAG_ADDR_CURRENT_ADDR		= 1 << 14,
	AX_VPB_SYNC_FLAG_ADPCM					= 1 << 15,
	AX_VPB_SYNC_FLAG_SRC					= 1 << 16,
	AX_VPB_SYNC_FLAG_SRC_RATIO				= 1 << 17,
	AX_VPB_SYNC_FLAG_ADPCM_LOOP				= 1 << 18,
	AX_VPB_SYNC_FLAG_LPF					= 1 << 19,
	AX_VPB_SYNC_FLAG_LPF_COEFS				= 1 << 20,
	AX_VPB_SYNC_FLAG_BIQUAD					= 1 << 21,
	AX_VPB_SYNC_FLAG_BIQUAD_COEFS			= 1 << 22,
	AX_VPB_SYNC_FLAG_RMT_ON					= 1 << 23,
	AX_VPB_SYNC_FLAG_RMT_MIXER_CTRL			= 1 << 24,
	AX_VPB_SYNC_FLAG_RMT_MIX				= 1 << 25,
	AX_VPB_SYNC_FLAG_RMT_DPOP				= 1 << 26,
	AX_VPB_SYNC_FLAG_RMT_SRC				= 1 << 27,
	AX_VPB_SYNC_FLAG_RMT_IIR				= 1 << 28,
	AX_VPB_SYNC_FLAG_RMT_IIR_LPF_COEFS		= 1 << 29,
	AX_VPB_SYNC_FLAG_RMT_IIR_BIQUAD_COEFS	= 1 << 30,
	AX_VPB_SYNC_FLAG_FULL_PB				= 1 << 31
};

typedef u32 AXCLMode;
enum AXCLMode_et
{
	AX_CL_MODE_STEREO,
	AX_CL_MODE_SURROUND,
	AX_CL_MODE_DPL2,
};

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82853 (type only)
typedef void AXVPBCallback(void *p);

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82a39
typedef struct _AXPBMIX
{
	u16	vL;				// size 0x02, offset 0x00
	u16	vDeltaL;		// size 0x02, offset 0x02
	u16	vR;				// size 0x02, offset 0x04
	u16	vDeltaR;		// size 0x02, offset 0x06
	u16	vAuxAL;			// size 0x02, offset 0x08
	u16	vDeltaAuxAL;	// size 0x02, offset 0x0a
	u16	vAuxAR;			// size 0x02, offset 0x0c
	u16	vDeltaAuxAR;	// size 0x02, offset 0x0e
	u16	vAuxBL;			// size 0x02, offset 0x10
	u16	vDeltaAuxBL;	// size 0x02, offset 0x12
	u16	vAuxBR;			// size 0x02, offset 0x14
	u16	vDeltaAuxBR;	// size 0x02, offset 0x16
	u16	vAuxCL;			// size 0x02, offset 0x18
	u16	vDeltaAuxCL;	// size 0x02, offset 0x1a
	u16	vAuxCR;			// size 0x02, offset 0x1c
	u16	vDeltaAuxCR;	// size 0x02, offset 0x1e
	u16	vS;				// size 0x02, offset 0x10
	u16	vDeltaS;		// size 0x02, offset 0x12
	u16	vAuxAS;			// size 0x02, offset 0x14
	u16	vDeltaAuxAS;	// size 0x02, offset 0x16
	u16	vAuxBS;			// size 0x02, offset 0x18
	u16	vDeltaAuxBS;	// size 0x02, offset 0x1a
	u16	vAuxCS;			// size 0x02, offset 0x1c
	u16	vDeltaAuxCS;	// size 0x02, offset 0x1e
} AXPBMIX; // size 0x30

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82c05
typedef struct _AXPBITD
{
	u16	flag;			// size 0x02, offset 0x00
	u16	bufferHi;		// size 0x02, offset 0x02
	u16	bufferLo;		// size 0x02, offset 0x04
	u16	shiftL;			// size 0x02, offset 0x06
	u16	shiftR;			// size 0x02, offset 0x08
	u16	targetShiftL;	// size 0x02, offset 0x0a
	u16	targetShiftR;	// size 0x02, offset 0x0c
} AXPBITD; // size 0x0e

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82c9a
typedef struct _AXPBDPOP
{
	s16	aL;		// size 0x02, offset 0x00
	s16	aAuxAL;	// size 0x02, offset 0x02
	s16	aAuxBL;	// size 0x02, offset 0x04
	s16	aAuxCL;	// size 0x02, offset 0x06
	s16	aR;		// size 0x02, offset 0x08
	s16	aAuxAR;	// size 0x02, offset 0x0a
	s16	aAuxBR;	// size 0x02, offset 0x0c
	s16	aAuxCR;	// size 0x02, offset 0x0e
	s16	aS;		// size 0x02, offset 0x10
	s16	aAuxAS;	// size 0x02, offset 0x12
	s16	aAuxBS;	// size 0x02, offset 0x14
	s16	aAuxCS;	// size 0x02, offset 0x16
} AXPBDPOP; // size 0x18

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82d6b
typedef struct _AXPBVE
{
	u16	currentVolume;	// size 0x02, offset 0x00
	s16	currentDelta;	// size 0x02, offset 0x02
} AXPBVE; // size 0x04

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82da9
typedef struct _AXPBADDR
{
	u16	loopFlag;			// size 0x02, offset 0x00
	u16	format;				// size 0x02, offset 0x02
	u16	loopAddressHi;		// size 0x02, offset 0x04
	u16	loopAddressLo;		// size 0x02, offset 0x06
	u16	endAddressHi;		// size 0x02, offset 0x08
	u16	endAddressLo;		// size 0x02, offset 0x0a
	u16	currentAddressHi;	// size 0x02, offset 0x0c
	u16	currentAddressLo;	// size 0x02, offset 0x0e
} AXPBADDR; // size 0x10

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x825d6
typedef struct _AXPBADPCM
{
	u16	a[8][2];	// size 0x20, offset 0x00
	u16	gain;		// size 0x02, offset 0x20
	u16	pred_scale;	// size 0x02, offset 0x22
	u16	yn1;		// size 0x02, offset 0x24
	u16	yn2;		// size 0x02, offset 0x26
} AXPBADPCM; // size 0x28

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82e72
typedef struct _AXPBSRC
{
	u16	ratioHi;			// size 0x02, offset 0x00
	u16	ratioLo;			// size 0x02, offset 0x02
	u16	currentAddressFrac;	// size 0x02, offset 0x04
	u16	last_samples[4];	// size 0x08, offset 0x06
} AXPBSRC; // size 0x0e

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82652
typedef struct _AXPBADPCMLOOP
{
	u16	loop_pred_scale;	// size 0x02, offset 0x00
	u16	loop_yn1;			// size 0x02, offset 0x02
	u16	loop_yn2;			// size 0x02, offset 0x04
} AXPBADPCMLOOP; // size 0x06

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82ee9
typedef struct _AXPBLPF
{
	u16	on;		// size 0x02, offset 0x00
	u16	yn1;	// size 0x02, offset 0x02
	u16	a0;		// size 0x02, offset 0x04
	u16	b0;		// size 0x02, offset 0x06
} AXPBLPF; // size 0x08

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82f2e
typedef struct _AXPBBIQUAD
{
	u16	on;		// size 0x02, offset 0x00
	u16	xn1;	// size 0x02, offset 0x02
	u16	xn2;	// size 0x02, offset 0x04
	u16	yn1;	// size 0x02, offset 0x06
	u16	yn2;	// size 0x02, offset 0x08
	u16	b0;		// size 0x02, offset 0x0a
	u16	b1;		// size 0x02, offset 0x0c
	u16	b2;		// size 0x02, offset 0x0e
	u16	a1;		// size 0x02, offset 0x10
	u16	a2;		// size 0x02, offset 0x12
} AXPBBIQUAD; // size 0x14

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82fc7
typedef struct _AXPBRMTMIX
{
	u16	vMain0;			// size 0x02, offset 0x00
	u16	vDeltaMain0;	// size 0x02, offset 0x02
	u16	vAux0;			// size 0x02, offset 0x04
	u16	vDeltaAux0;		// size 0x02, offset 0x06
	u16	vMain1;			// size 0x02, offset 0x08
	u16	vDeltaMain1;	// size 0x02, offset 0x0a
	u16	vAux1;			// size 0x02, offset 0x0c
	u16	vDeltaAux1;		// size 0x02, offset 0x0e
	u16	vMain2;			// size 0x02, offset 0x10
	u16	vDeltaMain2;	// size 0x02, offset 0x12
	u16	vAux2;			// size 0x02, offset 0x14
	u16	vDeltaAux2;		// size 0x02, offset 0x16
	u16	vMain3;			// size 0x02, offset 0x18
	u16	vDeltaMain3;	// size 0x02, offset 0x1a
	u16	vAux3;			// size 0x02, offset 0x1c
	u16	vDeltaAux3;		// size 0x02, offset 0x1e
} AXPBRMTMIX; // size 0x20

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x8310a
typedef struct _AXPBRMTDPOP
{
	s16	aMain0;	// size 0x02, offset 0x00
	s16	aMain1;	// size 0x02, offset 0x02
	s16	aMain2;	// size 0x02, offset 0x04
	s16	aMain3;	// size 0x02, offset 0x06
	s16	aAux0;	// size 0x02, offset 0x08
	s16	aAux1;	// size 0x02, offset 0x0a
	s16	aAux2;	// size 0x02, offset 0x0c
	s16	aAux3;	// size 0x02, offset 0x0e
} AXPBRMTDPOP; // size 0x10

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x831a2
typedef struct _AXPBRMTSRC
{
	u16	currentAddressFrac;	// size 0x02, offset 0x00
	u16	last_samples[4];	// size 0x08, offset 0x02
} AXPBRMTSRC; // size 0x0a

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x831f8
typedef union __AXPBRMTIIR
{
	AXPBLPF		lpf;	// size 0x08
	AXPBBIQUAD	biquad;	// size 0x14
} __AXPBRMTIIR; // size 0x14

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x8286e
typedef struct _AXPB
{
	u16				nextHi;			// size 0x002, offset 0x000 (0x028)
	u16				nextLo;			// size 0x002, offset 0x002 (0x02a)
	u16				currHi;			// size 0x002, offset 0x004 (0x02c)
	u16				currLo;			// size 0x002, offset 0x006 (0x02e)
	u16				srcSelect;		// size 0x002, offset 0x008 (0x030)
	u16				coefSelect;		// size 0x002, offset 0x00a (0x032)
	u32				mixerCtrl;		// size 0x004, offset 0x00c (0x034)
	AXVoiceState	state;			// size 0x002, offset 0x010 (0x038)
	u16				type;			// size 0x002, offset 0x012 (0x03a)
	AXPBMIX			mix;			// size 0x030, offset 0x014 (0x03c)
	AXPBITD			itd;			// size 0x00e, offset 0x044 (0x06c)
	AXPBDPOP		dpop;			// size 0x018, offset 0x052 (0x07a)
	AXPBVE			ve;				// size 0x004, offset 0x06a (0x092)
	AXPBADDR		addr;			// size 0x010, offset 0x06e (0x096)
	AXPBADPCM		adpcm;			// size 0x028, offset 0x07e (0x0a6)
	AXPBSRC			src;			// size 0x00e, offset 0x0a6 (0x0ce)
	AXPBADPCMLOOP	adpcmLoop;		// size 0x006, offset 0x0b4 (0x0dc)
	AXPBLPF			lpf;			// size 0x008, offset 0x0ba (0x0e2)
	AXPBBIQUAD		biquad;			// size 0x014, offset 0x0c2 (0x0ea)
	u16				remote;			// size 0x002, offset 0x0d6 (0x0fe)
	u16				rmtMixerCtrl;	// size 0x002, offset 0x0d8 (0x100)
	AXPBRMTMIX		rmtMix;			// size 0x020, offset 0x0da (0x102)
	AXPBRMTDPOP		rmtDpop;		// size 0x010, offset 0x0fa (0x122)
	AXPBRMTSRC		rmtSrc;			// size 0x00a, offset 0x10a (0x132)
	__AXPBRMTIIR	rmtIIR;			// size 0x014, offset 0x114 (0x13c)
	byte2_t			pad[12];
} AXPB; // size 0x140

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82773
typedef struct _AXVPB
{
	void			*next;			// size 0x004, offset 0x000
	void			*prev;			// size 0x004, offset 0x004
	void			*next1;			// size 0x004, offset 0x008
	u32				priority;		// size 0x004, offset 0x00c
	AXVPBCallback	*callback;		// size 0x004, offset 0x010
	register_t		userContext;	// size 0x004, offset 0x014
	u32				index;			// size 0x004, offset 0x018
	AXVPBSyncFlags	sync;			// size 0x004, offset 0x01c
	u32				depop;			// size 0x004, offset 0x020
	void			*itdBuffer;		// size 0x004, offset 0x024
	AXPB			pb;				// size 0x140, offset 0x028
} AXVPB; // size 0x168

void AXInit(void);
BOOL AXIsInit(void);

AXVPB *AXAcquireVoice(u32 priority, AXVPBCallback *cb, register_t userContext);
void AXFreeVoice(AXVPB *p);
void AXSetVoicePriority(AXVPB *p, u32 priority);

void AXRegisterAuxACallback(AXAuxCallback *cb, void *context);
void AXRegisterAuxBCallback(AXAuxCallback *cb, void *context);
void AXRegisterAuxCCallback(AXAuxCallback *cb, void *context);
void AXGetAuxACallback(AXAuxCallback **cbOut, void **contextOut);
void AXGetAuxBCallback(AXAuxCallback **cbOut, void **contextOut);
void AXGetAuxCCallback(AXAuxCallback **cbOut, void **contextOut);

void AXSetMode(AXCLMode mode);
void AXSetMasterVolume(u16 volume);
u16 AXGetAuxAReturnVolume(void);
u16 AXGetAuxBReturnVolume(void);
u16 AXGetAuxCReturnVolume(void);
void AXSetAuxAReturnVolume(u16 volume);
void AXSetAuxBReturnVolume(u16 volume);
void AXSetAuxCReturnVolume(u16 volume);

AXFrameCallback *AXRegisterCallback(AXFrameCallback *cb);

void AXSetVoiceState(AXVPB *p, u16 state);
void AXSetVoiceAddr(AXVPB *p, AXPBADDR *addr);
void AXGetLpfCoefs(u16 freq, u16 *a0Out, u16 *b0Out);
u32 AXGetMaxVoices(void);

// [SPQE7T]/ISpyD.elf:.debug_info::0x36bdec (type only)
typedef void *AXFXAllocFunc(u32 /* explicitly unnamed */);

// [SPQE7T]/ISpyD.elf:.debug_info::0x36be0f (type only)
typedef void AXFXFreeFunc(void */* explicitly unnamed */);

// [SPQE7T]/ISpyD.elf:.debug_info::0x36c3ae
typedef struct AXFX_BUS
{
	s32	*left;		// size 0x04, offset 0x00
	s32	*right;		// size 0x04, offset 0x04
	s32	*surround;	// size 0x04, offset 0x08
} AXFX_BUS; // size 0x0c

// [SPQE7T]/ISpyD.elf:.debug_info::0x36bec8
typedef struct AXFX_REVERBHI_EXP
{
	f32			*earlyLine[3];				// size 0x00c, offset 0x000
	u32			earlyPos[3];				// size 0x00c, offset 0x00c
	u32			earlyLength;				// size 0x004, offset 0x018
	u32			earlyMaxLength;				// size 0x004, offset 0x01c
	f32			earlyCoef[3];				// size 0x00c, offset 0x020
	f32			*preDelayLine[3];			// size 0x00c, offset 0x02c
	u32			preDelayPos;				// size 0x004, offset 0x038
	u32			preDelayLength;				// size 0x004, offset 0x03c
	u32			preDelayMaxLength;			// size 0x004, offset 0x040
	f32			*combLine[3][3];			// size 0x024, offset 0x044
	u32			combPos[3];					// size 0x00c, offset 0x068
	u32			combLength[3];				// size 0x00c, offset 0x074
	u32			combMaxLength[3];			// size 0x00c, offset 0x080
	f32			combCoef[3];				// size 0x00c, offset 0x08c
	f32			*allpassLine[3][2];			// size 0x018, offset 0x098
	u32			allpassPos[2];				// size 0x008, offset 0x0b0
	u32			allpassLength[2];			// size 0x008, offset 0x0b8
	u32			allpassMaxLength[2];		// size 0x008, offset 0x0c0
	f32			*lastAllpassLine[3];		// size 0x00c, offset 0x0c8
	u32			lastAllpassPos[3];			// size 0x00c, offset 0x0d4
	u32			lastAllpassLength[3];		// size 0x00c, offset 0x0e0
	u32			lastAllpassMaxLength[3];	// size 0x00c, offset 0x0ec
	f32			allpassCoef;				// size 0x004, offset 0x0f8
	f32			lastLpfOut[3];				// size 0x00c, offset 0x0fc
	f32			lpfCoef;					// size 0x004, offset 0x108
	u32			active;						// size 0x004, offset 0x10c
	u32			earlyMode;					// size 0x004, offset 0x110
	f32			preDelayTimeMax;			// size 0x004, offset 0x114
	f32			preDelayTime;				// size 0x004, offset 0x118
	u32			fusedMode;					// size 0x004, offset 0x11c
	f32			fusedTime;					// size 0x004, offset 0x120
	f32			coloration;					// size 0x004, offset 0x124
	f32			damping;					// size 0x004, offset 0x128
	f32			crosstalk;					// size 0x004, offset 0x12c
	f32			earlyGain;					// size 0x004, offset 0x130
	f32			fusedGain;					// size 0x004, offset 0x134
	AXFX_BUS	*busIn;						// size 0x004, offset 0x138
	AXFX_BUS	*busOut;					// size 0x004, offset 0x13c
	f32			outGain;					// size 0x004, offset 0x140
	f32			sendGain;					// size 0x004, offset 0x144
} AXFX_REVERBHI_EXP; // size 0x148

// [SPQE7T]/ISpyD.elf:.debug_info::0x36be2c
typedef struct AXFX_REVERBHI
{
	AXFX_REVERBHI_EXP	reverbInner;	// size 0x148, offset 0x000
	f32					coloration;		// size 0x004, offset 0x148
	f32					mix;			// size 0x004, offset 0x14c
	f32					time;			// size 0x004, offset 0x150
	f32					damping;		// size 0x004, offset 0x154
	f32					preDelay;		// size 0x004, offset 0x158
	f32					crosstalk;		// size 0x004, offset 0x15c
} AXFX_REVERBHI; // size 0x160

void AXFXSetHooks(AXFXAllocFunc *alloc, AXFXFreeFunc *free);
void AXFXGetHooks(AXFXAllocFunc **allocOut, AXFXFreeFunc **freeOut);
BOOL AXFXReverbHiInit(AXFX_REVERBHI *reverbHi);
BOOL AXFXReverbHiShutdown(AXFX_REVERBHI *reverbHi);

// Available for use as the argument cb to AXRegisterAuxCallback
void AXFXReverbHiCallback(void *data, void *context);

typedef long DVDResult;
enum DVDResult_et
{
	DVD_ERR_OK				= 0,

	DVD_ERR_FATAL			= -1,
//	DVD_ERR_2				= -2,
	DVD_ERR_CANCELED		= -3,
	DVD_ERR_COVER_CLOSED	= -4,

#define DVD_ESUCCESS	DVD_ERR_OK
#define DVD_EFATAL		DVD_ERR_FATAL
#define DVD_ECANCELED	DVD_ERR_CANCELED
#define DVD_ECOVER		DVD_ERR_COVER_CLOSED
};

typedef long DVDState;
enum DVDState_et
{
	DVD_STATE_IDLE			=  0,

	DVD_STATE_BUSY			=  1,
	DVD_STATE_WAITING		=  2,
	DVD_STATE_COVER_CLOSED	=  3,
	DVD_STATE_NO_DISK		=  4,
	DVD_STATE_COVER_OPENED	=  5,
	DVD_STATE_WRONG_DISK_ID	=  6,
	DVD_STATE_7				=  7,
	DVD_STATE_PAUSED		=  8,
	DVD_STATE_9				=  9,
	DVD_STATE_CANCELED		= 10,
	DVD_STATE_DISK_ERROR	= 11,
	DVD_STATE_MOTOR_STOPPED	= 12,

	DVD_STATE_FATAL			= -1,
};

DVDState DVDGetDriveStatus(void);

// Matrix types

typedef f32 Mtx23[2][3];
typedef f32 Mtx[3][4];
typedef f32 Mtx43[4][3];
typedef f32 Mtx44[4][4];

typedef f32 (*Mtx23Ptr)[3];
typedef f32 (*MtxPtr)[4];
typedef f32 (*Mtx43Ptr)[3];
typedef f32 (*Mtx44Ptr)[4];

// not official names, just what i thought would make sense
typedef const f32 (*CMtx23Ptr)[3];
typedef const f32 (*CMtxPtr)[4];
typedef const f32 (*CMtx43Ptr)[3];
typedef const f32 (*CMtx44Ptr)[4];

// Vectors

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x7a261
typedef struct Vec2
{
	f32	x;	// size 0x04, offset 0x00
	f32	y;	// size 0x04, offset 0x04
} Vec2; // size 0x08

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x7a232
typedef struct Vec
{
	f32	x;	// size 0x04, offset 0x00
	f32	y;	// size 0x04, offset 0x04
	f32	z;	// size 0x04, offset 0x08
} Vec; // size 0x0c

typedef Vec2 *Vec2Ptr;
typedef Vec *VecPtr;

typedef const Vec2 *CVec2Ptr;
typedef const Vec *CVecPtr;

#if !defined(NDEBUG)
# define MTXIdentity	C_MTXIdentity
# define MTXConcat		C_MTXConcat
# define MTXInverse		C_MTXInverse
# define MTXRotRad		C_MTXRotRad
# define MTXTrans		C_MTXTrans
# define MTXTransApply	C_MTXTransApply
# define MTXScale		C_MTXScale

# define MTXMultVec		C_MTXMultVec
#else
# define MTXIdentity	PSMTXIdentity
# define MTXConcat		PSMTXConcat
# define MTXInverse		PSMTXInverse
# define MTXRotRad		PSMTXRotRad
# define MTXTrans		PSMTXTrans
# define MTXTransApply	PSMTXTransApply
# define MTXScale		PSMTXScale

# define MTXMultVec		PSMTXMultVec
#endif

void PSMTXIdentity(Mtx m);
void C_MTXIdentity(Mtx m);

void PSMTXCopy(CMtxPtr src, MtxPtr dst);

void PSMTXConcat(CMtxPtr a, CMtxPtr b, MtxPtr ab);
void C_MTXConcat(CMtxPtr a, CMtxPtr b, MtxPtr ab);

void PSMTXInverse(CMtxPtr src, MtxPtr inv);
void C_MTXInverse(CMtxPtr src, MtxPtr inv);

void PSMTXRotRad(Mtx m, char axis, f32);
void C_MTXRotRad(Mtx m, char axis, f32);

void PSMTXTrans(Mtx m, f32, f32, f32);
void C_MTXTrans(Mtx m, f32, f32, f32);

void PSMTXTransApply(CMtxPtr src, MtxPtr dst, f32, f32, f32);
void C_MTXTransApply(CMtxPtr src, MtxPtr dst, f32, f32, f32);

void PSMTXScale(MtxPtr m, f32, f32, f32);
void C_MTXScale(MtxPtr m, f32, f32, f32);

void PSMTXMultVec(CMtxPtr m, CVecPtr src, VecPtr dst);
void C_MTXMultVec(CMtxPtr m, CVecPtr src, VecPtr dst);

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48f80e
typedef enum _GXAlphaOp
{
	GX_AOP_AND,
	GX_AOP_OR,
	GX_AOP_XOR,
	GX_AOP_XNOR,

	GX_MAX_ALPHAOP
} GXAlphaOp;

// [SPQE7T]/ISpyD.elf:.debug_info::0x21232f
typedef enum _GXAnisotropy
{
	GX_ANISO_1,
	GX_ANISO_2,
	GX_ANISO_4,

	GX_MAX_ANISOTROPY
} GXAnisotropy;

// NOTE: older GX version (Feb 24 2009 00:57:49)
// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x10044
typedef enum _GXAttnFn
{
	GX_AF_SPEC,
	GX_AF_SPOT,

	GX_AF_NONE
} GXAttnFn;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x438266
typedef enum _GXAttr
{
	GX_VA_PNMTXIDX,

	GX_VA_TEX0MTXIDX,
	GX_VA_TEX1MTXIDX,
	GX_VA_TEX2MTXIDX,
	GX_VA_TEX3MTXIDX,
	GX_VA_TEX4MTXIDX,
	GX_VA_TEX5MTXIDX,
	GX_VA_TEX6MTXIDX,
	GX_VA_TEX7MTXIDX,

	GX_VA_POS,
	GX_VA_NRM,

	GX_VA_CLR0,
	GX_VA_CLR1,

	GX_VA_TEX0,
	GX_VA_TEX1,
	GX_VA_TEX2,
	GX_VA_TEX3,
	GX_VA_TEX4,
	GX_VA_TEX5,
	GX_VA_TEX6,
	GX_VA_TEX7,

	GX_POS_MTX_ARRAY,
	GX_NRM_MTX_ARRAY,
	GX_TEX_MTX_ARRAY,
	GX_LIGHT_ARRAY,

	GX_VA_NBT,

	GX_VA_MAX_ATTR,
	GX_VA_NULL = 0xff
} GXAttr;

// [SPQE7T]/ISpyD.elf:.debug_info::0x268dc0
typedef enum _GXAttrType
{
	GX_NONE,

	GX_DIRECT,

	GX_INDEX8,
	GX_INDEX16,
} GXAttrType;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x45d222
typedef enum _GXBlendFactor
{
	GX_BL_ZERO,
	GX_BL_ONE,

	GX_BL_SRCCLR,
	GX_BL_INVSRCCLR,

	GX_BL_SRCALPHA,
	GX_BL_INVSRCALPHA,

	GX_BL_DSTALPHA,
	GX_BL_INVDSTALPHA,

	GX_BL_DSTCLR		= GX_BL_SRCCLR,
	GX_BL_INVDSTCLR		= GX_BL_INVSRCCLR
} GXBlendFactor;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48f667
typedef enum _GXBlendMode
{
	GX_BM_NONE,

	GX_BM_BLEND,
	GX_BM_LOGIC,
	GX_BM_SUBTRACT,

	GX_MAX_BLENDMODE
} GXBlendMode;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x4070a5
typedef enum _GXChannelID
{
	GX_COLOR0,
	GX_COLOR1,

	GX_ALPHA0,
	GX_ALPHA1,

	GX_COLOR0A0,
	GX_COLOR1A1,

	GX_COLOR_ZERO,

	GX_ALPHA_BUMP,
	GX_ALPHA_BUMPN,

	GX_COLOR_NULL = 0xff
} GXChannelID;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x401562
typedef enum _GXColorSrc
{
	GX_SRC_REG,
	GX_SRC_VTX,
} GXColorSrc;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x45d66e
typedef enum _GXCompare
{
	GX_NEVER,

	GX_LESS,
	GX_EQUAL,
	GX_LEQUAL,
	GX_GREATER,
	GX_NEQUAL,
	GX_GEQUAL,

	GX_ALWAYS
} GXCompare;

// [SPQE7T]/ISpyD.elf:.debug_info::0x268cce
typedef enum _GXCompCnt
{
	GX_POS_XY = 0,
	GX_POS_XYZ,

	GX_NRM_XYZ = 0,
	GX_NRM_NBT,
	GX_NRM_NBT3,

	GX_CLR_RGB = 0,
	GX_CLR_RGBA,

	GX_TEX_S = 0,
	GX_TEX_ST,
} GXCompCnt;

// [SPQE7T]/ISpyD.elf:.debug_info::0x25e1b3
typedef enum _GXCompType
{
	GX_U8,
	GX_S8,
	GX_U16,
	GX_S16,
	GX_F32,

	GX_RGB565 = 0,
	GX_RGB8,
	GX_RGBX8,
	GX_RGBA4,
	GX_RGBA6,
	GX_RGBA8,
} GXCompType;

// [SPQE7T]/ISpyD.elf:.debug_info::0x286e6d
typedef enum _GXCullMode
{
	GX_CULL_NONE,
	GX_CULL_FRONT,
	GX_CULL_BACK,
	GX_CULL_ALL
} GXCullMode;

// [SJXE41]/LynWiiRetail.elf:.debug_info::0xd838
typedef enum _GXDiffuseFn
{
	GX_DF_NONE,

	GX_DF_SIGN,
	GX_DF_CLAMP,
} GXDiffuseFn;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48fc70
typedef enum _GXIndTexAlphaSel
{
	GX_ITBA_OFF,

	GX_ITBA_S,
	GX_ITBA_T,
	GX_ITBA_U,

	GX_MAX_ITBALPHA
} GXIndTexAlphaSel;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48fb29
typedef enum _GXIndTexBiasSel
{
	GX_ITB_NONE,

	GX_ITB_S,
	GX_ITB_T,
	GX_ITB_ST,
	GX_ITB_U,
	GX_ITB_SU,
	GX_ITB_TU,
	GX_ITB_STU,

	GX_MAX_ITBIAS
} GXIndTexBiasSel;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48fac0
typedef enum _GXIndTexFormat
{
	GX_ITF_8,
	GX_ITF_5,
	GX_ITF_4,
	GX_ITF_3,

	GX_MAX_ITFORMAT
} GXIndTexFormat;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48ef62
typedef enum _GXIndTexMtxID
{
	GX_ITM_OFF,

	GX_ITM_0,
	GX_ITM_1,
	GX_ITM_2,

	GX_ITM_S0 = 5,
	GX_ITM_S1,
	GX_ITM_S2,

	GX_ITM_T0 = 9,
	GX_ITM_T1,
	GX_ITM_T2,
} GXIndTexMtxID;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x490652
typedef enum _GXIndTexScale
{
	GX_ITS_1,
	GX_ITS_2,
	GX_ITS_4,
	GX_ITS_8,
	GX_ITS_16,
	GX_ITS_32,
	GX_ITS_64,
	GX_ITS_128,
	GX_ITS_256,

	GX_MAX_ITSCALE
} GXIndTexScale;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48fa37
typedef enum _GXIndTexStageID
{
	GX_INDTEXSTAGE0,
	GX_INDTEXSTAGE1,
	GX_INDTEXSTAGE2,
	GX_INDTEXSTAGE3,

	GX_MAX_INDTEXSTAGE
} GXIndTexStageID;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48fbd5
typedef enum _GXIndTexWrap
{
	GX_ITW_OFF,

	GX_ITW_256,
	GX_ITW_128,
	GX_ITW_64,
	GX_ITW_32,
	GX_ITW_16,
	GX_ITW_0,

	GX_MAX_ITWRAP
} GXIndTexWrap;

// [SPQE7T]/ISpyD.elf:.debug_info::0x1eeb49
typedef enum _GXLightID
{
	GX_LIGHT0		= 1 << 0,
	GX_LIGHT1		= 1 << 1,
	GX_LIGHT2		= 1 << 2,
	GX_LIGHT3		= 1 << 3,
	GX_LIGHT4		= 1 << 4,
	GX_LIGHT5		= 1 << 5,
	GX_LIGHT6		= 1 << 6,
	GX_LIGHT7		= 1 << 7,

	GX_MAX_LIGHT	= 1 << 8,
	GX_LIGHT_NULL	= 0
} GXLightID;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48f6dc
typedef enum _GXLogicOp
{
	GX_LO_CLEAR,

	GX_LO_AND,
	GX_LO_REVAND,
	GX_LO_COPY,
	GX_LO_INVAND,
	GX_LO_NOOP,
	GX_LO_XOR,
	GX_LO_OR,
	GX_LO_NOR,
	GX_LO_EQUIV,
	GX_LO_INV,
	GX_LO_REVOR,
	GX_LO_INVCOPY,
	GX_LO_INVOR,
	GX_LO_NAND,

	GX_LO_SET
} GXLogicOp;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x455fb4
typedef enum _GXPrimitive
{
	GX_POINTS			= 0xb8, // 0b1011'1000

	GX_LINES			= 0xa8, // 0b1010'1000
	GX_LINESTRIP		= 0xb0, // 0b1011'0000

	GX_TRIANGLES		= 0x90, // 0x1001'0000
	GX_TRIANGLESTRIP	= 0x98, // 0x1001'1000
	GX_TRIANGLEFAN		= 0xa0, // 0x1010'0000

	GX_QUADS			= 0x80, // 0x1000'0000
} GXPrimitive;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x490518
typedef enum _GXTevAlphaArg
{
	GX_CA_APREV,

	GX_CA_A0,
	GX_CA_A1,
	GX_CA_A2,

	GX_CA_TEXA,
	GX_CA_RASA,

	GX_CA_KONST,
	GX_CA_ZERO,
	GX_CA_ONE,
} GXTevAlphaArg;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48fe17
typedef enum _GXTevBias
{
	GX_TB_ZERO,
	GX_TB_ADDHALF,
	GX_TB_SUBHALF,

	GX_MAX_TEVBIAS
} GXTevBias;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x162cf3
typedef enum _GXTevColorArg
{
	GX_CC_CPREV,
	GX_CC_APREV,

	GX_CC_C0,
	GX_CC_A0,
	GX_CC_C1,
	GX_CC_A1,
	GX_CC_C2,
	GX_CC_A2,

	GX_CC_TEXC,
	GX_CC_TEXA,
	GX_CC_RASC,
	GX_CC_RASA,

	GX_CC_ONE,
	GX_CC_HALF,
	GX_CC_KONST,
	GX_CC_ZERO,

	GX_CC_TEXRRR,
	GX_CC_TEXGGG,
	GX_CC_TEXBBB,

	GX_CC_QUARTER	= GX_CC_KONST,
} GXTevColorArg;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48f87f
typedef enum _GXTevColorChan
{
	GX_CH_RED,
	GX_CH_GREEN,
	GX_CH_BLUE,
	GX_CH_ALPHA
} GXTevColorChan;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x490211
typedef enum _GXTevKColorSel
{
	GX_TEV_KCSEL_8_8,
	GX_TEV_KCSEL_7_8,
	GX_TEV_KCSEL_6_8,
	GX_TEV_KCSEL_5_8,
	GX_TEV_KCSEL_4_8,
	GX_TEV_KCSEL_3_8,
	GX_TEV_KCSEL_2_8,
	GX_TEV_KCSEL_1_8,

	GX_TEV_KCSEL_1 = GX_TEV_KCSEL_8_8,
	GX_TEV_KCSEL_3_4 = GX_TEV_KCSEL_6_8,
	GX_TEV_KCSEL_1_2 = GX_TEV_KCSEL_4_8,
	GX_TEV_KCSEL_1_4 = GX_TEV_KCSEL_2_8,

	GX_TEV_KCSEL_K0 = 12,
	GX_TEV_KCSEL_K1,
	GX_TEV_KCSEL_K2,
	GX_TEV_KCSEL_K3,

	GX_TEV_KCSEL_K0_R,
	GX_TEV_KCSEL_K1_R,
	GX_TEV_KCSEL_K2_R,
	GX_TEV_KCSEL_K3_R,
	GX_TEV_KCSEL_K0_G,
	GX_TEV_KCSEL_K1_G,
	GX_TEV_KCSEL_K2_G,
	GX_TEV_KCSEL_K3_G,
	GX_TEV_KCSEL_K0_B,
	GX_TEV_KCSEL_K1_B,
	GX_TEV_KCSEL_K2_B,
	GX_TEV_KCSEL_K3_B,
	GX_TEV_KCSEL_K0_A,
	GX_TEV_KCSEL_K1_A,
	GX_TEV_KCSEL_K2_A,
	GX_TEV_KCSEL_K3_A,
} GXTevKColorSel;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48ff5c2
typedef enum _GXTevKAlphaSel
{
	GX_TEV_KASEL_8_8,
	GX_TEV_KASEL_7_8,
	GX_TEV_KASEL_6_8,
	GX_TEV_KASEL_5_8,
	GX_TEV_KASEL_4_8,
	GX_TEV_KASEL_3_8,
	GX_TEV_KASEL_2_8,
	GX_TEV_KASEL_1_8,

	GX_TEV_KASEL_1 = GX_TEV_KASEL_8_8,
	GX_TEV_KASEL_3_4 = GX_TEV_KASEL_6_8,
	GX_TEV_KASEL_1_2 = GX_TEV_KASEL_4_8,
	GX_TEV_KASEL_1_4 = GX_TEV_KASEL_2_8,

	GX_TEV_KASEL_K0_R = 16,
	GX_TEV_KASEL_K1_R,
	GX_TEV_KASEL_K2_R,
	GX_TEV_KASEL_K3_R,
	GX_TEV_KASEL_K0_G,
	GX_TEV_KASEL_K1_G,
	GX_TEV_KASEL_K2_G,
	GX_TEV_KASEL_K3_G,
	GX_TEV_KASEL_K0_B,
	GX_TEV_KASEL_K1_B,
	GX_TEV_KASEL_K2_B,
	GX_TEV_KASEL_K3_B,
	GX_TEV_KASEL_K0_A,
	GX_TEV_KASEL_K1_A,
	GX_TEV_KASEL_K2_A,
	GX_TEV_KASEL_K3_A
} GXTevKAlphaSel;

// NOTE: older GX version (Feb 24 2009 00:57:49)
// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x107e0
typedef enum _GXTevKColorID
{
	GX_KCOLOR0,
	GX_KCOLOR1,
	GX_KCOLOR2,
	GX_KCOLOR3,

	GX_MAX_KCOLOR
} GXTevKColorID;

// [SPQE7T]/ISpyD.elf:.debug_info::0x1f105e
typedef enum _GXTevMode
{
	GX_MODULATE,
	GX_DECAL,
	GX_BLEND,
	GX_REPLACE,
	GX_PASSCLR,
} GXTevMode;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48fce7
typedef enum _GXTevOp
{
	GX_TEV_ADD,
	GX_TEV_SUB,

	GX_TEV_COMP_R8_GT = 8,
	GX_TEV_COMP_R8_EQ,
	GX_TEV_COMP_GR16_GT,
	GX_TEV_COMP_GR16_EQ,
	GX_TEV_COMP_BGR24_GT,
	GX_TEV_COMP_BGR24_EQ,
	GX_TEV_COMP_RGB8_GT,
	GX_TEV_COMP_RGB8_EQ,

	GX_TEV_COMP_A8_GT = GX_TEV_COMP_RGB8_GT,
	GX_TEV_COMP_A8_EQ = GX_TEV_COMP_RGB8_EQ,
} GXTevOp;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48fef1
typedef enum _GXTevRegID
{
	GX_TEVPREV,

	GX_TEVREG0,
	GX_TEVREG1,
	GX_TEVREG2,

	GX_MAX_TEVREG
} GXTevRegID;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48fe77
typedef enum _GXTevScale
{
	GX_TEV_SCALE_0,
	GX_TEV_SCALE_1,
	GX_TEV_SCALE_2,
	GX_TEV_SCALE_3,
} GXTevScale;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x163019
typedef enum _GXTevStageID
{
	GX_TEVSTAGE0,
	GX_TEVSTAGE1,
	GX_TEVSTAGE2,
	GX_TEVSTAGE3,
	GX_TEVSTAGE4,
	GX_TEVSTAGE5,
	GX_TEVSTAGE6,
	GX_TEVSTAGE7,
	GX_TEVSTAGE8,
	GX_TEVSTAGE9,
	GX_TEVSTAGE10,
	GX_TEVSTAGE11,
	GX_TEVSTAGE12,
	GX_TEVSTAGE13,
	GX_TEVSTAGE14,
	GX_TEVSTAGE15,

	GX_MAX_TEVSTAGE
} GXTevStageID;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x4905c4
typedef enum _GXTevSwapSel
{
	GX_TEV_SWAP0,
	GX_TEV_SWAP1,
	GX_TEV_SWAP2,
	GX_TEV_SWAP3,

	GX_MAX_TEVSWAP
} GXTevSwapSel;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x14a955
typedef enum _GXTexCoordID
{
	GX_TEXCOORD0,
	GX_TEXCOORD1,
	GX_TEXCOORD2,
	GX_TEXCOORD3,
	GX_TEXCOORD4,
	GX_TEXCOORD5,
	GX_TEXCOORD6,
	GX_TEXCOORD7,

	GX_MAX_TEXCOORD,
	GX_TEXCOORD_NULL = 0xff
} GXTexCoordID;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0xba2e6
typedef enum _GXTexFilter
{
	GX_NEAR,
	GX_LINEAR,

	GX_NEAR_MIP_NEAR,
	GX_LIN_MIP_NEAR,

	GX_NEAR_MIP_LIN,
	GX_LIN_MIP_LIN,
} GXTexFilter;

// [SPQE7T]/ISpyD.elf:.debug_info::0x28a5e5
typedef enum _GXTexFmt
{
	GX_TF_I4		=  0,
	GX_TF_I8		=  1,
	GX_TF_IA4		=  2,
	GX_TF_IA8		=  3,

	GX_TF_RGB565	=  4,
	GX_TF_RGB5A3	=  5,
	GX_TF_RGBA8		=  6,

	GX_TF_CMPR		= 14,

	GX_CTF_R4		= 32,
	GX_CTF_RA4		= 34,
	GX_CTF_RA8		= 35,
	GX_CTF_YUVA8	= 38,

	GX_CTF_A8		= 39,
	GX_CTF_R8		= 40,
	GX_CTF_G8		= 41,
	GX_CTF_B8		= 42,
	GX_CTF_RG8		= 43,
	GX_CTF_GB8		= 44,

	GX_TF_Z8		= 17,
	GX_TF_Z16		= 19,
	GX_TF_Z24X8		= 22,

	GX_CTF_Z4		= 48,
	GX_CTF_Z8M		= 57,
	GX_CTF_Z8L		= 58,
	GX_CTF_Z16L		= 60,

	GX_TF_A8		= GX_CTF_A8
} GXTexFmt;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x14ab0b
typedef enum _GXTexGenSrc
{
	GX_TG_POS,

	GX_TG_NRM,
	GX_TG_BINRM,
	GX_TG_TANGENT,

	GX_TG_TEX0,
	GX_TG_TEX1,
	GX_TG_TEX2,
	GX_TG_TEX3,
	GX_TG_TEX4,
	GX_TG_TEX5,
	GX_TG_TEX6,
	GX_TG_TEX7,
	GX_TG_TEXCOORD0,
	GX_TG_TEXCOORD1,
	GX_TG_TEXCOORD2,
	GX_TG_TEXCOORD3,
	GX_TG_TEXCOORD4,
	GX_TG_TEXCOORD5,
	GX_TG_TEXCOORD6,

	GX_TG_COLOR0,
	GX_TG_COLOR1
} GXTexGenSrc;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x14aa2f
typedef enum _GXTexGenType
{
	GX_TG_MTX3x4,
	GX_TG_MTX2x4,

	GX_TG_BUMP0,
	GX_TG_BUMP1,
	GX_TG_BUMP2,
	GX_TG_BUMP3,
	GX_TG_BUMP4,
	GX_TG_BUMP5,
	GX_TG_BUMP6,
	GX_TG_BUMP7,

	GX_TG_SRTG,
} GXTexGenType;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x2b93e8
typedef enum _GXTexMapID
{
	GX_TEXMAP0,
	GX_TEXMAP1,
	GX_TEXMAP2,
	GX_TEXMAP3,
	GX_TEXMAP4,
	GX_TEXMAP5,
	GX_TEXMAP6,
	GX_TEXMAP7,

	GX_MAX_TEXMAP,
	GX_TEXMAP_NULL	= 0xff,
	GX_TEX_DISABLE	= 256
} GXTexMapID;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0xba288
typedef enum _GXTexWrapMode
{
	GX_CLAMP,
	GX_REPEAT,
	GX_MIRROR,

	GX_MAX_TEXWRAPMODE
} GXTexWrapMode;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0xba3e2
typedef enum _GXTlutFmt
{
	GX_TL_IA8,
	GX_TL_RGB565,
	GX_TL_RGB5A3,

	GX_MAX_TLUTFMT
} GXTlutFmt;

// [SPQE7T]/ISpyD.elf:.debug_info::0x2689ea
typedef enum _GXVtxFmt
{
	GX_VTXFMT0,
	GX_VTXFMT1,
	GX_VTXFMT2,
	GX_VTXFMT3,
	GX_VTXFMT4,
	GX_VTXFMT5,
	GX_VTXFMT6,
	GX_VTXFMT7,

	GX_MAX_VTXFMT
} GXVtxFmt;

/* Patent US 6,664,958 B1
 * [2] https://patents.google.com/patent/US6664958B1
 * section 15, lines 20-25
 */
typedef enum _GXZTexOp
{
	GX_ZT_DISABLE,
	GX_ZT_ADD,
	GX_ZT_REPLACE, // NOTE: misspelled as GZ_ZT_REPLACE in patent

	GX_MAX_ZTEXOP
} GXZTexOp;

/* Name:
 * Patent US 6,937,245 B1
 * https://patents.google.com/patent/US6937245B1
 * section 13, line 4
 */
typedef u8 GXBool;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0xba05c
typedef struct _GXColor
{
	u8	r;	// size 0x01, offset 0x00
	u8	g;	// size 0x01, offset 0x01
	u8	b;	// size 0x01, offset 0x02
	u8	a;	// size 0x01, offset 0x03
} GXColor; // size 0x04

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x4764f4
typedef struct _GXColorS10
{
	s16	r;	// size 0x02, offset 0x00
	s16	g;	// size 0x02, offset 0x02
	s16	b;	// size 0x02, offset 0x04
	s16	a;	// size 0x02, offset 0x06
} GXColorS10; // size 0x08

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x2a52c2
typedef struct _GXTexObj
{
	// i guess they didn't want to expose the internals
	byte4_t dummy[8];
} GXTexObj; // size 0x20

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x2b931e
typedef struct _GXTlutObj
{
	byte4_t	dummy[3];
} GXTlutObj; // size 0x0c

void GXClearVtxDesc(void);
int GXGetTexObjFmt(GXTexObj *);
u16 GXGetTexObjHeight(GXTexObj *);
u16 GXGetTexObjWidth(GXTexObj *);
GXTexWrapMode GXGetTexObjWrapS(GXTexObj *);
GXTexWrapMode GXGetTexObjWrapT(GXTexObj *);
void *GXGetTexObjUserData(GXTexObj *);
void GXInitTexObj(GXTexObj *, void *, u16, u16, GXTexFmt, GXTexWrapMode, GXTexWrapMode, GXBool);
void GXInitTexObjCI(GXTexObj *, void *, u16, u16, GXTexFmt, GXTexWrapMode,
                    GXTexWrapMode, GXBool, u32);
void GXInitTexObjLOD(GXTexObj *, GXTexFilter, GXTexFilter,
                     f32, f32, f32, GXBool,
                     GXBool, GXAnisotropy);
void GXInitTexObjTlut(GXTexObj *, u32);
void GXInitTexObjWrapMode(GXTexObj *, GXTexWrapMode, GXTexWrapMode);
void GXInitTexObjUserData(GXTexObj *, void *);
void GXInitTlutObj(GXTlutObj *, void *, GXTlutFmt, u16);
void GXLoadPosMtxImm(Mtx, int);
void GXLoadTexMtxImm(const Mtx, u32, GXTexGenType);
void GXLoadTexObj(GXTexObj *, int);
void GXLoadTlut(GXTlutObj *, u32);
void GXSetAlphaCompare(GXCompare, u8, GXAlphaOp, GXCompare, u8);
void GXSetAlphaUpdate(GXBool);
void GXSetBlendMode(GXBlendMode, GXBlendFactor, GXBlendFactor, GXLogicOp);
void GXSetChanCtrl(GXChannelID, GXBool, GXColorSrc, GXColorSrc, GXLightID, GXDiffuseFn, GXAttnFn);
void GXSetChanMatColor(GXChannelID, GXColor);
void GXSetCullMode(GXCullMode);
void GXSetCurrentMtx(int);
void GXSetDispCopyGamma(int);
void GXSetFog(int, f32, f32, f32, f32, GXColor);
void GXSetIndTexCoordScale(GXIndTexStageID, GXIndTexScale, GXIndTexScale);
void GXSetIndTexMtx(GXIndTexMtxID, const Mtx23, s8);
void GXSetIndTexOrder(GXIndTexStageID, GXTexCoordID, GXTexMapID);
void GXSetLineWidth(u8, int);
void GXSetNumChans(u8);
void GXSetNumIndStages(u8);
void GXSetNumTevStages(u8);
void GXSetNumTexGens(u8);
void GXSetTevAlphaIn(GXTevStageID, GXTevAlphaArg, GXTevAlphaArg, GXTevAlphaArg, GXTevAlphaArg);
void GXSetTevAlphaOp(GXTevStageID, GXTevOp, GXTevBias, GXTevScale, u8, GXTevRegID);
void GXSetTevColor(GXTevRegID, GXColor);
void GXSetTevColorIn(GXTevStageID, GXTevColorArg, GXTevColorArg, GXTevColorArg, GXTevColorArg);
void GXSetTevColorOp(GXTevStageID, GXTevOp, GXTevBias, GXTevScale, u8, GXTevRegID);
void GXSetTevColorS10(GXTevRegID, GXColorS10);
void GXSetTevDirect(GXTevStageID);
void GXSetTevIndirect(GXTevStageID, GXIndTexStageID, GXIndTexFormat, GXIndTexBiasSel, GXIndTexMtxID, GXIndTexWrap, GXIndTexWrap, GXBool, GXBool, GXIndTexAlphaSel);
void GXSetTevKAlphaSel(GXTevStageID, GXTevKAlphaSel);
void GXSetTevKColor(GXTevKColorID, GXColor);
void GXSetTevKColorSel(GXTevStageID, GXTevKColorSel);
void GXSetTevOp(GXTevStageID, GXTevMode);
void GXSetTevOrder(GXTevStageID, GXTexCoordID, GXTexMapID, GXChannelID);
void GXSetTevSwapMode(GXTevStageID, GXTevSwapSel, GXTevSwapSel);
void GXSetTevSwapModeTable(GXTevSwapSel, GXTevColorChan, GXTevColorChan, GXTevColorChan, GXTevColorChan);
void GXSetTexCoordCylWrap(int, u8, u8);
void GXSetTexCoordGen2(GXTexCoordID, GXTexGenType, GXTexGenSrc, u32, GXBool, u32);
void GXSetTexCoordScaleManually(int, u8, u16, u16);
void GXSetVtxAttrFmt(GXVtxFmt, GXAttr, GXCompCnt, GXCompType, u8);
void GXSetVtxDesc(GXAttr, GXAttrType);
void GXSetZMode(GXBool, GXCompare, GXBool);
void GXSetZTexture(GXZTexOp op, GXTexFmt fmt, u32 bias);

// ---

static inline void GXSetTexCoordGen(GXTexCoordID dst_coord, GXTexGenType func,
                                    GXTexGenSrc src_param, u32 mtx)
{
	GXSetTexCoordGen2(dst_coord, func, src_param, mtx, FALSE, 0x7d);
}

void GXBegin(GXPrimitive, GXVtxFmt, u16);

static inline void GXEnd(void)
{
#if !defined(NDEBUG)
	extern GXBool __GXinBegin;

	if (!__GXinBegin)
	{
		OSError_FileLine("GXGeometry.h", 127,
		                 "GXEnd: called without a GXBegin");
	}

	__GXinBegin = FALSE;
#endif // !defined(NDEBUG)
}

union WGPipe
{
	u16 u16;
	u32 u32;
	f32 f32;
};

extern volatile union WGPipe __WGPipe ATTR_ADDRESS(0xcc008000);

inline void GXPosition2f32(f32 x, f32 y)
{
	__WGPipe.f32 = x;
	__WGPipe.f32 = y;
}

inline void GXPosition3f32(f32 x, f32 y, f32 z)
{
	__WGPipe.f32 = x;
	__WGPipe.f32 = y;
	__WGPipe.f32 = z;
}

inline void GXColor1u32(u32 color)
{
	__WGPipe.u32 = color;
}

inline void GXTexCoord2u16(u16 u, u16 v)
{
	__WGPipe.u16 = u;
	__WGPipe.u16 = v;
}

inline void GXTexCoord2f32(f32 u, f32 v)
{
	__WGPipe.f32 = u;
	__WGPipe.f32 = v;
}

typedef struct MEMiHeapHead MEMiHeapHead;
typedef struct MEMAllocator MEMAllocator;

// [SPQE7T]/ISpyD.elf:.debug_info::0x113386 (type only)
typedef void *MEMAllocatorAllocFunc(MEMAllocator *pAllocator, u32 size);

// [SPQE7T]/ISpyD.elf:.debug_info::0x1133bb (type only)
typedef void MEMAllocatorFreeFunc(MEMAllocator *pAllocator, void *memBlock);

// [SPQE7T]/ISpyD.elf:.debug_info::0x113345
typedef struct MEMAllocatorFunc
{
	MEMAllocatorAllocFunc	*pfAlloc;	// size 0x04, offset 0x00
	MEMAllocatorFreeFunc	*pfFree;	// size 0x04, offset 0x04
} MEMAllocatorFunc; // size 0x08

// [SPQE7T]/ISpyD.elf:.debug_info::0x1132e1
struct MEMAllocator
{
	const MEMAllocatorFunc	*pFunc;		// size 0x04, offset 0x00
	MEMiHeapHead			*pHeap;		// size 0x04, offset 0x04
	u32						heapParam1;	// size 0x04, offset 0x08
	u32						heapParam2;	// size 0x04, offset 0x0c
}; // size 0x10

void *MEMAllocFromAllocator(MEMAllocator *allocator, u32 size);
void MEMFreeToAllocator(MEMAllocator *allocator, void *block);
void MEMInitAllocatorForExpHeap(MEMAllocator *allocator, MEMiHeapHead *heap,
                                s32 align);

MEMiHeapHead *MEMCreateExpHeapEx(void *start, u32 size, u16 opt);
MEMiHeapHead *MEMDestroyExpHeap(MEMiHeapHead *heap);
static inline MEMiHeapHead *MEMCreateExpHeap(void *start, u32 size)
{
	return MEMCreateExpHeapEx(start, size, 0);
}

typedef u32 SCStatus;
enum SCStatus_et
{
	SC_STATUS_OK,	/* name known from asserts */
	SC_STATUS_BUSY,
	SC_STATUS_FATAL,
	SC_STATUS_PARSE,
};

typedef u8 SCSoundMode;
enum SCSoundMode_et
{
	SC_SND_MONO,
	SC_SND_STEREO,
	SC_SND_SURROUND
};

typedef void SCFlushCallback(SCStatus status);

void SCInit(void);
SCStatus SCCheckStatus(void);

SCSoundMode SCGetSoundMode(void);

// [SGLEA4]/GormitiDebug.elf:.debug_info::0xba189
typedef struct TPLHeader
{
	u16				height;			// size 0x02, offset 0x00
	u16				width;			// size 0x02, offset 0x02
	u32				format;			// size 0x04, offset 0x04
	char			*data;			// size 0x04, offset 0x08
	GXTexWrapMode	wrapS;			// size 0x04, offset 0x0c
	GXTexWrapMode	wrapT;			// size 0x04, offset 0x10
	GXTexFilter		minFilter;		// size 0x04, offset 0x14
	GXTexFilter		magFilter;		// size 0x04, offset 0x18
	f32				LODBias;		// size 0x04, offset 0x1c
	u8				edgeLODEnable;	// size 0x01, offset 0x20
	u8				minLOD;			// size 0x01, offset 0x21
	u8				maxLOD;			// size 0x01, offset 0x22
	u8				unpacked;		// size 0x01, offset 0x23
} TPLHeader; // size 0x24

// [SGLEA4]/GormitiDebug.elf:.debug_info::0xba376
typedef struct TPLClutHeader
{
	u16			numEntries;	// size 0x02, offset 0x00
	u8			unpacked;	// size 0x01, offset 0x02
	byte1_t 	pad8;
	GXTlutFmt	format;		// size 0x04, offset 0x04
	char		*data;		// size 0x04, offset 0x08
} TPLClutHeader; // size 0x0c

// [SGLEA4]/GormitiDebug.elf:.debug_info::0xba141
typedef struct TPLDescriptor
{
	TPLHeader		*textureHeader;	// size 0x04, offset 0x00
	TPLClutHeader	*CLUTHeader;	// size 0x04, offset 0x04
} TPLDescriptor; // size 0x08

// [SGLEA4]/GormitiDebug.elf:.debug_info::0xba0de
typedef struct TPLPalette
{
	u32				versionNumber;		// size 0x04, offset 0x00
	u32				numDescriptors;		// size 0x04, offset 0x04
	TPLDescriptor	*descriptorArray;	// size 0x04, offset 0x08
} TPLPalette; // size 0x0c

void TPLBind(TPLPalette *pal);
TPLDescriptor *TPLGet(TPLPalette *pal, u32 id);

void VISetBlack(BOOL black);
void VIFlush(void);

typedef s32 WPADResult;
enum WPADResult_et
{
	WPAD_ERR_OK						= 0,

	WPAD_ERR_NO_CONTROLLER			= -1,	/* name known from asserts */
	WPAD_ERR_COMMUNICATION_ERROR	= -2,	// [RT3P54] has this as WPAD_ERR_BUSY
	WPAD_ERR_3						= -3,	// [RT3P54] has this as WPAD_ERR_TRANSFER
	WPAD_ERR_INVALID				= -4,	/* name comes from [R89JEL] */
//	WPAD_ERR_5						= -5,	/* unknown */
//	WPAD_ERR_6						= -6,	/* unknown */
	WPAD_ERR_CORRUPTED				= -7,	/* name comes from [R89JEL] */

#define WPAD_ESUCCESS	WPAD_ERR_OK
#define WPAD_ENODEV		WPAD_ERR_NO_CONTROLLER
#define WPAD_ECOMM		WPAD_ERR_COMMUNICATION_ERROR
// #define WPAD_E3
#define WPAD_EINVAL		WPAD_ERR_INVALID
#define WPAD_EBADE		WPAD_ERR_CORRUPTED

#define WPAD_CESUCCESS	(WPAD_ESUCCESS + 0)
#define WPAD_CENODEV	(WPAD_ENODEV + 0)
#define WPAD_CECOMM		(WPAD_ECOMM + 0)
#define WPAD_CERR_3		(WPAD_ERR_3 + 0)
#define WPAD_CEINVAL	(WPAD_EINVAL + 0)
#define WPAD_CEBADE		(WPAD_EBADE + 0)
};

typedef s32 WPADChannel;
enum WPADChannel_et
{
	WPAD_CHAN0,
	WPAD_CHAN1,
	WPAD_CHAN2,
	WPAD_CHAN3,				/* name known from asserts */

	WPAD_MAX_CONTROLLERS,	/* name known from asserts */
	WPAD_CHAN_INVALID		= -1
};

typedef u32 WPADDeviceType;
enum WPADDeviceType_et
{
	WPAD_DEV_CORE				=  0,
	WPAD_DEV_FS					=  1,
	WPAD_DEV_CLASSIC			=  2,	/* name known from asserts, but not value */
	WPAD_DEV_BALANCE_CHECKER	=  3,
	WPAD_DEV_VSM				=  4,

	WPAD_DEV_MOTION_PLUS		=  5,
	WPAD_DEV_MPLS_PT_FS			=  6,
	WPAD_DEV_MPLS_PT_CLASSIC	=  7,

	WPAD_DEV_TRAIN				= 16,
	WPAD_DEV_GUITAR				= 17,
	WPAD_DEV_DRUM				= 18,
	WPAD_DEV_TAIKO				= 19,
	WPAD_DEV_TURNTABLE			= 20,

	// seems to be like maybe general purpose non-specific device types
	// maybe this was for testing or something? idk
	WPAD_DEV_BULK_1				= 21,
	WPAD_DEV_BULK_2				= 22,
	WPAD_DEV_BULK_3				= 23,
	WPAD_DEV_BULK_4				= 24,
	WPAD_DEV_BULK_5				= 25,
	WPAD_DEV_BULK_6				= 26,
	WPAD_DEV_BULK_7				= 27,
	WPAD_DEV_BULK_8				= 28,

	WPAD_DEV_MPLS_PT_UNKNOWN	= 250,

	WPAD_DEV_251				= 251,
	WPAD_DEV_252				= 252,	// invalid device mode?
	WPAD_DEV_NONE				= 253,	// sort of like WPAD_ENODEV (see __wpadAbortInitExtension in WPADHIDParser.c)
//	WPAD_DEV_254				= 254,	/* unknown, doesn't seem to be used anywhere */
	WPAD_DEV_INITIALIZING		= 255,	// see __a1_20_status_report
};

// names from wiiuse/wpad.h
typedef u16 WPADButton;
enum WPADButton_et
{
//  H..-AB12 ...+^v><
	/* NOTE: the bytes from the report are swapped when being placed into this
	 * format (see MAKE_BUTTON in WPADHIDParser.c)
	 */
	WPAD_BUTTON_LEFT	= (1 <<  0),
	WPAD_BUTTON_RIGHT	= (1 <<  1),
	WPAD_BUTTON_DOWN	= (1 <<  2),
	WPAD_BUTTON_UP		= (1 <<  3),
	WPAD_BUTTON_PLUS	= (1 <<  4),
	//					  (1 <<  5),
	//					  (1 <<  6),
	//					  (1 <<  7),
	WPAD_BUTTON_2		= (1 <<  8),
	WPAD_BUTTON_1		= (1 <<  9),
	WPAD_BUTTON_B		= 1 << 10,
	WPAD_BUTTON_A		= 1 << 11,
	WPAD_BUTTON_MINUS	= 1 << 12,
	//					  1 << 13,	// WPAD_BUTTON_FS_Z
	//					  1 << 14,	// WPAD_BUTTON_FS_C
	WPAD_BUTTON_HOME	= 1 << 15,

	WPAD_BUTTON_ALL		= 0x9f1f
};

// names from wiiuse/wpad.h
typedef u16 WPADExtButton;
enum WPADExtButton_et
{
	// Nunchuk (fight stick?)
//  .CZ..... ........
	/* NOTE: these bits are actually in the normal button variable, but are
	 * enumerated here because they are buttons for an extension
	 */
	WPAD_BUTTON_FS_Z		= 1 << 13,
	WPAD_BUTTON_FS_C		= 1 << 14,

	WPAD_BUTTON_FS_ALL		= 0x6000,

	// Classic Controller
//  >vL-H+R. lBYAXr<^
	WPAD_BUTTON_CL_UP		= (1 <<  0),
	WPAD_BUTTON_CL_LEFT		= (1 <<  1),
	WPAD_BUTTON_CL_ZR		= (1 <<  2),
	WPAD_BUTTON_CL_X		= (1 <<  3),
	WPAD_BUTTON_CL_A		= (1 <<  4),
	WPAD_BUTTON_CL_Y		= (1 <<  5),
	WPAD_BUTTON_CL_B		= (1 <<  6),
	WPAD_BUTTON_CL_ZL		= (1 <<  7),
	//						  (1 <<  8),
	WPAD_BUTTON_CL_FULL_R	= (1 <<  9),
	WPAD_BUTTON_CL_PLUS		= 1 << 10,
	WPAD_BUTTON_CL_HOME		= 1 << 11,
	WPAD_BUTTON_CL_MINUS	= 1 << 12,
	WPAD_BUTTON_CL_FULL_L	= 1 << 13,
	WPAD_BUTTON_CL_DOWN		= 1 << 14,
	WPAD_BUTTON_CL_RIGHT	= 1 << 15,

	WPAD_BUTTON_CL_ALL		= 0xfeff,

	// Shinkansen train controller
	// shares the same format as the Classic Controller
	// (see WPADiExcludeButton)
	WPAD_BUTTON_TR_UP		= WPAD_BUTTON_CL_UP,
	WPAD_BUTTON_TR_LEFT		= WPAD_BUTTON_CL_LEFT,
	WPAD_BUTTON_TR_ZR		= WPAD_BUTTON_CL_ZR,
	WPAD_BUTTON_TR_X		= WPAD_BUTTON_CL_X,
	WPAD_BUTTON_TR_A		= WPAD_BUTTON_CL_A,
	WPAD_BUTTON_TR_Y		= WPAD_BUTTON_CL_Y,
	WPAD_BUTTON_TR_B		= WPAD_BUTTON_CL_B,
	WPAD_BUTTON_TR_ZL		= WPAD_BUTTON_CL_ZL,
	//						= WPAD_BUTTON_CL_8,
	WPAD_BUTTON_TR_FULL_R	= WPAD_BUTTON_CL_FULL_R,
	WPAD_BUTTON_TR_PLUS		= WPAD_BUTTON_CL_PLUS,
	WPAD_BUTTON_TR_HOME		= WPAD_BUTTON_CL_HOME,
	WPAD_BUTTON_TR_MINUS	= WPAD_BUTTON_CL_MINUS,
	WPAD_BUTTON_TR_FULL_L	= WPAD_BUTTON_CL_FULL_L,
	WPAD_BUTTON_TR_DOWN		= WPAD_BUTTON_CL_DOWN,
	WPAD_BUTTON_TR_RIGHT	= WPAD_BUTTON_CL_RIGHT,

	WPAD_BUTTON_TR_ALL		= WPAD_BUTTON_CL_ALL,
};

// WPADControlMotor
typedef u32 WPADMotorCommand;
enum WPADMotorCommand_et
{
	WPAD_MOTOR_STOP		= 0,	/* name known from asserts */
	WPAD_MOTOR_RUMBLE	= 1,	/* name known from asserts */
};

// WPADControlSpeaker
typedef u32 WPADSpeakerCommand;
enum WPADSpeakerCommand_et
{
	WPAD_SPEAKER_DISABLE	= 0,
	WPAD_SPEAKER_ENABLE		= 1,	// might be ON? see HBMRemoteSpk.cpp
	WPAD_SPEAKER_MUTE		= 2,
	WPAD_SPEAKER_UNMUTE		= 3,
	WPAD_SPEAKER_PLAY		= 4,	// figured out from HBM usage

	// does the same thing as ENABLE? unless i'm missing something
	// not used so i don't know the context
	WPAD_SPEAKER_CMD_05		= 5,
};

typedef void WPADCallback(WPADChannel chan, WPADResult result);
typedef void WPADConnectCallback(WPADChannel chan, WPADResult result);
typedef void WPADExtensionCallback(WPADChannel chan, s32 devType);

typedef void WUDSyncDeviceCallback(s32 result, s32 num);
typedef WUDSyncDeviceCallback WPADSimpleSyncCallback;

// [SPQE7T]/ISpyD.elf:.debug_info::0xd675b
typedef struct WPADInfo
{
	BOOL	dpd;		// size 0x04, offset 0x00
	BOOL	speaker;	// size 0x04, offset 0x04
	BOOL	attach;		// size 0x04, offset 0x08
	BOOL	lowBat;		// size 0x04, offset 0x0c
	BOOL	nearempty;	// size 0x04, offset 0x10
	u8		battery;	// size 0x01, offset 0x14
	u8		led;		// size 0x01, offset 0x15
	u8		protocol;	// size 0x01, offset 0x16
	u8		firmware;	// size 0x01, offset 0x17
} WPADInfo; // size 0x18

BOOL WPADStartFastSimpleSync(void);
BOOL WPADStopSimpleSync(void);
WPADSimpleSyncCallback *WPADSetSimpleSyncCallback(WPADSimpleSyncCallback *cb);
u8 WPADGetRadioSensitivity(WPADChannel chan);
void WPADDisconnect(WPADChannel chan);
WPADResult WPADProbe(WPADChannel chan, WPADDeviceType *devTypeOut);
WPADConnectCallback *WPADSetConnectCallback(WPADChannel chan,
                                            WPADConnectCallback *cb);
WPADExtensionCallback *WPADSetExtensionCallback(WPADChannel chan,
                                                WPADExtensionCallback *cb);
WPADResult WPADGetInfoAsync(WPADChannel chan, WPADInfo *info, WPADCallback *cb);
void WPADControlMotor(WPADChannel chan, WPADMotorCommand command);
void WPADEnableMotor(BOOL enabled);
BOOL WPADIsMotorEnabled(void);
BOOL WPADSaveConfig(SCFlushCallback *cb);
BOOL WPADIsSpeakerEnabled(WPADChannel chan);
WPADResult WPADControlSpeaker(WPADChannel chan, WPADSpeakerCommand command,
                              WPADCallback *cb);
u8 WPADGetSpeakerVolume(void);
void WPADSetSpeakerVolume(u8 vol);
BOOL WPADCanSendStreamData(WPADChannel chan);
WPADResult WPADSendStreamData(WPADChannel chan, void *p_buf, u16 len);
BOOL WPADIsUsedCallbackByKPAD(void);
void WPADSetCallbackByKPAD(BOOL isKPAD);

// [SPQE7T]/ISpyD.elf:.debug_info::0x29f653
typedef struct WENCInfo
{
	byte_t	data[32];
} WENCInfo; // size 0x20

s32 WENCGetEncodeData(WENCInfo *info, u32 flag, const s16 *pbyPcmData,
                       s32 nSampleNum, byte_t *pbyAdpcmData);

// [SPQE7T]/ISpyD.elf:.debug_info::0xd66f9
typedef struct KPADMPDir
{
	Vec	X;	// size 0x0c, offset 0x00
	Vec	Y;	// size 0x0c, offset 0x0c
	Vec	Z;	// size 0x0c, offset 0x18
} KPADMPDir; // size 0x24

// [SPQE7T]/ISpyD.elf:.debug_info::0xd66b8
typedef struct KPADMPStatus
{
	Vec			mpls;	// size 0x0c, offset 0x00
	Vec			angle;	// size 0x0c, offset 0x0c
	KPADMPDir	dir;	// size 0x24, offset 0x18
} KPADMPStatus; // size 0x3c

// [SPQE7T]/ISpyD.elf:.debug_info::0xd650e
typedef union KPADEXStatus
{
	// [SPQE7T]/ISpyD.elf:.debug_info::0xd6549
	struct /* explicitly untagged */
	{
		Vec2	stick;		// size 0x08, offset 0x00
		Vec		acc;		// size 0x0c, offset 0x08
		f32		acc_value;	// size 0x04, offset 0x14
		f32		acc_speed;	// size 0x04, offset 0x18
	} fs; // size 0x1c

	// [SPQE7T]/ISpyD.elf:.debug_info::0xd659d
	struct /* explicitly untagged */
	{
		u32		hold;		// size 0x04, offset 0x00
		u32		trig;		// size 0x04, offset 0x04
		u32		release;	// size 0x04, offset 0x08
		Vec2	lstick;		// size 0x08, offset 0x0c
		Vec2	rstick;		// size 0x08, offset 0x14
		f32		ltrigger;	// size 0x04, offset 0x1c
		f32		rtrigger;	// size 0x04, offset 0x20
	} cl; // size 0x24

	// [SPQE7T]/ISpyD.elf:.debug_info::0xd6623
	struct /* explicitly untagged */
	{
		f64	tgc_weight;		// size 0x08, offset 0x00
		f64	weight[4];		// size 0x20, offset 0x08
		f64	weight_ave[4];	// size 0x20, offset 0x28
		s32	weight_err;		// size 0x04, offset 0x48
		s32	tgc_weight_err;	// size 0x04, offset 0x4c
	} bl; // size 0x50
} KPADEXStatus;

// [SPQE7T]/ISpyD.elf:.debug_info::0xd6300
typedef struct KPADStatus
{
	u32				hold;			// size 0x04, offset 0x00
	u32				trig;			// size 0x04, offset 0x04
	u32				release;		// size 0x04, offset 0x08
	Vec				acc;			// size 0x0c, offset 0x0c
	f32				acc_value;		// size 0x04, offset 0x18
	f32				acc_speed;		// size 0x04, offset 0x1c
	Vec2			pos;			// size 0x08, offset 0x20
	Vec2			vec;			// size 0x08, offset 0x28
	f32				speed;			// size 0x04, offset 0x30
	Vec2			horizon;		// size 0x08, offset 0x34
	Vec2			hori_vec;		// size 0x08, offset 0x3c
	f32				hori_speed;		// size 0x04, offset 0x44
	f32				dist;			// size 0x04, offset 0x48
	f32				dist_vec;		// size 0x04, offset 0x4c
	f32				dist_speed;		// size 0x04, offset 0x50
	Vec2			acc_vertical;	// size 0x08, offset 0x54
	u8				dev_type;		// size 0x01, offset 0x5c
	s8				wpad_err;		// size 0x01, offset 0x5d
	s8				dpd_valid_fg;	// size 0x01, offset 0x5e
	u8				data_format;	// size 0x01, offset 0x5f
	KPADEXStatus	ex_status;		// size 0x50, offset 0x60
	KPADMPStatus	mpls;			// size 0x3c, offset 0xb0
	byte1_t			__paddings__[4]; // ??? is this the compiler?
} KPADStatus; // size 0xf0

#ifdef __cplusplus
	}
#endif

#endif // CONTEXT_RVL_H
