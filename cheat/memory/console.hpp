#pragma once

#include <iostream>
#include <cstdio>  // for FILE, freopen_s
#include <windows.h>

inline void AllocateConsole()
{
    if (AllocConsole())
    {
        FILE* fDummy;
        if (freopen_s(&fDummy, "CONOUT$", "w", stdout) != 0)
        {
            std::cerr << "Failed to redirect stdout" << std::endl;
        }
        if (freopen_s(&fDummy, "CONIN$", "r", stdin) != 0)
        {
            std::cerr << "Failed to redirect stdin" << std::endl;
        }
    }
    else
    {
        std::cerr << "Failed to allocate console" << std::endl;
    }
}

inline void FreeingConsole()
{
    fclose(stdout);
    fclose(stdin);
    ::FreeConsole();
}