#pragma once

template <class T>
class iSingleton
{
    protected: static T* instance;
    protected: static BOOL attached;

    protected: iSingleton()
    {
    }

    protected: virtual ~iSingleton()
    {
    }

    public: static T* i() 
    {
        return Instance();
    }

    public: static T* Instance()
    {
        if (instance == NULL)
        {
            instance = new T;
            attached = FALSE;
        }

        return instance;
    }

    public: static void Release()
    {
        if (attached)
            instance = NULL;
        else
            SAFE_DELETE(instance);
    }

    public: static void Attach(T* i)
    {
        ASSERT(i);
        ASSERT(instance != i);

        Release();

        instance = i;
        attached = TRUE;
    }
};

template <class T> T* iSingleton<T>::instance = NULL;
template <class T> BOOL iSingleton<T>::attached = FALSE;
