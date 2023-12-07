#pragma once

#define FIXEDALLOC

#define CHNIL INT_MIN

#pragma pack(push, 4)

template <class T = char>
class iStringType
{
    public: typedef char XCHAR;
    public: typedef LPSTR LPXSTR;
    public: typedef LPCSTR LPCXSTR;
    public: typedef wchar_t YCHAR;
    public: typedef LPWSTR LPYSTR;
    public: typedef LPCWSTR LPCYSTR;
};

template <>
class iStringType <wchar_t>
{
    public: typedef wchar_t XCHAR;
    public: typedef LPWSTR LPXSTR;
    public: typedef LPCWSTR LPCXSTR;
    public: typedef char YCHAR;
    public: typedef LPSTR LPYSTR;
    public: typedef LPCSTR LPCYSTR;
};

template <class T>
struct iStringDataT
{
    public: long refs;
    public: int dataLength;
    public: int allocLength;
    public: T* GetData() { return (T*)(this + 1); }
};

template <class T>
class iStringT
{
    protected: typedef typename iStringType<T>::XCHAR XCHAR;
    protected: typedef typename iStringType<T>::LPXSTR LPXSTR;
    protected: typedef typename iStringType<T>::LPCXSTR LPCXSTR;
    protected: typedef typename iStringType<T>::YCHAR YCHAR;
    protected: typedef typename iStringType<T>::LPYSTR LPYSTR;
    protected: typedef typename iStringType<T>::LPCYSTR LPCYSTR;

    protected: LPXSTR stringData;
    protected: static T chNil;
    protected: static int initData[];
    public: static const iStringT<T> empty;

#ifdef FIXEDALLOC
    static iFixedAlloc alloc64;
    static iFixedAlloc alloc128;
    static iFixedAlloc alloc256;
    static iFixedAlloc alloc512;
#endif

    public: iStringT()
    {
        Initialize();
    }

    public: iStringT(const iStringT& str)
    {
        ASSERT(str.GetData()->refs != 0);

        if (str.GetData()->refs >= 0)
        {
            stringData = str.stringData;
            InterlockedIncrement(&GetData()->refs);
        }
        else
        {
            Initialize();
            *this = str.stringData;
        }
    }

    public: iStringT(const VARIANT& vt)
    {
        Initialize();
        *this = vt;
    }

    public: iStringT(LPCXSTR str)
    {
        Initialize();
        *this = str;
    }

    public: iStringT(LPCYSTR str)
    {
        Initialize();
        *this = str;
    }

    public: iStringT(const unsigned char* str)
    {
        Initialize();
        *this = str;
    }

    public: iStringT(LPCXSTR str, int length)
    {
        Initialize();
        AssignCopy(length, str);
    }

    public: iStringT(LPCYSTR str, int length)
    {
        Initialize();
        AssignConvertCopy(length, str);
    }

    public: iStringT(XCHAR ch, int length = 1)
    {
        Initialize();

        if (length > 0)
        {
            LPXSTR buffer = GetBuffer(length);
            FloodCharacters(buffer, ch, length);
            ReleaseBuffer(length);
        }
    }

    public: ~iStringT()
    {
        if (GetData()->refs != CHNIL)
        {
            if (InterlockedDecrement(&GetData()->refs) <= 0)
                FreeData(GetData());
        }
    }

    protected: void Initialize()
    {
        stringData = GetEmptyString().stringData;
    }

    public: const iStringT& operator = (const iStringT& str)
    {
        if (stringData != str.stringData)
        {
            if (GetData()->refs < 0 || str.GetData()->refs < 0)
            {
                AssignCopy(str.GetData()->dataLength, str.stringData);
            }
            else
            {
                Release();
                stringData = str.stringData;
                InterlockedIncrement(&GetData()->refs);
            }
        }
        return *this;
    }

    public: const iStringT& operator = (const VARIANT& vt)
    {
        VARIANT result;
        VariantInit(&result);

        if (SUCCEEDED(VariantChangeType(&result, const_cast<VARIANT*>(&vt), 0, VT_BSTR)))
            *this = V_BSTR(&result);

        return *this;
    }

    public: const iStringT& operator = (LPCXSTR str)
    {
        AssignCopy(StringLength(str), str);
        return *this;
    }

    public: const iStringT& operator = (LPCYSTR str)
    {
        AssignConvertCopy(StringLength(str), str);
        return *this;
    }

    public: const iStringT& operator = (const unsigned char* str)
    {
        return (operator = ((LPCSTR)str));
    }

    public: const iStringT& operator = (XCHAR ch)
    {
        XCHAR ach[2] = { ch, 0 };
        return (operator = (ach));
    }

    public: const iStringT& operator = (YCHAR ch)
    {
        YCHAR ach[2] = { ch, 0 };
        return (operator = (ach));
    }

    public: const iStringT& operator += (const iStringT& str)
    {
        Append(str.GetData()->dataLength, str.stringData);
        return *this;
    }

    public: const iStringT& operator += (const VARIANT& vt)
    {
        iStringT str(vt);
        return (operator += (str));
    }

    public: const iStringT& operator += (LPCXSTR str)
    {
        ASSERT(str);
        Append(StringLength(str), str);
        return *this;
    }

    public: const iStringT& operator += (LPCYSTR str)
    {
        iStringT str(str);
        return (operator += (str));
    }

