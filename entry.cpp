#include "includes.h"
#include "MenuFonts.h"

t_Username GetUsername;

DWORD installed;

class init_font
{
public:
    init_font(void* font, uint32_t length)
    {
        if (handle = AddFontMemResourceEx(font, length, nullptr, &installed); handle == nullptr)
            return;

        VirtualProtect(font, length, PAGE_READWRITE, 0);
        memset(font, 0, length);
    }

private:
    HANDLE handle;
};

int __stdcall DllMain( HMODULE self, ulong_t reason, void *reserved ) {
    if( reason == DLL_PROCESS_ATTACH ) {

#ifndef KOLO
        HANDLE thread = CreateThread( nullptr, 0, Client::init, nullptr, 0, nullptr );
        if( !thread )
            return 0;

        init_font(static_cast<void*>(astriumwep), sizeof(astriumwep));

        // CloseHandle( thread );
#else
		Client::init( nullptr );
#endif
        return 1;
    }

    return 0;
}