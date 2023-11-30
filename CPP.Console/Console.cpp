#include "Common.h"
#include <fcntl.h>
#include <io.h>

int main()
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    TODO(todo warning message);
    float* mleak = new float;
    TRACE(_T("Hello World\n"));

    const char* lpsz = "Çï·Î¿ùµå";
    int wsize = MultiByteToWideChar(CP_ACP, 0, lpsz, -1, NULL, 0);
    iBuffer<WCHAR> wtext(wsize);
    MultiByteToWideChar(CP_ACP, 0, lpsz, -1, wtext, wsize);
    OutputDebugStringW(wtext);

    ASSERT(wtext);
    ASSERT(wtext != NULL);
    ASSERT(wtext != __nullptr);

    _setmode(_fileno(stdout), _O_U16TEXT);
    std::wcout << wtext[0] << wtext[1] << wtext[2] << wtext[3] << L" - " << wtext << L" - "  << (WCHAR*)wtext << std::endl;

    int msize = WideCharToMultiByte(CP_ACP, 0, wtext, -1, NULL, 0, NULL, NULL);
    iBuffer<CHAR> mtext(msize);
    WideCharToMultiByte(CP_ACP, 0, wtext, -1, mtext, msize, NULL, NULL);

    _setmode(_fileno(stdout), _O_TEXT);
    std::cout << mtext << std::endl;

    wtext.Release();
    ASSERT(!wtext);
    ASSERT(wtext == NULL);
    ASSERT(wtext == __nullptr);




    system("pause");
}
