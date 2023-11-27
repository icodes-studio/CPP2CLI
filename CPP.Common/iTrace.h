#pragma once

#ifndef TRACE
    #ifdef _DEBUG
        #ifndef _NO_TRACE
            extern void __cdecl iTrace(LPCTSTR fmt, ...);

            #define IMPLEMENT_TRACE(x)              \
            void __cdecl iTrace(LPCTSTR fmt, ...)   \
            {                                       \
                va_list vl;                         \
                va_start(vl, fmt);                  \
                TCHAR buf[x + 1];                   \
                _vsntprintf_s(buf, x, fmt, vl);     \
                buf[x] = 0;                         \
                va_end(vl);                         \
                OutputDebugString(buf);             \
            }

            #define TRACE                   iTrace
            #define TRACE0(s)               iTrace(s)
            #define TRACE1(s, p1)           iTrace(s, p1)
            #define TRACE2(s, p1, p2)       iTrace(s, p1, p2)
            #define TRACE3(s, p1, p2, p3)   iTrace(s, p1, p2, p3)
            #define _TRACE(s)               iTrace(_T("%s(%d): %s"), __FILE__, __LINE__, s);
            #define _TRACE_ERROR(s)         iTrace(_T("[ERROR] %s(%d): %s\n"), __FILE__, __LINE__, s);
            #define _TRACE_WARNING(s)       iTrace(_T("[WARNING] %s(%d): %s\n"), __FILE__, __LINE__, s);
        #else
            #define IMPLEMENT_TRACE(x)
            #define TRACE                   (__noop)
            #define TRACE0(s)               (__noop)
            #define TRACE1(s, p1)           (__noop)
            #define TRACE2(s, p1, p2)       (__noop)
            #define TRACE3(s, p1, p2, p3)   (__noop)
            #define _TRACE(s)               (__noop)
            #define _TRACE_ERROR(s)         (__noop)
            #define _TRACE_WARNING(s)       (__noop)
        #endif
    #else
        #define IMPLEMENT_TRACE(x)
        #define TRACE                       (__noop)
        #define TRACE0(s)                   (__noop)
        #define TRACE1(s, p1)               (__noop)
        #define TRACE2(s, p1, p2)           (__noop)
        #define TRACE3(s, p1, p2, p3)       (__noop)
        #define _TRACE(s)                   (__noop)
        #define _TRACE_ERROR(s)             (__noop)
        #define _TRACE_WARNING(s)           (__noop)
    #endif
#endif

#ifdef _DEBUG
    #ifndef ASSERT
    #define ASSERT(x) iAssert((x) ? TRUE : FALSE, __FILE__, __LINE__)
    #endif
    #ifndef VERIFY
    #define VERIFY(x) ASSERT(x)
    #endif
    #ifndef VALIDATE
    #define VALIDATE(x) ASSERT(x)
    #endif
#else
    #ifndef ASSERT
    #define ASSERT(x) (__noop)
    #endif
    #ifndef VERIFY
    #define VERIFY(x) ((void)(x))
    #endif
    #ifndef VALIDATE
    #define VALIDATE(x) (__noop)
    #endif
#endif

inline BOOL iAssert(BOOL status, const char* file, const UINT line)
{
    if (!status)
    {
#ifdef _UNICODE
        const short maxBuf = MAX_PATH * 2;
        WCHAR fileName[maxBuf];
        MultiByteToWideChar(CP_ACP, 0, file, -1, fileName, maxBuf);
        TRACE(L"ASSERT: %s(%d)\n", fileName, line);
#else
        TRACE("ASSERT: %s(%d)\n", file, line);
#endif

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP | WINAPI_PARTITION_SYSTEM)
        DebugBreak();
#else
        assert(status);
#endif
    }
    return status;
}
