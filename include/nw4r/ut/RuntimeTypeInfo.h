#ifndef NW4R_UT_RUNTIME_TYPE_INFO_H
#define NW4R_UT_RUNTIME_TYPE_INFO_H

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
			RuntimeTypeInfo(RuntimeTypeInfo const *parent) :
				mParentTypeInfo(parent)
			{
			}

			// methods
			bool IsDerivedFrom(RuntimeTypeInfo const *typeInfo) const
			{
				RuntimeTypeInfo const *self = this;

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
			RuntimeTypeInfo	const *mParentTypeInfo;	// size 0x04, offset 0x00
		}; // size 0x04

		template <class T>
		RuntimeTypeInfo const *GetTypeInfoFromPtr_(T *)
		{
			return &T::typeInfo;
		}
	} // namespace detail

	template <typename U, class T>
	U DynamicCast(T *obj)
	{
		detail::RuntimeTypeInfo const *typeInfoU =
			detail::GetTypeInfoFromPtr_(static_cast<U>(nullptr));

		if (obj)
		{
			if (obj->GetRuntimeTypeInfo()->IsDerivedFrom(typeInfoU))
				return static_cast<U>(obj);
		}

		return nullptr;
	}
}} // namespace nw4r::ut

#endif // NW4R_UT_RUNTIME_TYPE_INFO_H
