#ifndef NW4R_SND_SOUND_SYSTEM_H
#define NW4R_SND_SOUND_SYSTEM_H

/*******************************************************************************
 * headers
 */

#include <types.h>

/*******************************************************************************
 * classes and functions
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail { class TaskThread; }}}

namespace nw4r { namespace snd
{
	class SoundSystem
	{
	// nested types
	public:
		struct SoundSystemParam
		{
		// methods
		public:
			// cdtors
			SoundSystemParam() :
				soundThreadPriority		(4),
				soundThreadStackSize	(0x4000),
				dvdThreadPriority		(3),
				dvdThreadStackSize		(0x4000)
			{
			}

		// members
		public:
			s32	soundThreadPriority;	// size 0x04, offset 0x00
			u32	soundThreadStackSize;	// size 0x04, offset 0x04
			s32	dvdThreadPriority;		// size 0x04, offset 0x08
			u32	dvdThreadStackSize;		// size 0x04, offset 0x0c
		}; // size 0x10

	// static members
	private:
		static int sMaxVoices;
		static detail::TaskThread sTaskThread;

	// methods
	public:
		static void InitSoundSystem(const SoundSystemParam &param,
		                            void *workMem, u32 workMemSize);
		static bool IsInitializedSoundSystem();
		static void ShutdownSoundSystem();

		static u32 GetRequiredMemSize(const SoundSystemParam &param);
	}; // size 0x00
}} // namespace nw4r::snd

#endif // NW4R_SND_SOUND_SYSTEM_H
