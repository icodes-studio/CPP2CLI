#pragma once

#pragma pack(push, 4)

template <class TYPE, class ARG_TYPE = const TYPE&>
class iList
{
    protected: struct iNode
    {
        public: iNode* prev;
        public: iNode* next;
        public: TYPE data;
    };

    protected: iNode* headNode;
    protected: iNode* tailNode;
    protected: iNode* freeNode;
    protected: struct iPlex* memoryBlocks;
    protected: int memoryBlockSize;
    protected: int itemCount;

    public: iList(int blockSize = 10)
    {
        Initialize(blockSize);
    }

    public: iList(const iList& source)
    {
        Initialize();
        AddTail(source);
    }

    public: ~iList()
    {
        RemoveAll();
        ASSERT(itemCount == 0);
    }

    protected: void Initialize(int blockSize = 10)
    {
        ASSERT(blockSize > 0);

        itemCount = 0;
        headNode = NULL;
        tailNode = NULL;
        freeNode = NULL;
        memoryBlocks = NULL;
        memoryBlockSize = blockSize;
    }

    public: POSITION AddHead(ARG_TYPE element)
    {
        ASSERT(this);

        iNode* newNode = NewNode(NULL, headNode);
        newNode->data = element;

        if (headNode != NULL)
            headNode->prev = newNode;
        else
            tailNode = newNode;

        headNode = newNode;

        return (POSITION)newNode;
    }

    public: POSITION AddTail(ARG_TYPE element)
    {
        ASSERT(this);

        iNode* newNode = NewNode(tailNode, NULL);
        newNode->data = element;

        if (tailNode != NULL)
            tailNode->next = newNode;
        else
            headNode = newNode;

        tailNode = newNode;

        return (POSITION)newNode;
    }

    public: void AddHead(const iList* source)
    {
        ASSERT(this);
        ASSERT(source != NULL);

        POSITION pos = source->GetTailPosition();

        while (pos != NULL)
            AddHead(source->GetPrev(pos));
    }

    public: void AddHead(const iList& source)
    {
        AddHead(&source);
    }

    public: void AddTail(const iList* source)
    {
        ASSERT(this);
        ASSERT(source != NULL);

        POSITION pos = source->GetHeadPosition();

        while (pos != NULL)
            AddTail(source->GetNext(pos));
    }

    public: void AddTail(const iList& source)
    {
        AddTail(&source);
    }

    public: POSITION InsertBefore(POSITION pos, ARG_TYPE element)
    {
        ASSERT(this);

        if (pos == NULL)
            return AddHead(element);

        iNode* oldNode = (iNode*)pos;
        iNode* newNode = NewNode(oldNode->prev, oldNode);
        newNode->data = element;

        if (oldNode->prev != NULL)
        {
            ASSERT(oldNode->prev);
            oldNode->prev->next = newNode;
        }
        else
        {
            ASSERT(oldNode == headNode);
            headNode = newNode;
        }

        oldNode->prev = newNode;

        return (POSITION)newNode;
    }

    public: POSITION InsertAfter(POSITION pos, ARG_TYPE element)
    {
        ASSERT(this);

        if (pos == NULL)
            return AddTail(element);

        iNode* oldNode = (iNode*)pos;
        ASSERT(oldNode != NULL);
        iNode* newNode = NewNode(oldNode, oldNode->next);
        newNode->data = element;

        if (oldNode->next != NULL)
        {
            ASSERT(oldNode->next);
            oldNode->next->prev = newNode;
        }
        else
        {
            ASSERT(oldNode == tailNode);
            tailNode = newNode;
        }

        oldNode->next = newNode;

        return (POSITION)newNode;
    }

    public: TYPE& GetHead()
    {
        ASSERT(headNode != NULL);
        return headNode->data;
    }

    public: TYPE GetHead() const
    {
        ASSERT(headNode != NULL);
        return headNode->data;
    }

    public: TYPE& GetTail()
    {
        ASSERT(tailNode != NULL);
        return tailNode->data;
    }

    public: TYPE GetTail() const
    {
        ASSERT(tailNode != NULL);
        return tailNode->data;
    }

    public: TYPE& GetNext(POSITION& pos)
    {
        iNode* node = (iNode*)pos;
        ASSERT(node);
        pos = (POSITION)node->next;
        return node->data;
    }

    public: TYPE GetNext(POSITION& pos) const
    {
        iNode* node = (iNode*)pos;
        ASSERT(node);
        pos = (POSITION)node->next;
        return node->data;
    }

    public: TYPE& GetPrev(POSITION& pos)
    {
        iNode* node = (iNode*)pos;
        ASSERT(node);
        pos = (POSITION)node->prev;
        return node->data;
    }

    public: TYPE GetPrev(POSITION& pos) const
    {
        iNode* node = (iNode*)pos;
        ASSERT(node);
        pos = (POSITION)node->prev;
        return node->data;
    }

