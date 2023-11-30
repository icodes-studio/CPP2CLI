#pragma once

class iAutoLock
{
    private: CRITICAL_SECTION* cs;
    private: BOOL locked;

    public: iAutoLock(CRITICAL_SECTION* cs, BOOL initialLock = TRUE) :
        cs(cs),
        locked(FALSE)
    {
        if (initialLock)
            Lock();
    };

    public: ~iAutoLock()
    {
        if (locked)
            Unlock();
    };

    public: void Lock()
    {
        ASSERT(!locked);
        EnterCriticalSection(cs);
        locked = TRUE;
    }

    public: void Unlock()
    {
        ASSERT(locked);
        LeaveCriticalSection(cs);
        locked = FALSE;
    }

    private: iAutoLock(const iAutoLock& r)
    {
        ASSERT(FALSE);
    }

    private: iAutoLock& operator = (const iAutoLock& r)
    {
        ASSERT(FALSE);
    }
};
