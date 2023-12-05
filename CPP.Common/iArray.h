#pragma once

#pragma pack(push, 4)

template <class TYPE, class ARG_TYPE = const TYPE&>
class iArray
{
    protected: TYPE* data;
    protected: int size;
    protected: int maxSize;
    protected: int growBy;

    public: iArray()
    {
        Initialize();
    }

    public: iArray(const iArray& source)
    {
        Initialize();
        Copy(source);
    }

    public: ~iArray()
    {
        ASSERT(this);
        DestructElements<TYPE>(data, size);
        delete [] (BYTE*)data;
    }

    protected: void Initialize()
    {
        data = NULL;
        size = 0;
        maxSize = 0;
        growBy = 0;
    }

    public: BOOL IsEmpty() const
    {
        return (size == 0);
    }

    public: int GetCount() const
    {
        return size;
    }

    public: int GetSize() const
    {
        return size;
    }

    public: int GetUpperBound() const
    {
        return (size - 1);
    }

    public: void SetSize(int newSize, int growBy = -1)
    {
        ASSERT(this);
        ASSERT(newSize >= 0);

        if (growBy != -1)
            this->growBy = growBy;

        if (newSize == 0)
        {
            DestructElements<TYPE>(data, size);
            delete [] (BYTE*)data;
            data = NULL;
            size = maxSize = 0;
        }
        else if (data == NULL)
        {
            ASSERT(newSize <= UINT_MAX / sizeof(TYPE));

            data = (TYPE*) new BYTE[newSize * sizeof(TYPE)];
            ConstructElements<TYPE>(data, newSize);
            size = maxSize = newSize;
        }
        else if (newSize <= maxSize)
        {
            if (newSize > size)
                ConstructElements<TYPE>(&data[size], newSize - size);
            else if (newSize < size)
                DestructElements<TYPE>(&data[newSize], size - newSize);

            size = newSize;
        }
        else // if (newSize > maxSize)
        {
            int growBy = this->growBy;
            if (growBy == 0)
                growBy = min(1024, max(4, size / 8));

            int newMax =
                (newSize < maxSize + growBy) ?
                (maxSize + growBy) : newSize;

            ASSERT(newMax >= maxSize);
            ASSERT(newMax <= UINT_MAX / sizeof(TYPE));

            TYPE* newData = (TYPE*) new BYTE[newMax * sizeof(TYPE)];
            memcpy(newData, data, size * sizeof(TYPE));

            ASSERT(newSize > size);
            ConstructElements<TYPE>(&newData[size], newSize - size);

            delete [] (BYTE*)data;
            data = newData;
            size = newSize;
            maxSize = newMax;
        }
    }

    public: void FreeExtra()
    {
        ASSERT(this);

        if (size != maxSize)
        {
            ASSERT(size <= UINT_MAX / sizeof(TYPE));

            TYPE* newData = NULL;
            if (size != 0)
            {
                newData = (TYPE*) new BYTE[size * sizeof(TYPE)];
                memcpy(newData, data, size * sizeof(TYPE));
            }

            delete [] (BYTE*)data;
            data = newData;
            maxSize = size;
        }
    }

    public: void RemoveAll()
    {
        SetSize(0, -1);
    }

    public: void RemoveAt(int index, int count = 1)
    {
        ASSERT(this);
        ASSERT(index >= 0);
        ASSERT(count >= 0);
        ASSERT(index + count <= size);

        int moveCount = size - (index + count);
        DestructElements<TYPE>(&data[index], count);

        if (moveCount)
            memmove(&data[index], &data[index + count], moveCount * sizeof(TYPE));

        size -= count;
    }

    public: BOOL Remove(ARG_TYPE value)
    {
        int index = Find(value);
        if (index < 0)
            return FALSE;

        RemoveAt(index);

        return TRUE;
    }

    public: int Find(ARG_TYPE value, int start = 0) const
    {
        ASSERT(this);

        for (int i = start; i < size; ++i)
        {
            if (data[i] == value)
                return i;
        }

        return -1;
    }

    public: TYPE& GetAt(int index)
    {
        ASSERT(index >= 0 && index < size);
        return data[index];
    }

    public: const TYPE& GetAt(int index) const
    {
        ASSERT(index >= 0 && index < size);
        return data[index];
    }

    public: void SetAt(int index, ARG_TYPE element)
    {
        ASSERT(index >= 0 && index < size);
        data[index] = element;
    }

    public: void SetAtGrow(int index, ARG_TYPE element)
    {
        ASSERT(this);
        ASSERT(index >= 0);

        if (index >= size)
            SetSize(index + 1, -1);

        data[index] = element;
    }

    public: int Add(ARG_TYPE element)
    {
        int index = size;
        SetAtGrow(index, element);
        return index;
    }

    public: int Append(const iArray& source)
    {
        ASSERT(this);
        ASSERT(this != &source);

        int oldSize = size;
        SetSize(size + source.size);
        CopyElements<TYPE>(data + oldSize, source.data, source.size);

        return oldSize;
    }

    public: void Copy(const iArray& source)
    {
        ASSERT(this);
        ASSERT(this != &source);

        SetSize(source.size);
        CopyElements<TYPE>(data, source.data, source.size);
    }

    public: void InsertAt(int index, ARG_TYPE element, int count = 1)
    {
        ASSERT(this);
        ASSERT(index >= 0);
        ASSERT(count > 0);

        if (index >= size)
        {
            SetSize(index + count);
        }
        else
        {
            int oldSize = size;
            SetSize(size + count, -1);
            DestructElements<TYPE>(&data[oldSize], count);
            memmove(&data[index + count], &data[index], (oldSize - index) * sizeof(TYPE));
            ConstructElements<TYPE>(&data[index], count);
        }

        ASSERT(index + count <= size);

        while (count--)
            data[index++] = element;
    }

    public: void InsertAt(int start, const iArray* newArray)
    {
        ASSERT(this);
        ASSERT(newArray);
        ASSERT(start >= 0);

        if (newArray->GetSize() > 0)
        {
            InsertAt(start, newArray->GetAt(0), newArray->GetSize());
            for (int i = 0; i < newArray->GetSize(); i++)
                SetAt(start + i, newArray->GetAt(i));
        }
    }

    public: const TYPE* GetData() const
    {
        return (const TYPE*)data;
    }

    public: TYPE* GetData()
    {
        return (TYPE*)data;
    }

    public: TYPE operator [] (int index) const
    {
        return GetAt(index);
    }

    public: TYPE& operator [] (int index)
    {
        return GetAt(index);
    }

    public: const iArray& operator = (const iArray& source)
    {
        Copy(source);
        return *this;
    }
};

#pragma pack(pop)
