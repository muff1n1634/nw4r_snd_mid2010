#ifndef NW4R_UT_DVD_FILE_STREAM_H
#define NW4R_UT_DVD_FILE_STREAM_H

/*******************************************************************************
 * headers
 */

#include <types.h> // s32

#include "FileStream.h"

/*******************************************************************************
 * types
 */

// forward declarations
namespace nw4r { namespace ut { namespace detail { class RuntimeTypeInfo; }}}

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace ut
{
	class DvdFileStream : public FileStream
	{
	public:
		virtual detail::RuntimeTypeInfo const *GetRuntimeTypeInfo() const
		{
			return &typeInfo;
		}

		void SetPriority(s32 priority) { mPriority = priority; }

	public:
		static detail::RuntimeTypeInfo const typeInfo;

	private:
		char pad[0x68 - 0x14];
		s32 mPriority;
	};
}} // namespace nw4r::ut

#endif // NW4R_UT_DVD_FILE_STREAM_H