    public: const iStringT& operator += (const unsigned char* str)
    {
        return (operator += ((LPCSTR)str));
    }

    public: const iStringT& operator += (XCHAR ch)
    {
        XCHAR ach[2] = { ch, 0 };
        return (operator += (ach));
    }

    public: const iStringT& operator += (YCHAR ch)
    {
        YCHAR ach[2] = { ch, 0 };
        return (operator += (ach));
    }

    friend iStringT operator + (const iStringT& str1, const iStringT& str2)
    {
        iStringT s;
        s.Concatenate(str1.GetData()->dataLength, str1.stringData, str2.GetData()->dataLength, str2.stringData);
        return s;
    }

    friend iStringT operator + (const iStringT& str1, LPCXSTR str2)
    {
        ASSERT(str2);
        iStringT s;
        s.Concatenate(str1.GetData()->dataLength, str1.stringData, StringLength(str2), str2);
        return s;
    }

    friend iStringT operator + (LPCXSTR str1, const iStringT& str2)
    {
        ASSERT(str1);
        iStringT s;
        s.Concatenate(StringLength(str1), str1, str2.GetData()->dataLength, str2.stringData);
        return s;
    }

    friend iStringT operator + (const iStringT& str, XCHAR ch)
    {
        iStringT s;
        s.Concatenate(str.GetData()->dataLength, str.stringData, 1, &ch);
        return s;
    }

    friend iStringT operator + (XCHAR ch, const iStringT& str)
    {
        iStringT s;
        s.Concatenate(1, &ch, str.GetData()->dataLength, str.stringData);
        return s;
    }

    friend iStringT operator + (const iStringT& str1, LPCYSTR str2)
    {
        return str1 + iStringT(str2);
    }

    friend iStringT operator + (LPCYSTR str1, const iStringT& str2)
    {
        return iStringT(str1) + str2;
    }

    friend iStringT operator + (const iStringT& str, YCHAR ch)
    {
        YCHAR ach[2] = { ch, 0 };
        return str + ach;
    }

    friend iStringT operator + (YCHAR ch, const iStringT& str)
    {
        YCHAR ach[2] = { ch, 0 };
        return ach + str;
    }

    friend BOOL operator == (const iStringT& str1, const iStringT& str2)
    {
        return (str1.Compare(str2) == 0);
    }

    friend BOOL operator == (const iStringT& str1, LPCXSTR str2)
    {
        return (str1.Compare(str2) == 0);
    }

    friend BOOL operator == (LPCXSTR str1, const iStringT& str2)
    {
        return (str2.Compare(str1) == 0);
    }

    friend BOOL operator != (const iStringT& str1, const iStringT& str2)
    {
        return (str1.Compare(str2) != 0);
    }

    friend BOOL operator != (const iStringT& str1, LPCXSTR str2)
    {
        return (str1.Compare(str2) != 0);
    }

    friend BOOL operator != (LPCXSTR str1, const iStringT& str2)
    {
        return (str2.Compare(str1) != 0);
    }

    friend BOOL operator < (const iStringT& str1, const iStringT& str2)
    {
        return (str1.Compare(str2) < 0);
    }

    friend BOOL operator < (const iStringT& str1, LPCXSTR str2)
    {
        return (str1.Compare(str2) < 0);
    }

    friend BOOL operator < (LPCXSTR str1, const iStringT& str2)
    {
        return (str2.Compare(str1) > 0);
    }

    friend BOOL operator > (const iStringT& str1, const iStringT& str2)
    {
        return (str1.Compare(str2) > 0);
    }

    friend BOOL operator > (const iStringT& str1, LPCXSTR str2)
    {
        return (str1.Compare(str2) > 0);
    }

    friend BOOL operator > (LPCXSTR str1, const iStringT& str2)
    {
        return (str2.Compare(str1) < 0);
    }

    friend BOOL operator <= (const iStringT& str1, const iStringT& str2)
    {
        return (str1.Compare(str2) <= 0);
    }

    friend BOOL operator <= (const iStringT& str1, LPCXSTR str2)
    {
        return (str1.Compare(str2) <= 0);
    }

    friend BOOL operator <= (LPCXSTR str1, const iStringT& str2)
    {
        return (str2.Compare(str1) >= 0);
    }

    friend BOOL operator >= (const iStringT& str1, const iStringT& str2)
    {
        return (str1.Compare(str2) >= 0);
    }

    friend BOOL operator >= (const iStringT& str1, LPCXSTR str2)
    {
        return (str1.Compare(str2) >= 0);
    }

    friend BOOL operator >= (LPCXSTR str1, const iStringT& str2)
    {
        return (str2.Compare(str1) <= 0);
    }

    public: XCHAR operator [] (int index) const
    {
        ASSERT(index >= 0);
        ASSERT(index < GetData()->dataLength);
        return stringData[index];
    }

    public: operator LPCXSTR () const
    {
        return stringData;
    }

    public: LPCXSTR GetString() const
    {
        return stringData;
    }

    public: XCHAR GetAt(int index) const
    {
        ASSERT(index >= 0);
        ASSERT(index < GetData()->dataLength);
        return stringData[index];
    }

