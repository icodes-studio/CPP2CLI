#pragma once

class iVariant : public VARIANT
{
    public: typedef iMap<iString, iVariant*, LPCTSTR> Array;

    protected: BOOL dirty;
    protected: iString stringify;
    protected: Array child;
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
            stringify = *this;
            SetDirty(FALSE);
        }

        return stringify;
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

    public: operator bool ()
    {
        return (vt == VT_BOOL) ? (boolVal == VARIANT_TRUE) : (_tstoi((LPCTSTR)(*this)) == VARIANT_TRUE);
    }

    public: operator CHAR ()
    {
        return (vt == VT_I1) ? cVal : (CHAR)_tstoi((LPCTSTR)(*this));
    }

    public: operator BYTE ()
    {
        return (vt == VT_UI1) ? bVal : (BYTE)_tstoi((LPCTSTR)(*this));
    }

    public: operator SHORT ()
    {
        return (vt == VT_I2) ? iVal : (SHORT)_tstoi((LPCTSTR)(*this));
    }

    public: operator USHORT ()
    {
        return (vt == VT_UI2) ? uiVal : (UINT)_tstoi((LPCTSTR)(*this));
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
        iVariant* value = NULL;
        if (!child.Lookup(key, value))
        {
            value = new iVariant;
            child.SetAt(key, value);
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
        iVariant* value = NULL;
        return child.Lookup(key, value);
    }

    public: BOOL IsExistKey(INT key) const
    {
        iString target;
        target.Format(_T("%d"), key);

        iVariant* value = NULL;
        return child.Lookup(target, value);
    }

    public: BOOL IsExistKey(LONG key) const
    {
        iString target;
        target.Format(_T("%ld"), key);

        iVariant* value = NULL;
        return child.Lookup(target, value);
    }

    public: BOOL IsExistKey(LONGLONG key) const
    {
        iString target;
        target.Format(_T("%I64d"), key);

        iVariant* value = NULL;
        return child.Lookup(target, value);
    }

    public: iString GetKeyFromValue(iVariant& value)
    {
        iString key;
        iVariant* target = NULL;

        for (POSITION pos = child.GetStartPosition(); pos != NULL;)
        {
            child.GetNext(pos, key, target);

            if (target == NULL)
                continue;

            if (iString(*target) == iString(value))
                return key;
        }

        return iString::empty;
    }

    public: void Remove(LPCTSTR key)
    {
        iVariant* value = NULL;
        if (child.Lookup(key, value))
        {
            delete value;
            child.RemoveKey(key);
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
        return child.GetCount();
    }

    public: const iVariant::Array& Child() const
    {
        return child;
    }

    public: void Merge(const iVariant& value)
    {
        if (this == &value)
            return;

        iString key;
        iVariant* source;

        for (POSITION pos = value.child.GetStartPosition(); pos != NULL;)
        {
            source = NULL;
            key.Empty();

            value.child.GetNext(pos, key, source);
            if (!source) continue;

            (operator [] ((LPCTSTR)key)).Setup(*source);
        }
    }

    public: void Cleanup()
    {
        for (POSITION pos = child.GetStartPosition(); pos != NULL;)
        {
            iString key;
            iVariant* value = NULL;
            child.GetNext(pos, key, value);
            SAFE_DELETE(value);
        }

        child.RemoveAll();

        Clear();
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

        for (POSITION pos = value.child.GetStartPosition(); pos != NULL;)
        {
            from = NULL;
            to = NULL;
            key.Empty();

            value.child.GetNext(pos, key, from);
            if (!from) continue;

            to = new iVariant;
            if (!to) continue;

            to->Setup(*from);
            child[key] = to;
        }
    }

    public: void Log(int tab = 0)
    {
#ifdef _DEBUG
        iString print;
        iString temp;
        const Array::iNode* pair;

        if (++tab == 1)
            TRACE(_T("%s\n(\n"), (LPCTSTR)(*this));

        const iList<Array::iNode*>& list = child.ToList();
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

    public: iString Serialize();
    public: BOOL Deserialize(LPCTSTR source);

#if 0 // legacy
    private: void Serialize(iString& result);
    private: LPCTSTR Deserialize(LPCTSTR source, int depth);
#else
    private: void Serialize(Writer<StringBuffer>& writer);
    private: void Deserialize(Value::ConstMemberIterator& it, const Value& object);
    private: void Deserialize(Value::ConstValueIterator& it, const Value& value, int index);
#endif
};
