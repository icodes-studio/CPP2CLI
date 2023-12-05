#include "pch.h"

template <> UINT HashKey <LPCSTR>(LPCSTR key)
{
    UINT hash = 0;

    while (*key)
        hash = (hash << 5) + hash + *key++;

    return hash;
}

template <> UINT HashKey <LPCWSTR>(LPCWSTR key)
{
    UINT hash = 0;

    while (*key)
        hash = (hash << 5) + hash + *key++;

    return hash;
}

template <> UINT HashKey <const iCString&>(const iCString& key)
{
    return HashKey <LPCTSTR>((LPCTSTR)key);
}

template <> void ConstructElements <iCString>(iCString* elements, int count)
{
    ASSERT(count == 0 || elements);

    for (; count--; ++elements)
        memcpy(elements, &iCString::GetEmptyString(), sizeof(*elements));
}
