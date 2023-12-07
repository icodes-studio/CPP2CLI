#pragma once

template <class T, int FixedBytes = 128>
class iBuffer
{
    private: T* buffer;
    private: size_t size;
    private: BYTE fixedBuffer[FixedBytes];

    public: iBuffer()
    {
        size = 0;
        buffer = NULL;
    }

    public: iBuffer(size_t elements)
    {
        size = 0;
        buffer = NULL;
        Allocate(elements);
    }

    public: ~iBuffer()
    {
        Release();
    }

    public: operator T* () const
    {
        return buffer;
    }


    public: T* Allocate(size_t elements)
    {
        size = elements * sizeof(T);
        return AllocateBytes(size);
    }

    public: T* Reallocate(size_t elements)
    {
        ASSERT(elements < size_t(-1) / sizeof(T));
        size = elements * sizeof(T);

        if (buffer == NULL)
            return AllocateBytes(size);

        if (size > FixedBytes)
        {
            if (buffer == reinterpret_cast<T*>(fixedBuffer))
            {
                AllocateHeap(size);
                memcpy_s(buffer, size, fixedBuffer, FixedBytes);
            }
            else
            {
                ReAllocateHeap(size);
            }
        }
        else
        {
            if (buffer != reinterpret_cast<T*>(fixedBuffer))
            {
                memcpy_s(fixedBuffer, FixedBytes, buffer, size);
                FreeHeap();
            }

            buffer = reinterpret_cast<T*>(fixedBuffer);
        }

        return buffer;
    }

    public: T* AllocateBytes(size_t bytes)
    {
        ASSERT(buffer == NULL);

        if (bytes > FixedBytes)
            AllocateHeap(bytes);
        else
            buffer = reinterpret_cast<T*>(fixedBuffer);

        return buffer;
    }

    public: void Attach(T* memory)
    {
        buffer = memory;
    }

    public: T* Detach()
    {
        T* temp = buffer;
        buffer = NULL;
        return temp;
    }

    public: void Release()
    {
        if (buffer != reinterpret_cast<T*>(fixedBuffer))
            FreeHeap();

        size = 0;
        buffer = NULL;
    }

    public: size_t Size()
    {
        return size;
    }

    public: BOOL operator ! () const
    {
        return (buffer == NULL);
    }

    public: BOOL operator != (int null) const
    {
        return !operator == (null);
    }

    public: BOOL operator == (int null) const
    {
        ASSERT(null == 0);
        return operator !();
    }

    public: BOOL operator == (decltype(__nullptr)) const
    {
        return operator == (0);
    }

    public: BOOL operator != (decltype(__nullptr)) const
    {
        return operator != (0);
    }

    private: void AllocateHeap(size_t bytes)
    {
        Attach(static_cast<T*>(malloc(bytes)));
    }

    private: void ReAllocateHeap(size_t bytes)
    {
        Attach(static_cast<T*>(realloc(buffer, bytes)));
    }

    private: void FreeHeap()
    {
        free(buffer);
    }
};
