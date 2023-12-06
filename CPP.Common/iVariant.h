#pragma once

class iVariant : public VARIANT
{
    public: typedef iMap<iString, iVariant*> iData;

    protected: BOOL dirty;
    protected: iString value;
    protected: iVariant::iData data;
    protected: static iVariant empty;

    public: iVariant()
    {
        VariantInit(this);
        SetDirty(TRUE);
    }

    public: ~iVariant()
    {
        Cleanup();
    }

    public: iVariant(const iVariant& value)
    {
        VariantInit(this);
        Setup(value);
    }

    public: iVariant(const VARIANT& value)
    {
        vt = VT_EMPTY;
        InternalCopy(&value);
    }

    public: iVariant(LPCOLESTR value)
    {
        VariantInit(this);
        *this = value;
        SetDirty(TRUE);
    }

    public: iVariant(LPCSTR value)
    {
        VariantInit(this);
        *this = value;
        SetDirty(TRUE);
    }

    public: iVariant(bool value)
    {
        vt = VT_BOOL;
        boolVal = value ? VARIANT_TRUE : VARIANT_FALSE;
        SetDirty(TRUE);
    }

    public: iVariant(INT value, VARTYPE type = VT_I4)
    {
        ASSERT(type == VT_I4 || type == VT_INT);
        vt = type;
        intVal = value;
        SetDirty(TRUE);
    }

    public: iVariant(BYTE value)
    {
        vt = VT_UI1;
        bVal = value;
        SetDirty(TRUE);
    }

    public: iVariant(SHORT value)
    {
        vt = VT_I2;
        iVal = value;
        SetDirty(TRUE);
    }

    public: iVariant(LONG value, VARTYPE type = VT_I4)
    {
        ASSERT(type == VT_I4 || type == VT_ERROR);
        vt = type;
        lVal = value;
        SetDirty(TRUE);
    }

    public: iVariant(FLOAT value)
    {
        vt = VT_R4;
        fltVal = value;
        SetDirty(TRUE);
    }

    public: iVariant(DOUBLE value, VARTYPE type = VT_R8)
    {
        ASSERT(type == VT_R8 || type == VT_DATE);
        vt = type;
        dblVal = value;
        SetDirty(TRUE);
    }

    public: iVariant(LONGLONG value)
    {
        vt = VT_I8;
        llVal = value;
        SetDirty(TRUE);
    }

    public: public: iVariant(ULONGLONG value)
    {
        vt = VT_UI8;
        ullVal = value;
        SetDirty(TRUE);
    }

    public: iVariant(CY value)
    {
        vt = VT_CY;
        cyVal.Hi = value.Hi;
        cyVal.Lo = value.Lo;
        SetDirty(TRUE);
    }

    public: iVariant(LPDISPATCH value)
    {
        vt = VT_DISPATCH;
        pdispVal = value;

        if (pdispVal != NULL)
            pdispVal->AddRef();

        SetDirty(TRUE);
    }

    public: iVariant(LPUNKNOWN value)
    {
        vt = VT_UNKNOWN;
        punkVal = value;

        if (punkVal != NULL)
            punkVal->AddRef();

        SetDirty(TRUE);
    }

    public: iVariant(CHAR value)
    {
        vt = VT_I1;
        cVal = value;
        SetDirty(TRUE);
    }

    public: iVariant(USHORT value)
    {
        vt = VT_UI2;
        uiVal = value;
        SetDirty(TRUE);
    }

    public: iVariant(ULONG value)
    {
        vt = VT_UI4;
        ulVal = value;
        SetDirty(TRUE);
    }

    public: iVariant(UINT value, VARTYPE type = VT_UI4)
    {
        ASSERT(type == VT_UI4 || type == VT_UINT);
        vt = type;
        uintVal = value;
        SetDirty(TRUE);
    }

