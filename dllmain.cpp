// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
intptr_t returnadddress = 0;
bool detour(void* startpoint, void* ourASM, int length)
{
    if (length < 5)
    {
        return false;
    }
    DWORD oldProtection;
    VirtualProtect(startpoint, length, PAGE_EXECUTE_READWRITE, &oldProtection);
    
    DWORD relativeoffset = ((DWORD)ourASM - (DWORD)startpoint - 5);
    *(intptr_t*)startpoint = 0xE9;//HexCode
    *(DWORD*)((DWORD)startpoint + 1) = relativeoffset;
}

void __declspec(naked)ourASM()
{
    __asm
    {
        nop
        nop
        jmp [returnadddress]
    }
}
DWORD WINAPI hackTheard(LPVOID param)
{
    intptr_t moduleBase = (intptr_t)GetModuleHandle(L"ac_client.exe");
    int hooklength = 2;
    intptr_t hookAddress = moduleBase + 0xC73EA;
    returnadddress = hookAddress + hooklength;
    if (!(detour((void*)hookAddress, ourASM,hooklength)))
    {
        FreeLibraryAndExitThread((HMODULE)param, 0);
        return 0;
    }
    while (true)
    {
        Sleep(50);
    }
    FreeLibraryAndExitThread((HMODULE)param, 0);
    return 0;

    
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CloseHandle(CreateThread(0, 0, hackTheard, hModule, 0, 0));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

