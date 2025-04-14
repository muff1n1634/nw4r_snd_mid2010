#ifndef NW4R_SND_SOUND_STARTABLE_H
#define NW4R_SND_SOUND_STARTABLE_H

/*******************************************************************************
 * headers
 */

#include <types.h>

/*******************************************************************************
 * classes and functions
 */

// forward declarations
namespace nw4r { namespace snd { class SoundHandle; }}

namespace nw4r { namespace snd
{
	// TODO: where does this go? (NOTE to future self: probably SeqSound.h)
	struct SeqSoundInfo
	{
		void		*seqDataAddress;		// size 0x04, offset 0x00
		const char	*startLocationLabel;	// size 0x04, offset 0x04
	}; // size 0x08

	class SoundStartable
	{
	// enums
	public:
		enum StartResult
		{
			START_SUCCESS,

			START_ERR_LOW_PRIORITY,
			START_ERR_INVALID_LABEL_STRING,
			START_ERR_INVALID_SOUNDID,
			START_ERR_NOT_DATA_LOADED,
			START_ERR_NOT_ENOUGH_PLAYER_HEAP,
			START_ERR_CANNOT_OPEN_FILE,
			START_ERR_NOT_AVAILABLE,
			START_ERR_CANNOT_ALLOCATE_TRACK,
			START_ERR_NOT_ENOUGH_INSTANCE,
			START_ERR_INVALID_PARAMETER,
			START_ERR_INVALID_SEQ_START_LOCATION_LABEL,

			START_ERR_USER = 128,
			START_ERR_UNKNOWN = 255,
		};

	// nested types
	public:
		struct StartInfo
		{
		// enums
		public:
			enum EnableFlag
			{
				ENABLE_START_OFFSET		= 1 << 0,
				ENABLE_PLAYER_ID		= 1 << 1,
				ENABLE_PLAYER_PRIORITY	= 1 << 2,
				ENABLE_ACTOR_PLAYER_ID	= 1 << 3,
				ENABLE_SEQ_SOUND_INFO	= 1 << 4,
			};

			enum StartOffsetType
			{
				START_OFFSET_TYPE_MILLISEC,
				START_OFFSET_TYPE_TICK,
				START_OFFSET_TYPE_SAMPLE,
			};

		// members
		public:
			u32				enableFlag;			// size 0x04, offset 0x00
			StartOffsetType	startOffsetType;	// size 0x04, offset 0x04
			int				startOffset;		// size 0x04, offset 0x08
			u32				playerId;			// size 0x04, offset 0x0c
			int				playerPriority;		// size 0x04, offset 0x10
			int				actorPlayerId;		// size 0x04, offset 0x14
			SeqSoundInfo	seqSoundInfo;		// size 0x08, offset 0x18
		}; // size 0x20

	// methods
	public:
		// cdtors
		virtual ~SoundStartable() {}

		// virtual function ordering
		// vtable SoundStartable
		virtual StartResult detail_SetupSound(SoundHandle *handle, u32 soundId,
		                                      bool holdFlag,
		                                      const StartInfo *startInfo) = 0;
		virtual u32 detail_ConvertLabelStringToSoundId(const char *label) = 0;

		// members
		StartResult detail_StartSound(SoundHandle *handle, u32 soundId,
		                              const StartInfo *startInfo);
		StartResult detail_HoldSound(SoundHandle *handle, u32 soundId,
		                             const StartInfo *startInfo);

	// members
	private:
		/* vtable */	// size 0x04, offset 0x00
	}; // size 0x04
}} // namespace nw4r::snd

#endif // NW4R_SND_SOUND_STARTABLE_H