    public: void SetAt(int index, XCHAR ch)
    {
        ASSERT(index >= 0);
        ASSERT(index < GetData()->dataLength);

        CopyBeforeWrite();
        stringData[index] = ch;
    }

    public: int GetAllocLength() const
    {
        return GetData()->allocLength;
    }

    public: int GetLength() const
    {
        return GetData()->dataLength;
    }

    public: BOOL IsEmpty() const
    {
        return (GetData()->dataLength == 0);
    }

    public: void Empty()
    {
        if (GetData()->dataLength == 0)
            return;

        if (GetData()->refs >= 0)
            Release();
        else
            *this = &chNil;

        ASSERT(GetData()->dataLength == 0);
        ASSERT(GetData()->refs < 0 || GetData()->allocLength == 0);
    }

    public: int Compare(LPCXSTR str) const
    {
        ASSERT(str);
        return Compare(stringData, str);
    }

    public: int CompareNoCase(LPCXSTR str) const
    {
        ASSERT(str);
        return CompareNoCase(stringData, str);
    }

    public: int Collate(LPCXSTR str) const
    {
        ASSERT(str);
        return Collate(stringData, str);
    }

    public: int CollateNoCase(LPCXSTR str) const
    {
        ASSERT(str);
        return CollateNoCase(stringData, str);
    }

    public: iStringT Split(int index, TCHAR seperator)
    {
        iStringT result;
        LPXSTR str = stringData;

        while (index--)
        {
            str = FindChar(str, seperator);
            if (str == NULL)
                return result;

            str++;
        }

        LPCXSTR end = FindChar(str, seperator);
        int length = (end == NULL) ? StringLength(str) : (int)(end - str);
        ASSERT(length >= 0);
        memcpy_s(result.GetBufferSetLength(length), length * sizeof(XCHAR), str, length * sizeof(XCHAR));
        result.ReleaseBuffer();

        return result;
    }

    public: iStringT Split(int index, LPCXSTR seperator)
    {
        iStringT result;
        LPXSTR str = stringData;
        int step = StringLength(seperator);

        while (index--)
        {
            str = FindString(str, seperator);
            if (str == NULL)
                return result;

            str += step;
        }

        LPCXSTR end = FindString(str, seperator);
        int length = (end == NULL) ? StringLength(str) : (int)(end - str);
        ASSERT(length >= 0);
        memcpy_s(result.GetBufferSetLength(length), length * sizeof(XCHAR), str, length * sizeof(XCHAR));
        result.ReleaseBuffer();

        return result;
    }

    public: iList<iStringT> Split(TCHAR seperator)
    {
        iList<iStringT> result;
        LPXSTR str = stringData;

        while (str != NULL)
        {
            LPCXSTR end = FindChar(str, seperator);
            int length = (end == NULL) ? StringLength(str) : (int)(end - str);
            ASSERT(length >= 0);

            iStringT token;
            memcpy_s(token.GetBufferSetLength(length), length * sizeof(XCHAR), str, length * sizeof(XCHAR));
            token.ReleaseBuffer();
            result.AddTail(token);

            str = FindChar(str, seperator);
            if (str == NULL)
                break;

            str++;
        }

        return result;
    }

    public: iList<iStringT> Split(LPCXSTR seperator)
    {
        iList<iStringT> result;
        LPXSTR str = stringData;
        int step = StringLength(seperator);

        while (str != NULL)
        {
            LPCXSTR end = FindString(str, seperator);
            int length = (end == NULL) ? StringLength(str) : (int)(end - str);
            ASSERT(length >= 0);

            iStringT token;
            memcpy_s(token.GetBufferSetLength(length), length * sizeof(XCHAR), str, length * sizeof(XCHAR));
            token.ReleaseBuffer();
            result.AddTail(token);

            str = FindString(str, seperator);
            if (str == NULL)
                break;

            str += step;
        }

        return result;
    }

    public: iStringT Mid(int first, int count) const
    {
        if (first < 0)
            first = 0;

        if (count < 0)
            count = 0;

        if (first + count > GetData()->dataLength)
            count = GetData()->dataLength - first;

        if (first > GetData()->dataLength)
            count = 0;

        ASSERT(first >= 0);
        ASSERT(first + count <= GetData()->dataLength);

        if (first == 0 && first + count == GetData()->dataLength)
            return *this;

        iStringT result;
        AllocCopy(result, count, first, 0);

        return result;
    }

    public: iStringT Mid(int first) const
    {
        return Mid(first, GetData()->dataLength - first);
    }

    public: iStringT Left(int count) const
    {
        if (count < 0)
            count = 0;

        if (count >= GetData()->dataLength)
            return *this;

        iStringT result;
        AllocCopy(result, count, 0, 0);

        return result;
    }

    public: iStringT Right(int count) const
    {
        if (count < 0)
            count = 0;

        if (count >= GetData()->dataLength)
            return *this;

        iStringT result;
        AllocCopy(result, count, GetData()->dataLength - count, 0);

        return result;
    }

    public: iStringT SpanIncluding(LPCXSTR charSet) const
    {
        ASSERT(charSet);
        return Left(SpanIncluding(stringData, charSet));
    }

    public: iStringT SpanExcluding(LPCXSTR charSet) const
    {
        ASSERT(charSet);
        return Left(SpanExcluding(stringData, charSet));
    }

    public: void MakeUpper()
    {
        CopyBeforeWrite();
        MakeUpper(stringData);
    }

