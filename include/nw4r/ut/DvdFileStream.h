#ifndef RVL_SDK_NW4R_UT_DVD_FILE_STREAM_H
#define RVL_SDK_NW4R_UT_DVD_FILE_STREAM_H

/*******************************************************************************
 * headers
 */

#include <types.h>

#include "FileStream.h"
#include "RuntimeTypeInfo.h"

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace ut
{
	class DvdFileStream : public FileStream
	{
	public:
		virtual const detail::RuntimeTypeInfo *GetRuntimeTypeInfo() const
		{
			return &typeInfo;
		}

		void SetPriority(long priority) { mPriority = priority; }

	public:
		static const detail::RuntimeTypeInfo typeInfo;
	private:
		long mPriority;
	};
}} // namespace nw4hbm::ut

#endif // RVL_SDK_NW4R_UT_DVD_FILE_STREAM_H