    public: POSITION GetHeadPosition() const
    {
        return (POSITION)headNode;
    }

    public: POSITION GetTailPosition() const
    {
        return (POSITION)tailNode;
    }

    public: TYPE& GetAt(POSITION pos)
    {
        iNode* node = (iNode*)pos;
        ASSERT(node);
        return node->data;
    }

    public: TYPE GetAt(POSITION pos) const
    {
        iNode* node = (iNode*)pos;
        ASSERT(node);
        return node->data;
    }

    public: void SetAt(POSITION pos, ARG_TYPE element)
    {
        iNode* node = (iNode*)pos;
        ASSERT(node);
        node->data = element;
    }

    public: void RemoveAll()
    {
        ASSERT(this);

        for (iNode* node = headNode; node != NULL; node = node->next)
            DestructElements<TYPE>(&node->data, 1);

        itemCount = 0;
        headNode = NULL;
        tailNode = NULL;
        freeNode = NULL;
        memoryBlocks->FreeDataChain();
        memoryBlocks = NULL;
    }

    public: TYPE RemoveHead()
    {
        ASSERT(this);
        ASSERT(headNode != NULL);

        iNode* oldNode = headNode;
        TYPE removed = oldNode->data;
        headNode = oldNode->next;

        if (headNode != NULL)
            headNode->prev = NULL;
        else
            tailNode = NULL;

        FreeNode(oldNode);

        return removed;
    }

    public: TYPE RemoveTail()
    {
        ASSERT(this);
        ASSERT(tailNode != NULL);

        iNode* oldNode = tailNode;
        TYPE removed = oldNode->data;
        tailNode = oldNode->prev;

        if (tailNode != NULL)
            tailNode->next = NULL;
        else
            headNode = NULL;

        FreeNode(oldNode);

        return removed;
    }

    public: void RemoveAt(POSITION pos)
    {
        ASSERT(this);

        iNode* oldNode = (iNode*)pos;
        ASSERT(oldNode);

        if (oldNode == headNode)
        {
            headNode = oldNode->next;
        }
        else
        {
            ASSERT(oldNode->prev);
            oldNode->prev->next = oldNode->next;
        }

        if (oldNode == tailNode)
        {
            tailNode = oldNode->prev;
        }
        else
        {
            ASSERT(oldNode->next);
            oldNode->next->prev = oldNode->prev;
        }

        FreeNode(oldNode);
    }

    public: BOOL Remove(ARG_TYPE value)
    {
        POSITION pos = Find(value);
        if (pos == NULL)
            return FALSE;

        RemoveAt(pos);

        return TRUE;
    }

    public: int GetCount() const
    {
        return itemCount;
    }

    public: BOOL IsEmpty() const
    {
        return (itemCount == 0);
    }

    public: POSITION Find(ARG_TYPE value, POSITION start = NULL) const
    {
        ASSERT(this);

        iNode* result = (iNode*)start;

        if (result == NULL)
        {
            result = headNode;
        }
        else
        {
            ASSERT(result);
            result = result->next;
        }

        for (; result != NULL; result = result->next)
        {
            if (result->data == value)
                return (POSITION)result;
        }

        return NULL;
    }

    public: POSITION FindIndex(int index) const
    {
        ASSERT(this);

        if (index >= itemCount || index < 0)
            return NULL;

        iNode* result = headNode;
        while (index--)
        {
            ASSERT(result);
            result = result->next;
        }

        return (POSITION)result;
    }

    protected: iNode* NewNode(iNode* prev, iNode* next)
    {
        if (freeNode == NULL)
        {
            iPlex* newBlock = iPlex::Create(memoryBlocks, memoryBlockSize, sizeof(iNode));
            iNode* node = (iNode*)newBlock->GetData();
            node += memoryBlockSize - 1;

            for (int i = memoryBlockSize - 1; i >= 0; i--, node--)
            {
                node->next = freeNode;
                freeNode = node;
            }
        }

        ASSERT(freeNode != NULL);

        iNode* result = freeNode;
        freeNode = freeNode->next;
        result->prev = prev;
        result->next = next;
        itemCount++;

        ASSERT(itemCount > 0);

        ConstructElements<TYPE>(&result->data, 1);

        return result;
    }

    protected: void FreeNode(iNode* node)
    {
        DestructElements<TYPE>(&node->data, 1);
        node->next = freeNode;
        freeNode = node;
        itemCount--;

        ASSERT(itemCount >= 0);

        if (itemCount == 0)
            RemoveAll();
    }

    public: TYPE operator [] (int index) const
    {
        return GetAt(FindIndex(index));
    }

    public: TYPE& operator [] (int index)
    {
        return GetAt(FindIndex(index));
    }

    public: const iList& operator = (const iList& source)
    {
        RemoveAll();
        AddTail(source);
        return *this;
    }
};

#pragma pack(pop)
