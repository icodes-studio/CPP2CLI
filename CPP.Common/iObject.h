#pragma once

#define RUNTIME_CLASS(class_name) \
    ((iRuntimeClass*)(&class_name::class##class_name))

#define ASSERT_KINDOF(class_name, object) \
    ASSERT((object)->IsKindOf(RUNTIME_CLASS(class_name)))

#define DECLARE_DYNAMIC(class_name) \
    protected: static iRuntimeClass* GetBaseClass(); \
    public: static const iRuntimeClass class##class_name; \
    public: virtual iRuntimeClass* GetRuntimeClass() const;

#define DECLARE_DYNCREATE(class_name) \
    DECLARE_DYNAMIC(class_name) \
    static iObject* CreateObject();

#define IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, class_new_factory) \
    iRuntimeClass* class_name::GetBaseClass() \
    { return RUNTIME_CLASS(base_class_name); } \
    const iRuntimeClass class_name::class##class_name = { \
    #class_name, \
    sizeof(class class_name), \
    class_new_factory, \
    &class_name::GetBaseClass }; \
    iRuntimeClass* class_name::GetRuntimeClass() const \
    { return RUNTIME_CLASS(class_name); }

#define IMPLEMENT_DYNAMIC(class_name, base_class_name) \
    IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, NULL)

#define IMPLEMENT_DYNCREATE(class_name, base_class_name) \
    iObject* class_name::CreateObject() \
    { return new class_name; } \
    IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, class_name::CreateObject)

class iObject;

struct iRuntimeClass
{
    public: LPCSTR className;
    public: int objectSize;
    public: iObject* (*OnCreateObject)();
    public: iRuntimeClass* (*OnGetBaseClass)();

    public: iObject* CreateObject()
    {
        if (OnCreateObject == NULL)
        {
            TRACE(_T("WARNING: Trying to create object which is not DECLARE_DYNCREATE: %s.\n"), className);
            return NULL;
        }

        return (*OnCreateObject)();
    }

    public: BOOL IsDerivedFrom(const iRuntimeClass* baseClass) const
    {
        ASSERT(this != NULL);
        ASSERT(baseClass != NULL);

        const iRuntimeClass* thisClass = this;
        while (thisClass != NULL)
        {
            if (thisClass == baseClass)
                return TRUE;

            thisClass = (*thisClass->OnGetBaseClass)();
        }

        return FALSE;
    }
};

class iObject
{
    public: static const iRuntimeClass classiObject;

    protected: iObject()
    {
    }

    public: virtual ~iObject()
    {
    }

    public: virtual iRuntimeClass* GetRuntimeClass() const
    {
        return RUNTIME_CLASS(iObject);
    }

    public: BOOL IsKindOf(const iRuntimeClass* baseClass) const
    {
        ASSERT(this != NULL);
        iRuntimeClass* thisClass = GetRuntimeClass();
        return thisClass->IsDerivedFrom(baseClass);
    }

    public: static iRuntimeClass* GetBaseClass()
    {
        return NULL;
    }
};
