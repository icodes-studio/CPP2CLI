#pragma once

#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <crtdbg.h>
#include <assert.h>

#include <tchar.h>
#include <mbstring.h>
#include <iostream>

#include <string>
#include <vector>
#include <stack>

#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/en.h"

using namespace rapidjson;

#ifdef _DEBUG
    #ifndef DEBUG_NEW
        #define DEBUG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
        #define new DEBUG_NEW
    #endif
#endif

#ifndef ROUND4
    #define ROUND(x, y) (((x)+(y-1))&~(y-1))
    #define ROUND4(x) ROUND(x, 4)
#endif

#ifdef _DEBUG
    #define UNUSED(x)
#else
    #define UNUSED(x) x
#endif

#ifndef _countof
    #define _countof(x) (sizeof(x) / sizeof(x[0]))
#endif

#ifndef POSITION
    struct iVoid { };
    typedef iVoid* POSITION;
#endif

#ifndef BEFORE_START_POSITION
    #define BEFORE_START_POSITION ((POSITION)-1L)
#endif

#ifndef SAFE_DELETE
    #define SAFE_DELETE(x) \
    do { \
        if (x) \
        { \
            delete x; \
            x = NULL; \
        } \
    } while (0)
#endif

#ifndef SAFE_DELETE_ARRAY
    #define SAFE_DELETE_ARRAY(x) \
    do { \
        if (x) \
        { \
            delete [] x; \
            x = NULL; \
        } \
    } while (0)
#endif

#ifndef STRING
    #define STRING2(x) #x
    #define STRING(x) STRING2(x)
#endif

#ifndef TODO
    #define TODO(msg) \
    do { \
    __pragma(message(__FILE__ "(" STRING(__LINE__) "): TODO: " #msg)) \
    __pragma(warning(push)) \
    __pragma(warning(disable:4127)) \
    } while(0) \
    __pragma (warning(pop))
#endif

#ifndef DEPRECATED
    #define DEPRECATED(msg) __declspec( deprecated(msg) )
#endif

#define BUFSIZE_TINY   64
#define BUFSIZE_SMALL  128
#define BUFSIZE_MIDIUM 256
#define BUFSIZE_BIG    512
#define BUFSIZE_SUPER  1024
#define BUFSIZE_MEGA   2048
#define BUFSIZE_HUGE   4096

#pragma message (__FILE__ "(" STRING(__LINE__) "): TODO: We need to remove the windows dependency")
#include <oaidl.h>

#include "iTrace.h"
#include "iBuffer.h"
#include "iAutoLock.h"
#include "iPlex.h"
#include "iFixedAlloc.h"
#include "iTemplate.h"
#include "iList.h"
#include "iArray.h"
#include "iString.h"
#include "iMap.h"
#include "iVariant.h"
#include "iPtr.h"
#include "iSingleton.h"
#include "iObject.h"
