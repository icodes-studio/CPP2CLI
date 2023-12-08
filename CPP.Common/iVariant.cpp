#include "pch.h"

iVariant iVariant::empty;

static BOOL IsNumeric(const iString& value)
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

#if 0 // legacy
iString iVariant::Serialize()
{
    iString result;
    Serialize(result);
    return result;
}

void iVariant::Serialize(iString& result)
{
    iString packet;
    packet.Format(_T("a:%d:{"), child.GetCount());
    result += packet;

    for (POSITION pos = child.GetStartPosition(); pos;)
    {
        iString key;
        iVariant* value = NULL;
        child.GetNext(pos, key, value);

        if (value)
        {
            if (IsNumeric(key))
                packet.Format(_T("i:%s;"), key);
            else
                packet.Format(_T("s:%d:\"%s\";"), key.GetLength(), key);

            result += packet;

            if (value->child.GetCount() > 0)
            {
                value->Serialize(result);
            }
            else
            {
                LPCTSTR valueToken = *value;
                if (value->vt == VT_BSTR)
                {
                    packet.Format(_T("s:%d:\"%s\";"), _tcslen(valueToken), valueToken);
                }
                else
                {
                    if (IsNumeric(valueToken))
                        packet.Format(_T("i:%s;"), valueToken);
                    else
                        packet.Format(_T("s:%d:\"%s\";"), _tcslen(valueToken), valueToken);
                }
                result += packet;
            }
        }
    }
    result += _T("}");
}

static LPCTSTR Parse(LPCTSTR source, iString& target)
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

BOOL iVariant::Deserialize(LPCTSTR source)
{
    ASSERT(source);
    LPCTSTR result = Deserialize(source, 0);
    return (result && result[0] == 0);
}

LPCTSTR iVariant::Deserialize(LPCTSTR source, int depth)
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
            source = Parse(source, key);

            if (!source)
                return NULL;

            iVariant* value = new iVariant;
            source = value->Deserialize(source, depth + 1);

            if (!source)
            {
                delete value;
                return NULL;
            }

            iVariant* temp;
            if (child.Lookup(key, temp))
            {
                delete temp;
                child.RemoveKey(key);
            }

            child.SetAt(key, value);
        }
    }

    LPCTSTR next = Parse(source, stringify);
    if (next != NULL)
        *this = stringify;

    return next;
}
#else
iString iVariant::Serialize()
{
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    Serialize(writer);
    return buffer.GetString();
}

void iVariant::Serialize(Writer<StringBuffer>& writer)
{
    if (GetCount() > 0)
    {
        BOOL isArray = TRUE;
        const iList<Array::iNode*>& list = child.ToList();
        for (POSITION pos = list.GetHeadPosition(); pos != NULL;)
        {
            if (!IsNumeric(list.GetNext(pos)->key))
                isArray = false;
        }

        if (isArray)
        {
            writer.StartArray();

            for (POSITION pos = list.GetHeadPosition(); pos != NULL;)
                list.GetNext(pos)->value->Serialize(writer);

            writer.EndArray();
        }
        else
        {
            writer.StartObject();

            const Array::iNode* keyValue;
            for (POSITION pos = list.GetHeadPosition(); pos != NULL;)
            {
                keyValue = list.GetNext(pos);
                writer.Key(iStringA(keyValue->key));
                keyValue->value->Serialize(writer);
            }

            writer.EndObject();
        }
    }
    else
    {
        switch (vt)
        {
        case VT_BOOL:
            writer.Bool((boolVal == VARIANT_TRUE));
            break;
        case VT_UI1:
            writer.Uint(bVal);
            break;
        case VT_I1:
            writer.Int(cVal);
            break;
        case VT_UI2:
            writer.Uint(uiVal);
            break;
        case VT_I2:
            writer.Int(iVal);
            break;
        case VT_UI4:
        case VT_UINT:
            writer.Uint(uintVal);
            break;
        case VT_I4:
        case VT_INT:
            writer.Int(intVal);
            break;
        case VT_UI8:
            writer.Uint64(ullVal);
            break;
        case VT_I8:
            writer.Int64(llVal);
            break;
        case VT_R8:
            writer.Double(dblVal);
            break;
        case VT_R4:
            writer.Double(fltVal);
            break;
        case VT_BSTR:
            writer.String(iStringA(stringify));
            break;
        case VT_EMPTY:
            writer.Null();
            break;
        default:
            writer.String(iStringA(*this));
        }
    }
}

