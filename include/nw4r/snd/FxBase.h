#ifndef NW4R_SND_FX_BASE_H
#define NW4R_SND_FX_BASE_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "global.h"

#include "../ut/inlines.h" // NonCopyable
#include "../ut/LinkList.h"

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd
{
	class FxBase : private ut::NonCopyable
	{
	// typedefs
	public:
		typedef ut::LinkList<FxBase, 4> LinkList;

	// methods
	public:
		virtual ~FxBase() {}

		// virtual function ordering
		// vtable FxBase
		virtual bool StartUp() { return true; }
		virtual void Shutdown() {}
		virtual void UpdateBuffer(int, void **, u32, SampleFormat, f32,
		                          OutputMode) {}
		virtual void OnChangeOutputMode() {}

	public:
		/* base ut::NonCopyable */	// size 0x00, offset 0x00
		/* vtable */				// size 0x04, offset 0x00
		ut::LinkListNode	node;	// size 0x08, offset 0x04
	}; // size 0x0c
}} // namespace nw4r::snd

#endif // NW4R_SND_FX_BASE_H
