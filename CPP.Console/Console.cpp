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
    TRACE(_T("Hello World"));

    const char* lpsz = "Çï·Î¿ùµå";
    int wsize = MultiByteToWideChar(CP_ACP, 0, lpsz, -1, NULL, 0);
    iBuffer<WCHAR> text(wsize);
    MultiByteToWideChar(CP_ACP, 0, lpsz, -1, text, wsize);

    ASSERT(text);
    ASSERT(text != NULL);
    ASSERT(text != __nullptr);

    _setmode(_fileno(stdout), _O_U16TEXT);
    std::wcout << text[0] << text[1] << text[2] << text[3] << L" - " << (WCHAR*)text << std::endl;

    _setmode(_fileno(stdout), _O_TEXT);
    std::cout << lpsz << std::endl;

    text.Release();
    ASSERT(!text);
    ASSERT(text == NULL);
    ASSERT(text == __nullptr);

    system("pause");
}
