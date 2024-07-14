#pragma once
#ifndef ENGINE_H
#define ENGINE_H

#include "../includes.h"

template <typename T>
BOOL WPM(HANDLE hHand, uintptr_t Address, T val);

template <typename T>
T RPM(HANDLE hHand, uintptr_t Address);

uintptr_t GetModuleBase(const wchar_t* modName, DWORD PID);

#endif // ENGINE_H