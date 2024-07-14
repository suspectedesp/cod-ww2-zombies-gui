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
    std::string logMessage = "\n* ------------------------------------ *\nN E W _ R U N";
    gui::LogToFile(logMessage);
    HWND hwnd = FindWindow(NULL, L"Call of Duty®: WWII Multiplayer");
    bool allowwithoutwindow = GetBooleanFromJSON("debug-mode", "false");
    if (hwnd == NULL) {
        logMessage = "Cannot find window, open s2_mp64_ship.exe";
        gui::LogToFile(logMessage);
        MessageBoxA(NULL, "Cannot find window, open Call of Duty: WWII Multiplayer", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    DWORD procID;
    GetWindowThreadProcessId(hwnd, &procID);
    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
    if (handle == NULL)
    {
        logMessage = "Cannot obtain process handle";
        gui::LogToFile(logMessage);
        MessageBoxA(NULL, "Cannot obtain process handle", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    uintptr_t modulebase = GetModuleBase(L"s2_mp64_ship.exe", procID);
    if (modulebase == 0) {
        logMessage = "Module base is 0";
        gui::LogToFile(logMessage);
        MessageBoxA(NULL, "ModuleBase is 0", "Error", MB_OK | MB_ICONERROR);
    }
    gui::CreateHWindow("COD WWII Zombies: Menu");
    gui::CreateDevice();
    gui::CreateImGui();

    bool UserNameChangerLoop = GetBooleanFromJSON("UserNameChangerLoop", "true");
    std::string username = GetValueFromJSON("username", "DefaultUsername");
    logMessage = "Welcome, " + username;
    gui::LogToFile(logMessage);
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