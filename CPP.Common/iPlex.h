#pragma once

struct iPlex
{
    public: iPlex* next;

    public: LPVOID GetData() 
    { 
        return this + 1; 
    }

    public: void FreeDataChain()
    {
        iPlex* plex = this;
        while (plex != NULL)
        {
            BYTE* bytes = (BYTE*)plex;
            iPlex* next = plex->next;
            SAFE_DELETE_ARRAY(bytes);
            plex = next;
        }
    }

    public: static iPlex* Create(iPlex*& head, UINT count, UINT size)
    {
        ASSERT(count > 0 && size > 0);

        iPlex* plex = (iPlex*) new BYTE[sizeof(iPlex) + count * size];
        plex->next = head;
        head = plex;
        return plex;
    }
};