    public: void MakeLower()
    {
        CopyBeforeWrite();
        MakeLower(stringData);
    }

    public: void MakeReverse()
    {
        CopyBeforeWrite();
        MakeReverse(stringData);
    }

    public: void Trim()
    {
        TrimLeft();
        TrimRight();
    }

    public: void TrimRight()
    {
        CopyBeforeWrite();

        LPXSTR str = stringData;
        LPXSTR last = NULL;

        while (*str != '\0')
        {
            if (IsSpace(*str))
            {
                if (last == NULL)
                    last = str;
            }
            else
            {
                last = NULL;
            }
            str = CharNext(str);
        }

        if (last != NULL)
        {
            *last = '\0';
            GetData()->dataLength = int(last - stringData);
        }
    }

    public: void TrimLeft()
    {
        CopyBeforeWrite();

        LPCXSTR str = stringData;

        while (IsSpace(*str))
            str = CharNext(str);

        if (str != stringData)
        {
            int dataLength = GetData()->dataLength - int(str - stringData);
            memmove(stringData, str, (dataLength + 1) * sizeof(XCHAR));
            GetData()->dataLength = dataLength;
        }
    }

    public: void TrimRight(XCHAR ch)
    {
        CopyBeforeWrite();

        LPXSTR str = stringData;
        LPXSTR last = NULL;

        while (*str != '\0')
        {
            if (*str == ch)
            {
                if (last == NULL)
                    last = str;
            }
            else
            {
                last = NULL;
            }
            str = CharNext(str);
        }

        if (last != NULL)
        {
            *last = '\0';
            GetData()->dataLength = int(last - stringData);
        }
    }

    public: void TrimRight(LPCXSTR charSet)
    {
        CopyBeforeWrite();

        LPXSTR str = stringData;
        LPXSTR last = NULL;

        while (*str != '\0')
        {
            if (FindChar(charSet, *str) != NULL)
            {
                if (last == NULL)
                    last = str;
            }
            else
            {
                last = NULL;
            }
            str = CharNext(str);
        }

        if (last != NULL)
        {
            *last = '\0';
            GetData()->dataLength = int(last - stringData);
        }
    }

    public: void TrimLeft(XCHAR ch)
    {
        CopyBeforeWrite();

        LPCXSTR str = stringData;

        while (ch == *str)
            str = CharNext(str);

        if (str != stringData)
        {
            int dataLength = GetData()->dataLength - int(str - stringData);
            memmove(stringData, str, (dataLength + 1) * sizeof(XCHAR));
            GetData()->dataLength = dataLength;
        }
    }

    public: void TrimLeft(LPCXSTR charSet)
    {
        if (StringLength(charSet) == 0)
            return;

        CopyBeforeWrite();

        LPCXSTR str = stringData;

        while (*str != '\0')
        {
            if (FindChar(charSet, *str) == NULL)
                break;

            str = CharNext(str);
        }

        if (str != stringData)
        {
            int dataLength = GetData()->dataLength - int(str - stringData);
            memmove(stringData, str, (dataLength + 1) * sizeof(XCHAR));
            GetData()->dataLength = dataLength;
        }
    }

    public: int Replace(XCHAR oldChar, XCHAR newChar)
    {
        int count = 0;

        if (oldChar != newChar)
        {
            CopyBeforeWrite();

            LPXSTR str = stringData;
            LPXSTR end = str + GetData()->dataLength;

            while (str < end)
            {
                if (*str == oldChar)
                {
                    *str = newChar;
                    count++;
                }
                str = CharNext(str);
            }
        }

        return count;
    }

    public: int Replace(LPCXSTR oldStr, LPCXSTR newStr)
    {
        int sourceLength = StringLength(oldStr);
        if (sourceLength == 0)
            return 0;

        int count = 0;
        int replacementLength = StringLength(newStr);
        LPXSTR start = stringData;
        LPXSTR end = stringData + GetData()->dataLength;
        LPXSTR find;

        while (start < end)
        {
            while ((find = FindString(start, oldStr)) != NULL)
            {
                count++;
                start = find + sourceLength;
            }
            start += StringLength(start) + 1;
        }

        if (count > 0)
        {
            CopyBeforeWrite();

            int oldLength = GetData()->dataLength;
            int newLength = oldLength + (replacementLength - sourceLength) * count;

            if (GetData()->allocLength < newLength || GetData()->refs > 1)
            {
                iStringDataT<T>* oldData = GetData();
                LPXSTR temp = stringData;
                AllocBuffer(newLength);
                memcpy(stringData, temp, oldData->dataLength * sizeof(XCHAR));
                Release(oldData);
            }

            start = stringData;
            end = stringData + GetData()->dataLength;

            while (start < end)
            {
                while ((find = FindString(start, oldStr)) != NULL)
                {
                    int balance = oldLength - (find - stringData + sourceLength);
                    memmove(find + replacementLength, find + sourceLength, balance * sizeof(XCHAR));
                    memcpy(find, newStr, replacementLength * sizeof(XCHAR));
                    start = find + replacementLength;
                    start[balance] = '\0';
                    oldLength += (replacementLength - sourceLength);
                }
                start += StringLength(start) + 1;
            }

            ASSERT(stringData[newLength] == '\0');
            GetData()->dataLength = newLength;
        }

        return count;
    }

