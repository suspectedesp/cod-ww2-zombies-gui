#include "gui.h"
#include "includes.h"
#include "memory/loops.hpp"

#include <thread>

int WINAPI wWinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    PWSTR arguments,
    int commandShow)
{
    gui::LogToFile("\n* ------------------------------------ *\nN E W _ R U N");
    HWND hwnd = FindWindow(NULL, L"Call of Duty®: WWII Multiplayer");
    bool allowwithoutwindow = GetBooleanFromJSON("debug-mode", "false");
    if (hwnd == NULL) 
    {
        AllocateConsole();
        std::cout << "Cannot find window, open Call of Duty: WWII Multiplayer" << std::endl;
        gui::LogToFile("Cannot find window, open s2_mp64_ship.exe");

        while (hwnd == NULL) 
        {
            hwnd = FindWindow(NULL, L"Call of Duty®: WWII Multiplayer"); 
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        gui::LogToFile("Window found, proceeding with the program.");
        std::cout << "Window found, proceeding with the program." << std::endl;
        FreeingConsole();
    }
    DWORD procID;
    GetWindowThreadProcessId(hwnd, &procID);
    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
    if (handle == NULL)
    {
        gui::LogToFile("Cannot obtain process handle");
        MessageBoxA(NULL, "Cannot obtain process handle", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    uintptr_t modulebase = GetModuleBase(L"s2_mp64_ship.exe", procID);
    if (modulebase == 0) {
        gui::LogToFile("Module base is 0");
        MessageBoxA(NULL, "ModuleBase is 0", "Error", MB_OK | MB_ICONERROR);
    }
    gui::CreateHWindow("COD WWII Zombies: Menu");
    gui::CreateDevice();
    gui::CreateImGui();

    bool UserNameChangerLoop = GetBooleanFromJSON("UserNameChangerLoop", "true");
    std::string username = GetValueFromJSON("username", "DefaultUsername");
    gui::LogToFile("Welcome, " + username);
    if (UserNameChangerLoop)
    {
        std::thread DisplayUsernameThread(DisplayUsername, handle, modulebase);
        DisplayUsernameThread.detach();
    }

    while (gui::isRunning)
    {
        gui::BeginRender();
        gui::Render(handle, modulebase);
        gui::EndRender();

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    gui::DestroyImGui();
    gui::DestroyDevice();
    gui::DestroyHWindow();

    return EXIT_SUCCESS;
}