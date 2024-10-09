#pragma once
#include "MemoryPool.h"

template<typename T>
class MPAllocator
{
public:
    using value_type = T;

    MPAllocator() { }

    template<class Other>
    MPAllocator(const MPAllocator<Other>&) {}

    T* allocate(size_t count)
    {
        const int32 size = static_cast<int32>(count * sizeof(T));
        return static_cast<T*>(c_alloc(size));
    }

    void deallocate(T* ptr, size_t count)
    {
        c_delocate(ptr);
    }

};

template<typename _Ty>
using cvector = std::vector<_Ty, class MPAllocator<_Ty>>;

template<typename _Ty, typename... Args>
std::shared_ptr<_Ty> Make_Shared(Args &&... args)
{
    return ::allocate_shared<_Ty, MPAllocator<_Ty>, Args...>(MPAllocator<_Ty>(), ::forward<Args>(args)...);
};