#ifndef NW4R_SND_BANK_H
#define NW4R_SND_BANK_H

/*******************************************************************************
 * headers
 */

#include "BankFile.h" // BankFileReader

#include "nw4r/NW4RAssert.hpp"

/*******************************************************************************
 * classes and functions
 */

// forward declarations
namespace nw4r { namespace snd { namespace detail { class Channel; }}}
namespace nw4r { namespace snd { namespace detail { struct NoteOnInfo; }}}

namespace nw4r { namespace snd { namespace detail
{
	// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x2b0af4
	class Bank
	{
	// methods
	public:
		// cdtors
		Bank(const void *bankData);
		~Bank();

		// methods
		Channel *NoteOn(const NoteOnInfo &noteOnInfo) const;

		void SetWaveDataAddress(const void *waveData)
		{
			// specifically not the header variant
			NW4RAssertPointerNonnull_Line(50, waveData);

			mWaveDataAddress = waveData;
		}

	// members
	private:
		BankFileReader	mBankReader;		// size 0x0c, offset 0x00
		const void		*mWaveDataAddress;	// size 0x04, offset 0x0c
	}; // size 0x10
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_BANK_H
