#ifndef NW4R_SND_INSTANCE_POOL_H
#define NW4R_SND_INSTANCE_POOL_H

/*******************************************************************************
 * headers
 */

#include <new>

#include <types.h>

/*******************************************************************************
 * classes and functions
 */

namespace nw4r { namespace snd { namespace detail
{
	class PoolImpl
	{
	// methods
	protected:
		// cdtors
		PoolImpl(): mNext(nullptr) {}

		// methods
		u32 CreateImpl(void *buffer, u32 size, u32 stride);
		void DestroyImpl(void *buffer, u32 size);

		int CountImpl() const;

		void *AllocImpl();
		void FreeImpl(void *element);

	// members
	private:
		PoolImpl	*mNext;	// size 0x04, offset 0x00
	}; // size 0x04

	template <typename T>
	class InstancePool : private PoolImpl
	{
	// methods
	public:
		u32 Create(void *buffer, u32 size)
		{
			u32 objSize = sizeof(T);

			return CreateImpl(buffer, size, objSize);
		}

		void Destroy(void *ptr, u32 size) { DestroyImpl(ptr, size); }

		int Count() const { return CountImpl(); }

		T *Alloc()
		{
			void *ptr = AllocImpl();
			if (!ptr)
				return nullptr;

			return new (ptr) T;
		}

		void Free(T *obj)
		{
			if (obj)
			{
				obj->~T();
				FreeImpl(obj);
			}
		}

	// members
	private:
		/* base PoolImpl */	// size 0x04, offset 0x00
	}; // size 0x04

	template <class T>
	class MemoryPool : private PoolImpl
	{
	// methods
	public:
		u32 Create(void *buffer, u32 size)
		{
			return CreateImpl(buffer, size, sizeof(T));
		}

		void Destroy(void *buffer, u32 size) { DestroyImpl(buffer, size); }

		int Count() const { return CountImpl(); }

		T *Alloc() { return static_cast<T *>(AllocImpl()); }
		void Free(T *obj) { FreeImpl(obj); }

	// members
	private:
		/* base PoolImpl */	// size 0x04, offset 0x00
	}; // size 0x04
}}} // namespace nw4r::snd::detail

#endif // NW4R_SND_INSTANCE_POOL_H