    public: int Remove(XCHAR ch)
    {
        CopyBeforeWrite();

        LPXSTR source = stringData;
        LPXSTR dest = stringData;
        LPXSTR end = stringData + GetData()->dataLength;

        while (source < end)
        {
            if (*source != ch)
            {
                *dest = *source;
                dest = _tcsinc(dest);
            }
            source = CharNext(source);
        }

        *dest = '\0';
        int count = source - dest;
        GetData()->dataLength -= count;

        return count;
    }

    public: int Insert(int index, XCHAR ch)
    {
        CopyBeforeWrite();

        if (index < 0)
            index = 0;

        int newLength = GetData()->dataLength;
        if (index > newLength)
            index = newLength;

        newLength++;

        if (GetData()->allocLength < newLength)
        {
            iStringDataT<T>* oldData = GetData();
            LPXSTR temp = stringData;
            AllocBuffer(newLength);
            memcpy(stringData, temp, (oldData->dataLength + 1) * sizeof(XCHAR));
            Release(oldData);
        }

        memcpy(stringData + index + 1, stringData + index, (newLength - index) * sizeof(XCHAR));
        stringData[index] = ch;
        GetData()->dataLength = newLength;

        return newLength;
    }

    public: int Insert(int index, LPCXSTR str)
    {
        if (index < 0)
            index = 0;

        int insertLength = StringLength(str);
        int newLength = GetData()->dataLength;

        if (insertLength > 0)
        {
            CopyBeforeWrite();

            if (index > newLength)
                index = newLength;

            newLength += insertLength;

            if (GetData()->allocLength < newLength)
            {
                iStringDataT<T>* oldData = GetData();
                LPXSTR temp = stringData;
                AllocBuffer(newLength);
                memcpy(stringData, temp, (oldData->dataLength + 1) * sizeof(XCHAR));
                Release(oldData);
            }

            memcpy(stringData + index + insertLength, stringData + index, (newLength - index - insertLength + 1) * sizeof(XCHAR));
            memcpy(stringData + index, str, insertLength * sizeof(XCHAR));
            GetData()->dataLength = newLength;
        }

        return newLength;
    }

    public: int Delete(int index, int count = 1)
    {
        if (index < 0)
            index = 0;

        int newLength = GetData()->dataLength;

        if (count > 0 && index < newLength)
        {
            CopyBeforeWrite();
            int bytesToCopy = newLength - (index + count) + 1;
            memcpy(stringData + index, stringData + index + count, bytesToCopy * sizeof(XCHAR));
            GetData()->dataLength = newLength - count;
        }

        return newLength;
    }

    public: int Find(XCHAR ch) const
    {
        return Find(ch, 0);
    }

    public: int ReverseFind(XCHAR ch) const
    {
        LPTSTR str = FindCharRev(stringData, ch);
        return (str == NULL) ? -1 : (int)(str - stringData);
    }

    public: int Find(XCHAR ch, int start) const
    {
        int length = GetData()->dataLength;
        if (start >= length)
            return -1;

        LPTSTR str = FindChar(stringData + start, ch);
        return (str == NULL) ? -1 : (int)(str - stringData);
    }

    public: int FindOneOf(LPCXSTR str) const
    {
        ASSERT(str);
        LPTSTR found = ScanSet(stringData, str);
        return (found == NULL) ? -1 : (int)(found - stringData);
    }

    public: int Find(LPCXSTR str) const
    {
        return Find(str, 0);
    }

    public: int Find(LPCXSTR str, int start) const
    {
        ASSERT(str);

        int length = GetData()->dataLength;
        if (start > length)
            return -1;

        LPXSTR found = FindString(stringData + start, str);
        return (found == NULL) ? -1 : (int)(found - stringData);
    }

    public: void Format(LPCXSTR format, ...)
    {
        ASSERT(format);

        va_list args;
        va_start(args, format);
        FormatV(format, args);
        va_end(args);
    }

    public: void FormatV(LPCXSTR format, va_list args)
    {
        int length = GetFormattedLength(format, args);
        LPXSTR buffer = GetBuffer(length);
        FormatV(buffer, length + 1, format, args);
        ReleaseBuffer();
    }

    public: void FormatMessage(LPCTSTR format, ...)
    {
        va_list args;
        va_start(args, format);

        LPXSTR temp;
        if (::FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER, format, 0, 0, (LPXSTR)&temp, 0, &args) != 0 && temp != NULL)
        {
            *this = temp;
            LocalFree(temp);
        }