BOOL iVariant::Deserialize(LPCTSTR source)
{
    ASSERT(source);

    Document document;
    if (document.Parse(iStringA(source)).HasParseError())
        return FALSE;

    if (document.IsArray())
    {
        Value::ConstValueIterator it = document.Begin();
        Deserialize(it, document, 0);
    }
    else
    {
        Value::ConstMemberIterator it = document.MemberBegin();
        Deserialize(it, document);
    }

    return TRUE;
}

void iVariant::Deserialize(Value::ConstMemberIterator& it, const Value& object)
{
    if (it == object.MemberEnd())
        return;

    if (it->value.IsBool())
    {
        (*this)[iString(it->name.GetString())] = it->value.GetBool();
    }
    else if (it->value.IsObject())
    {
        GenericObject<true, Value> obj = it->value.GetObj();
        Value::ConstMemberIterator itor = obj.MemberBegin();
        (*this)[iString(it->name.GetString())].Deserialize(itor, obj); 
    }
    else if (it->value.IsArray())
    {
        Value::ConstValueIterator itor = it->value.Begin();
        (*this)[iString(it->name.GetString())].Deserialize(itor, it->value.GetArray(), 0);
    }
    else if (it->value.IsInt())
    {
        (*this)[iString(it->name.GetString())] = it->value.GetInt();
    }
    else if (it->value.IsUint())
    {
        (*this)[iString(it->name.GetString())] = it->value.GetUint();
    }
    else if (it->value.IsInt64())
    {
        (*this)[iString(it->name.GetString())] = it->value.GetInt64();
    }
    else if (it->value.IsUint64())
    {
        (*this)[iString(it->name.GetString())] = it->value.GetUint64();
    }
    else if (it->value.IsDouble())
    {
        (*this)[iString(it->name.GetString())] = it->value.GetDouble();
    }
    else if (it->value.IsString())
    {
        (*this)[iString(it->name.GetString())] = it->value.GetString();
    }
    else
    {
        (*this)[iString(it->name.GetString())];
    }

    Deserialize(++it, object);
}

void iVariant::Deserialize(Value::ConstValueIterator& it, const Value& value, int index)
{
    if (it == value.End())
        return;

    if (it->IsBool())
    {
        (*this)[index++] = it->GetBool();
    }
    else if (it->IsObject())
    {
        GenericObject<true, Value> obj = it->GetObj();
        Value::ConstMemberIterator itor = obj.MemberBegin();
        (*this)[index++].Deserialize(itor, obj);
    }
    else if (it->IsArray())
    {
        Value::ConstValueIterator itor = it->Begin();
        (*this)[index++].Deserialize(itor, it->GetArray(), 0);
    }
    else if (it->IsInt())
    {
        (*this)[index++] = it->GetInt();
    }
    else if (it->IsUint())
    {
        (*this)[index++] = it->GetUint();
    }
    else if (it->IsInt64())
    {
        (*this)[index++] = it->GetInt64();
    }
    else if (it->IsUint64())
    {
        (*this)[index++] = it->GetUint64();
    }
    else if (it->IsDouble())
    {
        (*this)[index++] = it->GetDouble();
    }
    else if (it->IsString())
    {
        (*this)[index++] = it->GetString();
    }
    else
    {
        (*this)[index++];
    }

    Deserialize(++it, value, index);
}
#endif
