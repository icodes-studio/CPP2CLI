#include "Common.h"

int main()
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    TODO(aaaaaaaa);
    float* data = new float;
    TRACE(_T("Hello New World"));

    const char* lpsz = "동해물과";
    int ret = (int)strlen(lpsz) + 1;
    int convert = MultiByteToWideChar(CP_ACP, 0, lpsz, ret, NULL, 0);
    iBuffer<WCHAR> pszW(convert);
    pszW[0] = 99;
    pszW[1] = 99;
    pszW[2] = 99;
    pszW[3] = 99;
    pszW[4] = 99;
    ret = MultiByteToWideChar(CP_ACP, 0, "동해물과", ret, pszW, convert);




    UINT bytes = static_cast<UINT>(ret) * static_cast<UINT>(sizeof(OLECHAR));
    BSTR temp = SysAllocStringByteLen((LPCSTR)(LPWSTR)pszW, bytes);

    std::cout << "Hello World!\n";

    system("pause");
}