        va_end(args);
    }

    public: LPXSTR GetBuffer(int length)
    {
        ASSERT(length >= 0);

        if (GetData()->refs > 1 || length > GetData()->allocLength)
        {
            iStringDataT<T>* oldData = GetData();
            int oldLength = GetData()->dataLength;

            if (length < oldLength)
                length = oldLength;

            AllocBuffer(length);
            memcpy(stringData, oldData->GetData(), (oldLength + 1) * sizeof(XCHAR));
            GetData()->dataLength = oldLength;
            Release(oldData);
        }

        ASSERT(GetData()->refs <= 1);
        ASSERT(stringData != NULL);

        return stringData;
    }

    public: void ReleaseBuffer(int newLength = -1)
    {
        CopyBeforeWrite();

        if (newLength == -1)
            newLength = StringLength(stringData);

        ASSERT(newLength <= GetData()->allocLength);
        GetData()->dataLength = newLength;
        stringData[newLength] = '\0';
    }

    public: BSTR AllocSysString() const
    {
        return AllocSysString(stringData, GetData()->dataLength);
    }

    public: BSTR SetSysString(BSTR* str) const
    {
        VERIFY(SetSysString(stringData, str, GetData()->dataLength));
        ASSERT(str);
        return (*str);
    }

    public: LPXSTR GetBufferSetLength(int newLength)
    {
        ASSERT(newLength >= 0);

        GetBuffer(newLength);
        GetData()->dataLength = newLength;
        stringData[newLength] = '\0';

        return stringData;
    }

    public: void FreeExtra()
    {
        ASSERT(GetData()->dataLength <= GetData()->allocLength);

        if (GetData()->dataLength != GetData()->allocLength)
        {
            iStringDataT<T>* oldData = GetData();
            AllocBuffer(GetData()->dataLength);
            memcpy(stringData, oldData->GetData(), oldData->dataLength * sizeof(XCHAR));
            ASSERT(stringData[GetData()->dataLength] == '\0');
            Release(oldData);
        }

        ASSERT(GetData() != NULL);
    }

    public: LPXSTR LockBuffer()
    {
        LPXSTR str = GetBuffer(0);
        GetData()->refs = -1;
        return str;
    }

    public: void UnlockBuffer()
    {
        ASSERT(GetData()->refs == -1);

        if (GetData()->refs != CHNIL)
            GetData()->refs = 1;
    }

    protected: void Append(int length, LPCXSTR str)
    {
        if (length == 0)
            return;

        if (GetData()->refs > 1 || GetData()->dataLength + length > GetData()->allocLength)
        {
            iStringDataT<T>* oldData = GetData();
            Concatenate(GetData()->dataLength, stringData, length, str);
            ASSERT(oldData != NULL);
            Release(oldData);
        }
        else
        {
            memcpy(stringData + GetData()->dataLength, str, length * sizeof(XCHAR));
            GetData()->dataLength += length;
            ASSERT(GetData()->dataLength <= GetData()->allocLength);
            stringData[GetData()->dataLength] = '\0';
        }
    }

    protected: void Concatenate(int length1, LPCXSTR str1, int length2, LPCXSTR str2)
    {
        int nNewLen = length1 + length2;
        if (nNewLen != 0)
        {
            AllocBuffer(nNewLen);
            memcpy(stringData, str1, length1 * sizeof(XCHAR));
            memcpy(stringData + length1, str2, length2 * sizeof(XCHAR));
        }
    }

    protected: iStringDataT<T>* GetData() const
    {
        ASSERT(stringData);
        return ((iStringDataT<T>*)stringData) - 1;
    }

    protected: void AssignCopy(int length, LPCXSTR str)
    {
        AllocBeforeWrite(length);
        memcpy(stringData, str, length * sizeof(XCHAR));
        GetData()->dataLength = length;
        stringData[length] = '\0';
    }

    protected: void AssignConvertCopy(int length, LPCSTR str)
    {
        AllocBeforeWrite(length);
        A2W(stringData, str, length);
        ReleaseBuffer();
    }

    protected: void AssignConvertCopy(int length, LPCWSTR str)
    {
        AllocBeforeWrite(length * 2);
        W2A(stringData, str, length);
        ReleaseBuffer();
    }

    protected: void AllocBeforeWrite(int length)
    {
        if (GetData()->refs > 1 || length > GetData()->allocLength)
        {
            Release();
            AllocBuffer(length);
        }

        ASSERT(GetData()->refs <= 1);
    }

    protected: protected: void Release()
    {
        if (GetData()->refs != CHNIL)
        {
            ASSERT(GetData()->refs != 0);

            if (InterlockedDecrement(&GetData()->refs) <= 0)
                FreeData(GetData());

            Initialize();
        }
    }

    protected: void AllocBuffer(int length)
    {
        ASSERT(length >= 0);
        ASSERT(length <= INT_MAX - 1);

        if (length == 0)
        {
            Initialize();
        }
        else
        {
            iStringDataT<T>* newData;

#ifdef FIXEDALLOC
            if (length <= 64)
            {
                newData = (iStringDataT<T>*)alloc64.Alloc();
                newData->allocLength = 64;
            }
            else if (length <= 128)
            {
                newData = (iStringDataT<T>*)alloc128.Alloc();
                newData->allocLength = 128;
            }
            else if (length <= 256)
            {
                newData = (iStringDataT<T>*)alloc256.Alloc();
                newData->allocLength = 256;
            }
            else if (length <= 512)
            {
                newData = (iStringDataT<T>*)alloc512.Alloc();
                newData->allocLength = 512;
            }
            else
#endif
            {
                newData = (iStringDataT<T>*) new BYTE[sizeof(iStringDataT<T>) + (length + 1) * sizeof(XCHAR)];
                newData->allocLength = length;
            }

            newData->refs = 1;
            newData->GetData()[length] = '\0';
            newData->dataLength = length;
            stringData = newData->GetData();
        }
    }

    protected: void CopyBeforeWrite()
    {
        if (GetData()->refs > 1)
        {
            iStringDataT<T>* copy = GetData();
            Release();
            AllocBuffer(copy->dataLength);
            memcpy(stringData, copy->GetData(), (copy->dataLength + 1) * sizeof(XCHAR));
        }

        ASSERT(GetData()->refs <= 1);
    }

    protected: void AllocCopy(iStringT& dest, int copyLength, int copyIndex, int extraLength) const
    {
        int nNewLen = copyLength + extraLength;
        if (nNewLen == 0)
        {
            dest.Initialize();
        }
        else
        {
            dest.AllocBuffer(nNewLen);
            memcpy(dest.stringData, stringData + copyIndex, copyLength * sizeof(XCHAR));
        }
    }

    public: static const iStringT& GetEmptyString()
    {
        static LPCXSTR nil = (LPCXSTR)(((BYTE*)&initData) + sizeof(iStringDataT<T>));
        return *(iStringT*)&nil;
    }

    public: static void Release(iStringDataT<T>* data)
    {
        if (data->refs != CHNIL)
        {
            ASSERT(data->refs != 0);

            if (InterlockedDecrement(&data->refs) <= 0)
                FreeData(data);
        }
    }

    public: static int StringLength(LPCSTR str)
    {
        return (str == NULL) ? 0 : static_cast<int>(strlen(str));
    }

    public: static int StringLength(LPCWSTR str)
    {
        return (str == NULL) ? 0 : static_cast<int>(wcslen(str));
    }

    public: static void FreeData(iStringDataT<T>* data)
    {
        int length = data->allocLength;

#ifdef FIXEDALLOC
        if (length == 64)
            alloc64.Free(data);
        else if (length == 128)
            alloc128.Free(data);
        else if (length == 256)
            alloc256.Free(data);
        else  if (length == 512)
            alloc512.Free(data);
        else
#endif
        delete [] (BYTE*)data;
    }

    public: static void FloodCharacters(LPSTR str, char ch, int repeat)
    {
        memset(str, ch, repeat);
    }

    public: static void FloodCharacters(LPWSTR str, wchar_t ch, int repeat)
    {
        for (int i = 0; i < repeat; ++i)
            str[i] = ch;
    }

    public: static int W2A(char* mbstr, const wchar_t* wcstr, int count)
    {
        if (count == 0 && mbstr != NULL)
            return 0;

        int length = WideCharToMultiByte(CP_ACP, 0, wcstr, count, mbstr, (count * 2) + 1, NULL, NULL);

        ASSERT(mbstr == NULL || length <= (count * 2) + 1);

        if (length > 0)
            mbstr[length] = 0;

        return length;
    }

    public: static int A2W(wchar_t* wcstr, const char* mbstr, int count)
    {
        if (count == 0 && wcstr != NULL)
            return 0;

        int length = MultiByteToWideChar(CP_ACP, 0, mbstr, count, wcstr, count + 1);

        ASSERT(wcstr == NULL || length <= count + 1);

        if (length > 0)
            wcstr[length] = 0;

        return length;
    }

    public: static int Compare(LPCSTR str1, LPCSTR str2)
    {
        return _mbscmp((const unsigned char*)(str1), (const unsigned char*)(str2));
    }

    public: static int Compare(LPCWSTR str1, LPCWSTR str2)
    {
        return wcscmp(str1, str2);
    }

    public: static int CompareNoCase(LPCSTR str1, LPCSTR str2)
    {
        return _mbsicmp((const unsigned char*)(str1), (const unsigned char*)(str2));
    }

    public: static int CompareNoCase(LPCWSTR str1, LPCWSTR str2)
    {
        return _wcsicmp(str1, str2);
    }

    public: static int Collate(LPCSTR str1, LPCSTR str2)
    {
        return _mbscoll((const unsigned char*)(str1), (const unsigned char*)(str2));
    }

    public: static int Collate(LPCWSTR str1, LPCWSTR str2)
    {
        return wcscoll(str1, str2);
    }

    public: static int CollateNoCase(LPCSTR str1, LPCSTR str2)
    {
        return _mbsicoll((const unsigned char*)(str1), (const unsigned char*)(str2));
    }

    public: static int CollateNoCase(LPCWSTR str1, LPCWSTR str2)
    {
        return _wcsicoll(str1, str2);
    }

    public: static int SpanIncluding(LPCSTR str1, LPCSTR str2)
    {
        return _mbsspn((const unsigned char*)(str1), (const unsigned char*)(str2));
    }

    public: static int SpanIncluding(LPCWSTR str1, LPCWSTR str2)
    {
        return wcsspn(str1, str2);
    }

    public: static int SpanExcluding(LPCSTR str1, LPCSTR str2)
    {
        return _mbscspn((const unsigned char*)(str1), (const unsigned char*)(str2));
    }

    public: static int SpanExcluding(LPCWSTR str1, LPCWSTR str2)
    {
        return wcscspn(str1, str2);
    }

    public: static void MakeUpper(LPSTR str)
    {
        _mbsupr_s((unsigned char*)(str), strlen(str) + 1);
    }

    public: static void MakeUpper(LPWSTR str)
    {
        _wcsupr_s(str, wcslen(str) + 1);
    }

    public: static void MakeLower(LPSTR str)
    {
        _mbslwr_s((unsigned char*)(str), strlen(str) + 1);
    }

    public: static void MakeLower(LPWSTR str)
    {
        _wcslwr_s(str, wcslen(str) + 1);
    }

    public: static void MakeReverse(LPSTR str)
    {
        _mbsrev((unsigned char*)str);
    }

    public: static void MakeReverse(LPWSTR str)
    {
        _wcsrev(str);
    }

    public: static int IsSpace(char ch)
    {
        return _ismbcspace(ch);
    }

    public: static int IsSpace(wchar_t ch)
    {
        return iswspace(ch);
    }

    public: static LPSTR CharNext(LPCSTR str)
    {
        return (LPSTR)_mbsinc((const unsigned char*)str);
    }

    public: static LPWSTR CharNext(LPCWSTR str)
    {
        return (LPWSTR)(str + 1);
    }

    public: static LPSTR FindChar(LPCSTR str, char ch)
    {
        return (LPSTR)_mbschr((const unsigned char*)(str), (unsigned char)(ch));
    }

    public: static LPWSTR FindChar(LPCWSTR str, wchar_t ch)
    {
        return (LPWSTR)wcschr(str, ch);
    }

    public: static LPSTR FindCharRev(LPCSTR str, char ch)
    {
        return (LPSTR)_mbsrchr((const unsigned char*)(str), (unsigned char)(ch));
    }

    public: static LPWSTR FindCharRev(LPCWSTR str, wchar_t ch)
    {
        return (LPWSTR)wcsrchr(str, ch);
    }

    public: static LPSTR FindString(LPCSTR str1, LPCSTR str2)
    {
        return (LPSTR)(_mbsstr((const unsigned char*)(str1), (const unsigned char*)(str2)));
    }

    public: static LPWSTR FindString(LPCWSTR str1, LPCWSTR str2)
    {
        return (LPWSTR)(wcsstr(str1, str2));
    }

    public: static LPSTR ScanSet(LPCSTR str1, LPCSTR str2)
    {
        return (LPSTR)(_mbspbrk((const unsigned char*)(str1), (const unsigned char*)(str2)));
    }

    public: static LPWSTR ScanSet(LPCWSTR str1, LPCWSTR str2)
    {
        return (LPWSTR)wcspbrk(str1, str2);
    }

    public: static int GetFormattedLength(LPCSTR format, va_list args)
    {
        return _vscprintf(format, args);
    }

    public: static int GetFormattedLength(LPCWSTR format, va_list args)
    {
        return _vscwprintf(format, args);
    }

    public: static int FormatV(LPSTR buffer, int bufSize, LPCSTR format, va_list args)
    {
        return vsprintf_s(buffer, bufSize, format, args);
    }

    public: static int FormatV(LPWSTR buffer, int bufSize, LPCWSTR format, va_list args)
    {
        return vswprintf(buffer, bufSize, format, args);
    }

    public: static BSTR AllocSysString(LPCSTR source, int length)
    {
        int wideLength = MultiByteToWideChar(CP_ACP, 0, source, length, NULL, NULL);
        BSTR bstr = SysAllocStringLen(NULL, wideLength);

        if (bstr != NULL)
            MultiByteToWideChar(CP_ACP, 0, source, length, bstr, wideLength);

        return bstr;
    }

    public: static BSTR AllocSysString(LPCWSTR source, int length)
    {
        return SysAllocStringLen(source, length);
    }

    public: static BOOL SetSysString(LPCSTR source, BSTR* bstr, int length)
    {
        int wideLength = MultiByteToWideChar(CP_ACP, 0, source, length, NULL, NULL);
        BOOL success = SysReAllocStringLen(bstr, NULL, wideLength);

        if (success)
            MultiByteToWideChar(CP_ACP, 0, source, length, *bstr, wideLength);

        return success;
    }

    public: static BOOL SetSysString(LPCWSTR source, BSTR* bstr, int length)
    {
        return SysReAllocStringLen(bstr, source, length);
    }
};

