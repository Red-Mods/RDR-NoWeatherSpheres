#include "pch.h"



BOOL APIENTRY DllMain(HMODULE _Module, DWORD _ReasonForCall, LPVOID _Reserved)
{
    switch (_ReasonForCall)
    {
        case DLL_PROCESS_ATTACH:
        {
            // Prevent loading weather spheres, that mean that area like thieves landing will no longer feature an hardcoded gloomy weather.
            Pattern("48 85 DB 0F 84 93 ? ? ? 41 8B 45 34 C1 E8 14").Scan([](const Pattern& _This)
            {
                _This.Add(4).Put(0x85);
            });
        }
        break;
    }

    return TRUE;
}