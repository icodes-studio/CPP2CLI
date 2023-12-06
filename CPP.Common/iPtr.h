#pragma once

template <class T>
class iPtr
{
    protected: T* pointer;
    protected: UINT* refs;

    public: iPtr()
    {
        pointer = NULL;
        refs = NULL;
    }

    public: iPtr(T* p)
    {
        pointer = p;
        refs = new UINT(1);
    }

    public: iPtr(const iPtr& p)
    {
        pointer = p.pointer;
        refs = p.refs;

        AddRef();
    }

    public: ~iPtr()
    {
        Release();
    }

    public: iPtr& operator = (const iPtr& p)
    {
        Release();

        pointer = p.pointer;
        refs = p.refs;

        AddRef();

        return *this;
    }

    public: iPtr& operator = (T* p)
    {
        Release();

        pointer = p;
        refs = new UINT(1);

        return *this;
    }

    public: operator T* () const
    {
        return pointer;
    }

    public: T& operator * () const
    {
        ASSERT(NULL != refs);
        return *pointer;
    }

    public: T* operator -> () const
    {
        ASSERT(NULL != refs);
        return pointer;
    }

    public: BOOL operator ! () const
    {
        return (pointer == NULL);
    }

    public: BOOL operator == (const iPtr& p) const
    {
        return (pointer == p.pointer);
    }

    public: BOOL operator == (T* p) const
    {
        return (pointer == p);
    }

    public: BOOL operator != (const iPtr& p) const
    {
        return (pointer != p.pointer);
    }

    public: BOOL operator != (T* p) const
    {
        return (pointer != p);
    }

    public: UINT GetRefCount() const
    {
        if (NULL != refs)
            return *refs;

        return 0;
    }

    public: void Release()
    {
        if (NULL != refs)
        {
            if (0 == --(*refs))
                Destroy();
        }

        pointer = NULL;
        refs = NULL;
    }

    public: UINT AddRef()
    {
        if (NULL != refs)
            return ++(*refs);

        return 0;
    }

    public: UINT ReleaseRef()
    {
        if (NULL != refs)
            return --(*refs);

        return 0;
    }

    protected: void Destroy()
    {
        SAFE_DELETE(pointer);
        SAFE_DELETE(refs);
    }
};
