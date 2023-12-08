#include <i.h>
#include <fcntl.h>
#include <io.h>

struct Node
{
    public: Node* next;
    public: Node* prev;
    public: float data;
};

int main()
{
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    TODO(--------------------);
    TODO(test warning message);
    TODO(--------------------);
    float* mleak = new float;
    TRACE(_T("Hello World\n"));

    // iBuffer
    const char* lpsz = "헬로월드";
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

    // iPlex
    int memoryBlockCount = 10;
    struct iPlex* memoryBlocks = NULL;

    for (int i = 0; i < 2; i++)
    {
        iPlex* newBlock = iPlex::Create(memoryBlocks, memoryBlockCount, sizeof(Node));
        Node* node = (Node*)newBlock->GetData();
        node = node + (memoryBlockCount - 1);

        Node* freeNode = NULL;
        for (int i = memoryBlockCount - 1; i >= 0; i--, node--)
        {
            node->next = freeNode;
            freeNode = node;
        }

        while (freeNode != NULL)
        {
            Node* newNode = freeNode;
            freeNode = freeNode->next;
        }
    }

    memoryBlocks->FreeDataChain();

    // iFixedAlloc
    int length = 128;
    iFixedAlloc alloc128 = iFixedAlloc(ROUND4(129 * sizeof(TCHAR) + sizeof(iStringDataT<TCHAR>)), 2);
    iStringDataT<TCHAR>* newData = (iStringDataT<TCHAR>*)alloc128.Alloc();
    newData->GetData()[length] = '\0';
    ASSERT(alloc128.freeNode != NULL);
    newData = (iStringDataT<TCHAR>*)alloc128.Alloc();
    newData->GetData()[length] = '\0';
    ASSERT(alloc128.freeNode == NULL);
    newData = (iStringDataT<TCHAR>*)alloc128.Alloc();
    newData->GetData()[length] = '\0';
    ASSERT(alloc128.freeNode != NULL);

    // iList
    iList<Node> testList;
    for (float f = 0; f < 10.f; f++)
        testList.AddTail({ NULL, NULL, f });

    for (POSITION pos = testList.GetHeadPosition(); pos != NULL; )
        _tprintf(_T("%f\n"), testList.GetNext(pos).data);

    // iArray
    iArray<Node> testArray;
    for (float f = 0; f < 10.f; f++)
        testArray.Add({ NULL, NULL, f });

    for (int i = 0; i < testArray.GetSize(); i++)
        _tprintf(_T("%f\n"), testList[i].data);

    // iMap
    iMap<int, Node> testMap;
    testMap[10] = { NULL, NULL, 10.f };
    testMap.RemoveKey(10);

    // iString
    iString str1;
    iString str2("동해물과 백두산이");
    iString str3 = str2;
    str2 = "";
    str1 = str2;
    str1.FormatMessage(_T("%1!*.*s! %3 %4!*s!"), 4, 2, _T("Bill"), _T("Bob"), 6, _T("Bill"));
    BSTR bstr = str3.AllocSysString();
    TRACE(bstr);
    SysFreeString(bstr);
    iString str4(_T('A'), 10);
    iMap<iString, iString> stringMap;
    stringMap[_T("111")] = _T("111");
    stringMap[_T("222")] = _T("222");
    stringMap[_T("333")] = _T("333");
    stringMap[_T("444")] = _T("444");
    for (POSITION pos = stringMap.GetStartPosition(); pos != NULL;)
    {
        iString key, value;
        stringMap.GetNext(pos, key, value);
        TRACE(_T("key:%s, value:%s\n"), key, value);
    }
    const auto& list = stringMap.ToList();
    for (POSITION pos = list.GetHeadPosition(); pos != NULL;)
    {
        auto node = list.GetNext(pos);
        TRACE(_T("key:%s, value:%s\n"), node->key, node->value);
    }

    // iVariant
    iVariant a("aaaaa");
    iVariant b((long long)1234567891234567891);
    a = b;
    TRACE(a); 
    TRACE(_T("\n"));
    a = (const iVariant*)&b;
    a[_T("a1111")] = 10;
    a[_T("222")] = _T("동해물과");
    a[_T("333")] = _T("333");
    a[_T("333")][_T("A")] = _T("AAA");
    a[_T("333")][_T("B")] = _T("BBB");
    a[_T("333")][_T("C")] = _T("CCC");
    a[_T("444")][0] = _T("data-0");
    a[_T("444")][1] = _T("data-1");
    a[_T("444")][2] = 2;
    a[_T("444")][3][0] = 0;
    a[_T("444")][3][1] = 1;
    a[_T("444")][3][2] = 2;
    a[_T("444")][3][3][_T("a")] = _T("AAA");
    a[_T("444")][3][3][_T("b")] = _T("BBB");
    a[_T("444")][3][3][_T("c")] = 9.3f;
    a.Log();
    auto serialized = a.Serialize();
    TRACE(serialized);
    iVariant c;
    BOOL result = c.Deserialize(serialized);
    c.Log();


    system("pause");
}

