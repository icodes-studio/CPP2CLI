#include "Common.h"

int main()
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    TODO(aaaaaaaa);
    float* data = new float;
    TRACE(_T("Hello New World"));

    std::cout << "Hello World!\n";

    system("pause");
}
