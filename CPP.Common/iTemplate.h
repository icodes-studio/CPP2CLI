#pragma once

#ifndef __PLACEMENT_NEW_INLINE
#define __PLACEMENT_NEW_INLINE
inline void* __cdecl operator new(size_t, void* p)
{
    return (p);
}

inline void __cdecl operator delete(void*, void*)
{
    return;
}
#endif

#pragma push_macro("new")
#undef new
template <class TYPE> inline void ConstructElements(TYPE* elements, int count = 1)
{
    ASSERT(count == 0 || elements);

    memset((void*)elements, 0, count * sizeof(TYPE));

    for (; count--; ++elements)
        ::new((void*)elements) TYPE;
}
#pragma pop_macro("new")

template <class TYPE> inline void DestructElements(TYPE* elements, int count = 1)
{
    ASSERT(count == 0 || elements);

    for (; count--; ++elements)
        elements->~TYPE();
}

template <class TYPE> inline void CopyElements(TYPE* dest, const TYPE* source, int count = 1)
{
    ASSERT(count == 0 || dest);
    ASSERT(count == 0 || source);

    while (count--)
        *dest++ = *source++;
}

template <class TYPE, class ARG_TYPE> inline BOOL CompareElements(const TYPE* element1, const ARG_TYPE* element2)
{
    ASSERT(element1);
    ASSERT(element2);

    return *element1 == *element2;
}

template <class ARG_KEY> inline UINT HashKey(ARG_KEY key)
{
    return UINT(DWORD_PTR(key) >> 4);
}