template <class T> T iStringT<T>::chNil = '\0';
template <class T> int iStringT<T>::initData[] = { CHNIL, 0, 0, 0 };
template <class T> const iStringT<T> iStringT<T>::empty;

#ifdef FIXEDALLOC
template <class T> iFixedAlloc iStringT<T>::alloc64  = iFixedAlloc(ROUND4( 65 * sizeof(T) + sizeof(iStringDataT<T>)));
template <class T> iFixedAlloc iStringT<T>::alloc128 = iFixedAlloc(ROUND4(129 * sizeof(T) + sizeof(iStringDataT<T>)));
template <class T> iFixedAlloc iStringT<T>::alloc256 = iFixedAlloc(ROUND4(257 * sizeof(T) + sizeof(iStringDataT<T>)));
template <class T> iFixedAlloc iStringT<T>::alloc512 = iFixedAlloc(ROUND4(513 * sizeof(T) + sizeof(iStringDataT<T>)));
#endif

typedef iStringT<wchar_t> iStringW;
typedef iStringT<char> iStringA;
typedef iStringT<TCHAR> iString;

template <> void ConstructElements<iString>(iString* elements, int count);
template <> UINT HashKey<LPCWSTR>(LPCWSTR key);
template <> UINT HashKey<LPCSTR>(LPCSTR key);
template <> UINT HashKey<const iString&>(const iString& key);

#pragma pack(pop)
