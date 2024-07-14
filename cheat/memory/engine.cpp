#include "engine.h"
#include <tlhelp32.h>
#include "../includes.h"

template <typename T>
BOOL WPM(HANDLE hHand, uintptr_t Address, T val) {
    return WriteProcessMemory(hHand, (BYTE*)Address, &val, sizeof(val), nullptr);
}

template <typename T>
T RPM(HANDLE hHand, uintptr_t Address) {
    T ret;
    ReadProcessMemory(hHand, (BYTE*)Address, &ret, sizeof(ret), nullptr);
    return ret;
}

uintptr_t GetModuleBase(const wchar_t* modName, DWORD PID) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, PID);
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32W modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32FirstW(hSnap, &modEntry)) {
            do {
                if (!_wcsicmp(modEntry.szModule, modName)) {
                    return (uintptr_t)modEntry.modBaseAddr;
                }
            } while (Module32NextW(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return 0;
}

// Explicit template instantiation to avoid linker errors
template BOOL WPM<std::string>(HANDLE hHand, uintptr_t Address, std::string val);
template unsigned __int64 RPM<unsigned __int64>(HANDLE hHand, uintptr_t Address);
template BOOL WPM<int>(HANDLE hHand, uintptr_t Address, int val); // New instantiation
