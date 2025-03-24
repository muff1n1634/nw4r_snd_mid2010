#ifndef RVL_SDK_NW4R_UT_RUNTIME_TYPE_INFO_H
#define RVL_SDK_NW4R_UT_RUNTIME_TYPE_INFO_H

/*******************************************************************************
 * headers
 */

#include <types.h>

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace ut
{
	namespace detail
	{
		class RuntimeTypeInfo
		{
		// methods
		public:
			// cdtors
			RuntimeTypeInfo(const RuntimeTypeInfo *parent):
				mParentTypeInfo	(parent)
				{}

			// methods
			bool IsDerivedFrom(const RuntimeTypeInfo *typeInfo) const
			{
				const RuntimeTypeInfo *self = this;

				while (self)
				{
					if (self == typeInfo)
						return true;

					self = self->mParentTypeInfo;
				}

				return false;
			}

		// members
		private:
			const RuntimeTypeInfo	*mParentTypeInfo; // offset 0x00, size 0x04
		}; // size 0x04

		template <class T>
		const RuntimeTypeInfo *GetTypeInfoFromPtr_(T *)
		{
			return &T::typeInfo;
		}
	} // namespace detail

	template <typename U, class T>
	U DynamicCast(T *obj)
	{
		const detail::RuntimeTypeInfo *typeInfoU =
			detail::GetTypeInfoFromPtr_(static_cast<U>(nullptr));

		if (obj)
		{
			if (obj->GetRuntimeTypeInfo()->IsDerivedFrom(typeInfoU))
				return static_cast<U>(obj);
		}

		return nullptr;
	}
}} // namespace nw4r::ut

#endif // RVL_SDK_NW4R_UT_RUNTIME_TYPE_INFO_H
