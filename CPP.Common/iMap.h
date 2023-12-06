#pragma once

template <class KEY, class VALUE, class ARG_KEY = const KEY&, class ARG_VALUE = const VALUE&>
class iMap
{
    public: struct iNode
    {
        public: iNode* next;
        public: UINT hash;
        public: KEY key;
        public: VALUE value;
    };

    protected: iNode** hashTable;
    protected: UINT hashTableSize;
    protected: iNode* freeList;
    protected: iList<iNode*> orderedList;
    protected: struct iPlex* memoryBlocks;
    protected: int memoryBlockSize;
    protected: int itemCount;

    public: iMap(int blockSize = 10)
    {
        ASSERT(blockSize > 0);

        hashTable = NULL;
        hashTableSize = 17;
        itemCount = 0;
        freeList = NULL;
        memoryBlocks = NULL;
        memoryBlockSize = blockSize;
    }

    public: ~iMap()
    {
        RemoveAll();
        ASSERT(itemCount == 0);
    }

    public: int GetCount() const
    {
        return itemCount;
    }

    public: BOOL IsEmpty() const
    {
        return (itemCount == 0);
    }

    public: BOOL ContainsKey(ARG_KEY key)
    {
        VALUE value;
        return Lookup(key, value);
    }

    public: BOOL ContainsValue(ARG_VALUE value)
    {
        for (POSITION pos = GetStartPosition(); pos != NULL;)
        {
            KEY k; VALUE v;
            GetNext(pos, k, v);
            if (value == v) return TRUE;
        }

        return FALSE;
    }

    public: BOOL Lookup(ARG_KEY key, VALUE& value) const
    {
        ASSERT(this);

        UINT hash;
        iNode* node = GetAt(key, hash);
        if (node == NULL)
            return FALSE;

        value = node->value;
        return TRUE;
    }

    public: VALUE& operator [] (ARG_KEY key)
    {
        ASSERT(this);

        UINT hash;
        iNode* node;

        if ((node = GetAt(key, hash)) == NULL)
        {
            if (hashTable == NULL)
                InitHashTable(hashTableSize);

            node = NewNode();
            node->hash = hash;
            node->key = key;
            node->next = hashTable[hash];

            hashTable[hash] = node;
        }

        return node->value;
    }

    public: void SetAt(ARG_KEY key, ARG_VALUE value)
    {
        (*this)[key] = value;
    }

    public: BOOL RemoveKey(ARG_KEY key)
    {
        ASSERT(this);

        if (hashTable == NULL)
            return FALSE;

        iNode** nodePrev;
        nodePrev = &hashTable[HashKey<ARG_KEY>(key) % hashTableSize];

        for (iNode* node = *nodePrev; node != NULL; node = node->next)
        {
            if (CompareElements(&node->key, &key))
            {
                *nodePrev = node->next;
                FreeNode(node);
                return TRUE;
            }
            nodePrev = &node->next;
        }

        return FALSE;
    }

    public: void RemoveAll()
    {
        ASSERT(this);

        if (hashTable != NULL)
        {
            for (UINT hash = 0; hash < hashTableSize; hash++)
            {
                iNode* node;
                for (node = hashTable[hash]; node != NULL; node = node->next)
                {
                    DestructElements<VALUE>(&node->value, 1);
                    DestructElements<KEY>(&node->key, 1);
                }
            }
        }

        SAFE_DELETE_ARRAY(hashTable);

        itemCount = 0;
        freeList = NULL;
        memoryBlocks->FreeDataChain();
        memoryBlocks = NULL;
        orderedList.RemoveAll();
    }

    public: POSITION GetStartPosition() const
    {
        return (itemCount == 0) ? NULL : BEFORE_START_POSITION;
    }

    public: void GetNext(POSITION& next, KEY& key, VALUE& value) const
    {
        ASSERT(this);
        ASSERT(hashTable != NULL);

        iNode* nodeRet = (iNode*)next;
        ASSERT(nodeRet != NULL);

        if (nodeRet == (iNode*)BEFORE_START_POSITION)
        {
            for (UINT bucket = 0; bucket < hashTableSize; bucket++)
            {
                if ((nodeRet = hashTable[bucket]) != NULL)
                    break;
            }
            ASSERT(nodeRet != NULL);
        }

        ASSERT(nodeRet != NULL);
        iNode* nodeNext;

        if ((nodeNext = nodeRet->next) == NULL)
        {
            for (UINT bucket = nodeRet->hash + 1; bucket < hashTableSize; bucket++)
            {
                if ((nodeNext = hashTable[bucket]) != NULL)
                    break;
            }
        }

        next = (POSITION)nodeNext;
        key = nodeRet->key;
        value = nodeRet->value;
    }

    public: void InitHashTable(UINT hashSize, BOOL allocNow = TRUE)
    {
        ASSERT(this);
        ASSERT(itemCount == 0);
        ASSERT(hashSize > 0);

        SAFE_DELETE_ARRAY(hashTable);

        if (allocNow)
        {
            hashTable = new iNode*[hashSize];
            memset(hashTable, 0, sizeof(iNode*) * hashSize);
        }

        hashTableSize = hashSize;
    }

    public: UINT GetHashTableSize() const
    {
        return hashTableSize;
    }

    public: const iList<iNode*>& ToList() const
    {
        return orderedList;
    }

    protected: iNode* NewNode()
    {
        if (freeList == NULL)
        {
            iPlex* newBlock = iPlex::Create(memoryBlocks, memoryBlockSize, sizeof(iNode));
            iNode* node = (iNode*) newBlock->GetData();

            node += memoryBlockSize - 1;

            for (int i = memoryBlockSize - 1; i >= 0; i--, node--)
            {
                node->next = freeList;
                freeList = node;
            }
        }

        ASSERT(freeList != NULL);

        iNode* node = freeList;
        freeList = freeList->next;
        itemCount++;
        ASSERT(itemCount > 0);

        ConstructElements<KEY>(&node->key, 1);
        ConstructElements<VALUE>(&node->value, 1);

        orderedList.AddTail(node);

        return node;
    }

    protected: void FreeNode(iNode* node)
    {
        orderedList.Remove(node);

        DestructElements<VALUE>(&node->value, 1);
        DestructElements<KEY>(&node->key, 1);

        node->next = freeList;
        freeList = node;
        itemCount--;
        ASSERT(itemCount >= 0);

        if (itemCount == 0)
            RemoveAll();
    }

    protected: iNode* GetAt(ARG_KEY key, UINT& hash) const
    {
        hash = HashKey<ARG_KEY>(key) % hashTableSize;

        if (hashTable == NULL)
            return NULL;

        for (iNode* node = hashTable[hash]; node != NULL; node = node->next)
        {
            if (node->key == key)
                return node;
        }

        return NULL;
    }
};
