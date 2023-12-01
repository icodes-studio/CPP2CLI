#pragma once

class iFixedAlloc
{
    public: iFixedAlloc(UINT allocSize = 64, UINT blockSize = 64)
    {
        ASSERT(allocSize >= sizeof(iNode));
        ASSERT(blockSize > 1);

        this->allocSize = allocSize;
        this->blockSize = blockSize;
        this->freeNode = NULL;
        this->blocks = NULL;

        InitializeCriticalSection(&protect);
    }

    public: ~iFixedAlloc()
    {
        FreeAll();
        DeleteCriticalSection(&protect);
    }

    public: void FreeAll()
    {
        iAutoLock lock(&protect);
        blocks->FreeDataChain();
        blocks = NULL;
        freeNode = NULL;
    }

    public: void* Alloc()
    {
        iAutoLock lock(&protect);

        if (freeNode == NULL)
        {
            iPlex* newBlock = iPlex::Create(blocks, blockSize, allocSize);
            if (newBlock == NULL)
                return NULL;

            iNode* node = (iNode*)newBlock->GetData();
            (BYTE*&)node += (allocSize * blockSize) - allocSize;
            for (int i = blockSize - 1; i >= 0; i--, (BYTE*&)node -= allocSize)
            {
                node->next = freeNode;
                freeNode = node;
            }
        }

        ASSERT(freeNode != NULL);
        void* node = freeNode;
        freeNode = freeNode->next;

        return node;
    }

    public: void Free(void* p)
    {
        if (p != NULL)
        {
            iAutoLock lock(&protect);
            iNode* node = (iNode*)p;
            node->next = freeNode;
            freeNode = node;
        }
    }

    public: UINT GetAllocSize() 
    { 
        return allocSize;
    }

    protected: struct iNode
    {
        public: iNode* next;
    };

    protected: UINT allocSize;
    protected: UINT blockSize;
    protected: iPlex* blocks;
    protected: iNode* freeNode;
    protected: CRITICAL_SECTION protect;
};

#ifndef _DEBUG
    #define DECLARE_FIXED_ALLOC(class_name)                             \
    public: void* operator new(size_t size)                             \
    {                                                                   \
        ASSERT(size == s_alloc.GetAllocSize());                         \
        UNUSED(size);                                                   \
        return s_alloc.Alloc();                                         \
    }                                                                   \
    public: void* operator new(size_t, void* p)                         \
    {                                                                   \
        return p;                                                       \
    }                                                                   \
    public: void operator delete(void* p)                               \
    {                                                                   \
        s_alloc.Free(p);                                                \
    }                                                                   \
    public: void* operator new(size_t size, LPCSTR, int)                \
    {                                                                   \
        ASSERT(size == s_alloc.GetAllocSize());                         \
        UNUSED(size);                                                   \
        return s_alloc.Alloc();                                         \
    }                                                                   \
    protected: static iFixedAlloc s_alloc;

    #define IMPLEMENT_FIXED_ALLOC(class_name, block_size)               \
    iFixedAlloc class_name::s_alloc(sizeof(class_name), block_size)
#else
    #define DECLARE_FIXED_ALLOC(class_name)
    #define IMPLEMENT_FIXED_ALLOC(class_name, block_size)
#endif