    public: iVariant(const SAFEARRAY* value)
    {
        LPSAFEARRAY copyArray;
        if (value != NULL)
        {
            HRESULT result = SafeArrayCopy((LPSAFEARRAY)value, &copyArray);
            if (SUCCEEDED(result) && copyArray != NULL)
            {
                SafeArrayGetVartype((LPSAFEARRAY)value, &vt);
                vt |= VT_ARRAY;
                parray = copyArray;
            }
            else
            {
                vt = VT_ERROR;
                scode = result;
            }

            SetDirty(TRUE);
        }
        else
        {
            SetDirty(FALSE);
        }
    }

    public: iVariant& operator = (const iVariant& value)
    {
        Setup(value);
        return *this;
    }

    public: iVariant& operator = (const VARIANT& value)
    {
        InternalCopy(&value);
        return *this;
    }

    public: iVariant& operator = (LPCOLESTR value)
    {
        Clear();
        vt = VT_BSTR;
        bstrVal = SysAllocString(value);

        if (bstrVal == NULL && value != NULL)
        {
            vt = VT_ERROR;
            scode = E_OUTOFMEMORY;
        }

        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (LPCSTR value)
    {
        Clear();
        vt = VT_BSTR;
        iString str(value);
        bstrVal = str.AllocSysString();

        if (bstrVal == NULL && value != NULL)
        {
            vt = VT_ERROR;
            scode = E_OUTOFMEMORY;
        }

        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (bool value)
    {
        if (vt != VT_BOOL)
        {
            Clear();
            vt = VT_BOOL;
        }

        boolVal = value ? VARIANT_TRUE : VARIANT_FALSE;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (INT value)
    {
        if (vt != VT_I4)
        {
            Clear();
            vt = VT_I4;
        }

        intVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (BYTE value)
    {
        if (vt != VT_UI1)
        {
            Clear();
            vt = VT_UI1;
        }

        bVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (SHORT value)
    {
        if (vt != VT_I2)
        {
            Clear();
            vt = VT_I2;
        }

        iVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (LONG value)
    {
        if (vt != VT_I4)
        {
            Clear();
            vt = VT_I4;
        }

        lVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (FLOAT value)
    {
        if (vt != VT_R4)
        {
            Clear();
            vt = VT_R4;
        }

        fltVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (DOUBLE value)
    {
        if (vt != VT_R8)
        {
            Clear();
            vt = VT_R8;
        }

        dblVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (CY value)
    {
        if (vt != VT_CY)
        {
            Clear();
            vt = VT_CY;
        }

        cyVal.Hi = value.Hi;
        cyVal.Lo = value.Lo;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (LPDISPATCH value)
    {
        Clear();
        vt = VT_DISPATCH;
        pdispVal = value;

        if (pdispVal != NULL)
            pdispVal->AddRef();

        return *this;
    }

    public: iVariant& operator = (LPUNKNOWN value)
    {
        Clear();
        vt = VT_UNKNOWN;
        punkVal = value;

        if (punkVal != NULL)
            punkVal->AddRef();

        return *this;
    }

    public: iVariant& operator = (CHAR value)
    {
        if (vt != VT_I1)
        {
            Clear();
            vt = VT_I1;
        }

        cVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (USHORT value)
    {
        if (vt != VT_UI2)
        {
            Clear();
            vt = VT_UI2;
        }

        uiVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (ULONG value)
    {
        if (vt != VT_UI4)
        {
            Clear();
            vt = VT_UI4;
        }

        ulVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (UINT value)
    {
        if (vt != VT_UI4)
        {
            Clear();
            vt = VT_UI4;
        }

        uintVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (LPBYTE value)
    {
        if (vt != (VT_UI1 | VT_BYREF))
        {
            Clear();
            vt = VT_UI1 | VT_BYREF;
        }

        pbVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (PSHORT value)
    {
        if (vt != (VT_I2 | VT_BYREF))
        {
            Clear();
            vt = VT_I2 | VT_BYREF;
        }

        piVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (PUSHORT value)
    {
        if (vt != (VT_UI2 | VT_BYREF))
        {
            Clear();
            vt = VT_UI2 | VT_BYREF;
        }

        puiVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (PINT value)
    {
        if (vt != (VT_I4 | VT_BYREF))
        {
            Clear();
            vt = VT_I4 | VT_BYREF;
        }

        pintVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (PUINT value)
    {
        if (vt != (VT_UI4 | VT_BYREF))
        {
            Clear();
            vt = VT_UI4 | VT_BYREF;
        }

        puintVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (PLONG value)
    {
        if (vt != (VT_I4 | VT_BYREF))
        {
            Clear();
            vt = VT_I4 | VT_BYREF;
        }

        plVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (PULONG value)
    {
        if (vt != (VT_UI4 | VT_BYREF))
        {
            Clear();
            vt = VT_UI4 | VT_BYREF;
        }

        pulVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (LONGLONG value)
    {
        if (vt != VT_I8)
        {
            Clear();
            vt = VT_I8;
        }

        llVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (PLONGLONG value)
    {
        if (vt != (VT_I8 | VT_BYREF))
        {
            Clear();
            vt = VT_I8 | VT_BYREF;
        }

        pllVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (ULONGLONG value)
    {
        if (vt != VT_UI8)
        {
            Clear();
            vt = VT_UI8;
        }

        ullVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (PULONGLONG value)
    {
        if (vt != (VT_UI8 | VT_BYREF))
        {
            Clear();
            vt = VT_UI8 | VT_BYREF;
        }

        pullVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (PFLOAT value)
    {
        if (vt != (VT_R4 | VT_BYREF))
        {
            Clear();
            vt = VT_R4 | VT_BYREF;
        }

        pfltVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (DOUBLE* value)
    {
        if (vt != (VT_R8 | VT_BYREF))
        {
            Clear();
            vt = VT_R8 | VT_BYREF;
        }

        pdblVal = value;
        SetDirty(TRUE);

        return *this;
    }

    public: iVariant& operator = (const SAFEARRAY* value)
    {
        Clear();
        LPSAFEARRAY copyArray;

        if (value != NULL)
        {
            HRESULT result = SafeArrayCopy((LPSAFEARRAY)value, &copyArray);
            if (SUCCEEDED(result) && copyArray != NULL)
            {
                SafeArrayGetVartype((LPSAFEARRAY)value, &vt);
                vt |= VT_ARRAY;
                parray = copyArray;
            }
            else
            {
                vt = VT_ERROR;
                scode = result;
            }
        }

        return *this;
    }

    public: iVariant& operator = (LPVOID value)
    {
        return operator = (PULONG_PTR(value));
    }

    public: iVariant& operator = (LPCVOID value)
    {
        return operator = (PULONG_PTR(value));
    }

    public: operator LPCTSTR ()
    {
        if (dirty)
        {
            this->value = *this;
            SetDirty(FALSE);
        }

        return this->value;
    }

    public: operator LPVOID ()
    {
        if ((vt & VT_BYREF) == VT_BYREF)
#ifdef _WIN64
            return (LPVOID)ULONG_PTR(this->pullVal);
#else
            return (LPVOID)ULONG_PTR(this->pulVal);
#endif
        return NULL;
    }

    public: operator BYTE ()
    {
        return (vt == VT_UI1) ? bVal : (BYTE)_tstoi((LPCTSTR)(*this));
    }

    public: operator INT ()
    {
        return (vt == VT_I4) ? intVal : _tstoi((LPCTSTR)(*this));
    }

    public: operator UINT ()
    {
        return (vt == VT_UI4) ? uintVal : (UINT)_tstoi((LPCTSTR)(*this));
    }

    public: operator LONG ()
    {
        return (vt == VT_I4) ? lVal : _tstol((LPCTSTR)(*this));
    }

    public: operator ULONG ()
    {
        return (vt == VT_UI4) ? ulVal : (ULONG)_tstol((LPCTSTR)(*this));
    }

    public: operator LONGLONG ()
    {
        return (vt == VT_I8) ? llVal : _tstoi64((LPCTSTR)(*this));
    }

    public: operator ULONGLONG ()
    {
        return (vt == VT_UI8) ? ullVal : (ULONGLONG)_tstoi64((LPCTSTR)(*this));
    }

    public: operator DOUBLE ()
    {
        return (vt == VT_R8) ? dblVal : _tstof((LPCTSTR)(*this));
    }

    public: operator FLOAT ()
    {
        return (vt == VT_R4) ? fltVal : (FLOAT)_tstof((LPCTSTR)(*this));
    }

    public: BOOL operator == (const VARIANT& operand) const
    {
        if (vt == VT_NULL && operand.vt == VT_NULL)
            return TRUE;

        return VarCmp((LPVARIANT)this, (LPVARIANT)&operand, LOCALE_USER_DEFAULT, 0) == VARCMP_EQ;
    }

    public: BOOL operator != (const VARIANT& operand) const
    {
        return !operator == (operand);
    }

    public: BOOL operator < (const VARIANT& operand) const
    {
        if (vt == VT_NULL && operand.vt == VT_NULL)
            return FALSE;

        return VarCmp((LPVARIANT)this, (LPVARIANT)&operand, LOCALE_USER_DEFAULT, 0) == VARCMP_LT;
    }

    public: BOOL operator > (const VARIANT& operand) const
    {
        if (vt == VT_NULL && operand.vt == VT_NULL)
            return FALSE;

        return VarCmp((LPVARIANT)this, (LPVARIANT)&operand, LOCALE_USER_DEFAULT, 0) == VARCMP_GT;
    }

    public: iVariant& operator [] (LPCTSTR key)
    {
        iString target(key);
        target.MakeUpper();

        iVariant* value = NULL;
        if (!data.Lookup(target, value))
        {
            value = new iVariant;
            data.SetAt(target, value);
        }

        return (value == NULL) ? empty : *value;
    }

    public: iVariant& operator [] (INT key)
    {
        iString target;
        target.Format(_T("%d"), key);
        return (*this)[target];
    }

    public: iVariant& operator [] (LONG key)
    {
        iString target;
        target.Format(_T("%ld"), key);
        return (*this)[target];
    }

    public: iVariant& operator [] (LONGLONG key)
    {
        iString target;
        target.Format(_T("%I64d"), key);
        return (*this)[target];
    }

    public: BOOL IsExistKey(LPCTSTR key) const
    {
        iString target(key);
        target.MakeUpper();

        iVariant* value = NULL;
        return data.Lookup(target, value);
    }

    public: BOOL IsExistKey(INT key) const
    {
        iString target;
        target.Format(_T("%d"), key);

        iVariant* value = NULL;
        return data.Lookup(target, value);
    }

    public: BOOL IsExistKey(LONG key) const
    {
        iString target;
        target.Format(_T("%ld"), key);

        iVariant* value = NULL;
        return data.Lookup(target, value);
    }

    public: BOOL IsExistKey(LONGLONG key) const
    {
        iString target;
        target.Format(_T("%I64d"), key);

        iVariant* value = NULL;
        return data.Lookup(target, value);
    }

    public: iString GetKeyFromValue(iVariant& value)
    {
        iString key;
        iVariant* target = NULL;

        for (POSITION pos = data.GetStartPosition(); pos != NULL;)
        {
            data.GetNext(pos, key, target);

            if (target == NULL)
                continue;

            if (iString(*target) == iString(value))
                return key;
        }

        return iString::empty;
    }

    public: void Remove(LPCTSTR key)
    {
        iString target(key);
        target.MakeUpper();

        iVariant* value = NULL;
        if (data.Lookup(target, value))
        {
            delete value;
            data.RemoveKey(target);
        }
    }

    public: void Remove(INT key)
    {
        iString target;
        target.Format(_T("%d"), key);
        Remove(target);
    }

    public: void Remove(LONG key)
    {
        iString target;
        target.Format(_T("%ld"), key);
        Remove(target);
    }

    public: void Remove(LONGLONG key)
    {
        iString target;
        target.Format(_T("%I64d"), key);
        Remove(target);
    }

    public: int GetCount() const
    {
        return data.GetCount();
    }

    public: const iVariant::iData& Data() const
    {
        return data;
    }

    public: void Merge(const iVariant& value)
    {
        if (this == &value)
            return;

        const iVariant::iData& vdata = value.data;

        iString key;
        iVariant* source;

        for (POSITION pos = vdata.GetStartPosition(); pos != NULL;)
        {
            source = NULL;
            key.Empty();

            vdata.GetNext(pos, key, source);
            if (!source) continue;

            (operator [] ((LPCTSTR)key)).Setup(*source);
        }
    }

    public: void Cleanup()
    {
        for (POSITION pos = data.GetStartPosition(); pos != NULL;)
        {
            iString key;
            iVariant* value = NULL;
            data.GetNext(pos, key, value);
            SAFE_DELETE(value);
        }

        data.RemoveAll();

        Clear();
    }

    public: void Log(int tab = 0)
    {
#ifdef _DEBUG
        iString print;
        iString temp;
        const iData::iNode* pair;

        if (++tab == 1)
            TRACE(_T("%s\n(\n"), (LPCTSTR)(*this));

        const iList<iData::iNode*>& list = data.ToList();
        for (POSITION pos = list.GetHeadPosition(); pos != NULL;)
        {
            pair = list.GetNext(pos);
            if (!pair->value) continue;

            print = _T("");

            for (int i = 0; i < tab; i++)
                print += _T("    ");

            temp.Format(_T("[%s] : %s\n"), pair->key, (LPCTSTR)(*pair->value));
            print += temp;

            if (pair->value->GetCount() > 0)
            {
                for (int i = 0; i < tab; i++)
                    print += _T("    ");

                print += _T("(\n");

                TRACE(_T("%s"), print);

                pair->value->Log(tab);
                print = _T("");

                for (int i = 0; i < tab; i++)
                    print += _T("    ");

                print += _T(")\n");
            }

            TRACE(_T("%s"), print);
        }

        if (tab == 1)
            TRACE(_T(")\n"));
#endif
    }

    public: HRESULT Clear()
    {
        SetDirty(TRUE);
        return VariantClear(this);
    }

    public: HRESULT Copy(const VARIANT* target)
    {
        SetDirty(TRUE);
        return VariantCopy(this, target);
    }

    public: HRESULT CopyTo(BSTR* target)
    {
        ASSERT(target != NULL && vt == VT_BSTR);
        HRESULT result = E_POINTER;

        if (target != NULL && vt == VT_BSTR)
        {
            *target = SysAllocStringByteLen((char*)bstrVal, SysStringByteLen(bstrVal));
            if (*target == NULL)
                result = E_OUTOFMEMORY;
            else
                result = S_OK;
        }
        else if (vt != VT_BSTR)
        {
            result = DISP_E_TYPEMISMATCH;
        }

        return result;
    }

    public: HRESULT Attach(LPVARIANT source)
    {
        if (source == NULL)
            return E_INVALIDARG;

        HRESULT hr = Clear();
        if (!FAILED(hr))
        {
            memcpy(this, source, sizeof(VARIANT));
            source->vt = VT_EMPTY;
            hr = S_OK;
        }

        return hr;
    }

    public: HRESULT Detach(LPVARIANT target)
    {
        ASSERT(target != NULL);
        if (target == NULL)
            return E_POINTER;

        HRESULT hr = VariantClear(target);
        if (!FAILED(hr))
        {
            memcpy(target, this, sizeof(VARIANT));
            vt = VT_EMPTY;
            hr = S_OK;
        }

        SetDirty(TRUE);

        return hr;
    }

    public: HRESULT ChangeType(VARTYPE type, const VARIANT* source = NULL)
    {
        SetDirty(TRUE);

        LPVARIANT variant = const_cast<LPVARIANT>(source);
        if (variant == NULL)
            variant = this;

        return VariantChangeType(this, variant, 0, type);
    }

    public: HRESULT InternalClear()
    {
        HRESULT result = Clear();
        ASSERT(SUCCEEDED(result));

        if (FAILED(result))
        {
            vt = VT_ERROR;
            scode = result;
        }

        return result;
    }

    public: void InternalCopy(const VARIANT* source)
    {
        HRESULT result = Copy(source);

        if (FAILED(result))
        {
            vt = VT_ERROR;
            scode = result;
        }

        SetDirty(TRUE);
    }

    protected: void SetDirty(BOOL dirty)
    {
        this->dirty = dirty;
    }

    protected: void Setup(const iVariant& value)
    {
        if (this == &value)
            return;

        Cleanup();
        Copy(&value);

        iString key;
        iVariant* to;
        iVariant* from;
        const iVariant::iData& vdata = value.data;

        for (POSITION pos = vdata.GetStartPosition(); pos != NULL;)
        {
            from = NULL;
            to = NULL;
            key.Empty();

            vdata.GetNext(pos, key, from);
            if (!from) continue;

            to = new iVariant;
            if (!to) continue;

            to->Setup(*from);
            data[key] = to;
        }
    }

    public: void Deserialize(LPCTSTR source)
    {
        ASSERT(source);
        OnDeserialize(source);
    }

    protected: LPCTSTR OnDeserialize(LPCTSTR source)
    {
        if (source[0] == 'a')
        {
            int i;
            for (i = 1; source[i]; i++)
            {
                if (source[i] == '{')
                    break;
            }

            source += i;

            if (source[0] != '{')
                return NULL;

            source++;

            while (TRUE)
            {
                if (source[0] == '}')
                    return source + 1;

                iString key;
                source = OnDeserialize(source, key);

                if (!source)
                    return NULL;

                iVariant* target = new iVariant;
                source = target->OnDeserialize(source);

                if (!source)
                {
                    delete target;
                    return NULL;
                }

                iVariant* value;
                if (data.Lookup(key, value))
                {
                    delete value;
                    data.RemoveKey(key);
                }

                data.SetAt(key, target);
            }
        }

        LPCTSTR next = OnDeserialize(source, value);
        if (next != NULL)
            *this = value;

        return next;
    }

    protected: LPCTSTR OnDeserialize(LPCTSTR source, iString& target)
    {
        int i;

        if (source[0] == 'N')
            return source + 2;

        else if (source[0] == 'i' || source[0] == 'b' || source[0] == 'd')
        {
            source += 2;
            for (i = 0; source[i]; i++)
            {
                if (source[i] == ';')
                    break;
            }

            if (source[i] == 0)
                return NULL;

            target = (iString(source, i));

            return source + i + 1;
        }
        else if (source[0] == 's')
        {
            if (source[1] != ':')
                return NULL;

            int length = _tstoi(source + 2);

            for (i = 0; source[i]; i++)
            {
                if (source[i] == '"')
                    break;
            }

            if (source[i] == 0)
                return NULL;

            source += i + 1;
            int available = (int)_tcslen(source);
            if (length > (available - 2))
                return NULL;

            if (source[length] != '"')
                return NULL;

            if (source[length + 1] != ';')
                return NULL;

            target = (iString(source, length));

            return source += length + 2;
        }

        return NULL;
    }

    public: void Serialize(iString& target)
    {
        iString packet;
        packet.Format(_T("a:%d:{"), data.GetCount());
        target += packet;

        for (POSITION pos = data.GetStartPosition(); pos;)
        {
            iString key;
            iVariant* value = NULL;
            data.GetNext(pos, key, value);

            if (value)
            {
                if (IsNumeric(key))
                    packet.Format(_T("i:%s;"), key);
                else
                    packet.Format(_T("s:%d:\"%s\";"), key.GetLength(), key);

                target += packet;

                if (value->data.GetCount() > 0)
                {
                    value->Serialize(target);
                }
                else
                {
                    LPCTSTR valueToken = *value;

                    if (IsNumeric(valueToken))
                        packet.Format(_T("i:%s;"), valueToken);
                    else
                        packet.Format(_T("s:%d:\"%s\";"), _tcslen(valueToken), valueToken);

                    target += packet;
                }
            }
        }

        target += _T("}");
    }

    public: void DeserializeJSON(LPCTSTR source)
    {
        // TODO: ...
    }

    public: void SerializeJSON(iString& target)
    {
        // TODO: ...
    }

    public: static BOOL IsNumeric(const iString& value)
    {
        if (value.IsEmpty())
            return FALSE;

        for (register int i = 0; i < value.GetLength(); ++i)
        {
            if (!_istdigit(value[i]))
                return FALSE;
        }

        return TRUE;
    }
};
