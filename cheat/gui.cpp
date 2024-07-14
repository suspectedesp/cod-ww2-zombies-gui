#include "gui.h"
#include <sstream>
#include <windows.h>
#include <d3d9.h>
#include <fstream>
#include <thread>
#include <atomic>
#include <chrono>

#include "includes.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND window,
    UINT message,
    WPARAM wideParameter,
    LPARAM longParameter
);

namespace gui {
    bool isRunning = true;
    HWND window = nullptr;
    WNDCLASSEX windowClass = {};
    POINTS position = {};
    LPDIRECT3D9 d3d = nullptr;
    LPDIRECT3DDEVICE9 device = nullptr;
    D3DPRESENT_PARAMETERS presentParameters = {};
}

LRESULT CALLBACK WindowProcess(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter)
{
    if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
        return true;

    switch (message)
    {
    case WM_SIZE: {
        if (gui::device && wideParameter != SIZE_MINIMIZED)
        {
            gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
            gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
            gui::ResetDevice();
        }
    } return 0;

    case WM_SYSCOMMAND: {
        if ((wideParameter & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
    } break;

    case WM_DESTROY: {
        PostQuitMessage(0);
    } return 0;

    case WM_LBUTTONDOWN: {
        gui::position = MAKEPOINTS(longParameter); // set click points
    } return 0;

    case WM_MOUSEMOVE: {
        if (wideParameter == MK_LBUTTON)
        {
            const auto points = MAKEPOINTS(longParameter);
            auto rect = RECT{ };

            GetWindowRect(gui::window, &rect);

            rect.left += points.x - gui::position.x;
            rect.top += points.y - gui::position.y;

            if (gui::position.x >= 0 &&
                gui::position.x <= gui::WIDTH &&
                gui::position.y >= 0 && gui::position.y <= 19)
                SetWindowPos(
                    gui::window,
                    HWND_TOPMOST,
                    rect.left,
                    rect.top,
                    0, 0,
                    SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
                );
        }

    } return 0;
    }

    return DefWindowProc(window, message, wideParameter, longParameter);
}

void gui::CreateHWindow(const char* windowName) noexcept
{
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_CLASSDC;
    windowClass.lpfnWndProc = WindowProcess;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandleA(0);
    windowClass.hIcon = 0;
    windowClass.hCursor = 0;
    windowClass.hbrBackground = 0;
    windowClass.lpszMenuName = 0;
    windowClass.lpszClassName = L"class001"; // Changed to wide string
    windowClass.hIconSm = 0;

    RegisterClassEx(&windowClass);

    window = CreateWindowEx(
        0,
        L"class001",
        L"COD WWII Zombies: Menu", // Changed to wide string
        WS_POPUP,
        100,
        100,
        WIDTH,
        HEIGHT,
        0,
        0,
        windowClass.hInstance,
        0
    );

    ShowWindow(window, SW_SHOWDEFAULT);
    UpdateWindow(window);
}

void gui::DestroyHWindow() noexcept
{
    DestroyWindow(window);
    UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::CreateDevice() noexcept
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (!d3d)
        return false;

    ZeroMemory(&presentParameters, sizeof(presentParameters));

    presentParameters.Windowed = TRUE;
    presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    presentParameters.EnableAutoDepthStencil = TRUE;
    presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
    presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if (d3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        window,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &presentParameters,
        &device) < 0)
        return false;

    return true;
}

void gui::ResetDevice() noexcept
{
    ImGui_ImplDX9_InvalidateDeviceObjects();

    const auto result = device->Reset(&presentParameters);

    if (result == D3DERR_INVALIDCALL)
        IM_ASSERT(0);

    ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept
{
    if (device)
    {
        device->Release();
        device = nullptr;
    }

    if (d3d)
    {
        d3d->Release();
        d3d = nullptr;
    }
}

void gui::CreateImGui() noexcept
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ::ImGui::GetIO();

    io.IniFilename = NULL;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(device);
}

void gui::DestroyImGui() noexcept
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void gui::BeginRender() noexcept
{
    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);

        if (message.message == WM_QUIT)
        {
            isRunning = !isRunning;
            return;
        }
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void gui::EndRender() noexcept
{
    ImGui::EndFrame();

    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

    device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

    if (device->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }

    const auto result = device->Present(0, 0, 0, 0);

    // Handle loss of D3D9 device
    if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        ResetDevice();
}

static std::atomic<bool> healthEnabled(false);
static std::atomic<bool> weaponsEnabled(false);
static std::atomic<bool> abilityEnabled(false);
static std::thread healthThread;
static std::thread weaponsThread;
static std::thread abilityThread;
static char name[80] = "Default Name";


void gui::LogToFile(const std::string& message) noexcept {
    std::string executablePath = GetExecutablePath();
    std::string directory = GetExecutableDir(executablePath);
    std::string logFilePath = directory + "\\logs.txt";

    std::ofstream logFile(logFilePath, std::ios::app); // Append mode
    if (logFile.is_open()) {
        logFile << message << std::endl;
        logFile.close();
    }
}

void gui::HealthFunction(HANDLE handle, uintptr_t modulebase) noexcept {
    uintptr_t playerhealthAddress = RPM<uintptr_t>(handle, modulebase + self::HealthAddress) + self::HealthOffset;

    while (healthEnabled) {
        // Write health value
        WPM<int>(handle, playerhealthAddress, amounts::health);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }
}

void gui::WeaponsFunction(HANDLE handle, uintptr_t modulebase) noexcept {
    uintptr_t _1911_pistole1Address = RPM<uintptr_t>(handle, modulebase + weapons::_1911) + weapons::_1911_pistole1;
    uintptr_t _1911_pistole2Address = RPM<uintptr_t>(handle, modulebase + weapons::_1911) + weapons::_1911_pistole2;
    uintptr_t lethalWeaponAddress = RPM<uintptr_t>(handle, modulebase + lethal::lethalWeapon) + lethal::lethalWeaponOffset;
    uintptr_t m1GarandAddress = RPM<uintptr_t>(handle, modulebase + weapons::m1Garand) + weapons::m1GarandOffset;
    uintptr_t m1928Address = RPM<uintptr_t>(handle, modulebase + weapons::m1928) + weapons::m1928Offset;
    uintptr_t m30LuftwaffeDrillingAddress = RPM<uintptr_t>(handle, modulebase + weapons::m30LuftwaffeDrilling) + weapons::m30luftwaffeDrillingOffset;
    uintptr_t machinePistole1Address = RPM<uintptr_t>(handle, modulebase + weapons::machinePistole) + weapons::machinePistole_pistole1;
    uintptr_t machinePistole2Address = RPM<uintptr_t>(handle, modulebase + weapons::machinePistole) + weapons::machinePistole_pistole2;
    uintptr_t svt40Address = RPM<uintptr_t>(handle, modulebase + weapons::svt40) + weapons::svt40Offset;
    uintptr_t type100Address = RPM<uintptr_t>(handle, modulebase + weapons::type100) + weapons::type100Offset;
    uintptr_t teslaGunAddress = RPM<uintptr_t>(handle, modulebase + weapons::teslaGun) + weapons::teslaGunOffset;

    while (weaponsEnabled) {
        // Write weapon values
        WPM<int>(handle, _1911_pistole1Address, amounts::weapons);
        WPM<int>(handle, _1911_pistole2Address, amounts::weapons);
        WPM<int>(handle, lethalWeaponAddress, amounts::lethals);
        WPM<int>(handle, m1GarandAddress, amounts::weapons);
        WPM<int>(handle, m1928Address, amounts::weapons);
        WPM<int>(handle, m30LuftwaffeDrillingAddress, amounts::weapons);
        WPM<int>(handle, machinePistole1Address, amounts::weapons);
        WPM<int>(handle, machinePistole2Address, amounts::weapons);
        WPM<int>(handle, svt40Address, amounts::weapons);
        WPM<int>(handle, type100Address, amounts::weapons);
        WPM<int>(handle, teslaGunAddress, amounts::weapons);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void gui::AbilityFunction(HANDLE handle, uintptr_t modulebase) noexcept {
    uintptr_t SPECIAL_ABILITYAddress = modulebase + special::ability_address;

    while (abilityEnabled) {
        // Write ability value
        WPM<int>(handle, SPECIAL_ABILITYAddress, special::ability_max_amount);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void gui::Render(HANDLE handle, uintptr_t moduleBase) noexcept {
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
    ImGui::Begin(
        "COD WWII Zombies: Menu",
        &isRunning,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove
    );

    // Temporary bool variables for checkbox states
    bool healthChecked = healthEnabled.load();
    if (ImGui::Checkbox("Health", &healthChecked)) {
        healthEnabled = healthChecked;
        if (healthEnabled && !healthThread.joinable()) {
            healthThread = std::thread([handle, moduleBase]() { gui::HealthFunction(handle, moduleBase); });
        }
        else if (!healthEnabled && healthThread.joinable()) {
            healthThread.join();
        }
    }

    bool weaponsChecked = weaponsEnabled.load();
    if (ImGui::Checkbox("Ammo", &weaponsChecked)) {
        weaponsEnabled = weaponsChecked;
        if (weaponsEnabled && !weaponsThread.joinable()) {
            weaponsThread = std::thread([handle, moduleBase]() { gui::WeaponsFunction(handle, moduleBase); });
        }
        else if (!weaponsEnabled && weaponsThread.joinable()) {
            weaponsThread.join();
        }
    }

    bool abilityChecked = abilityEnabled.load();
    if (ImGui::Checkbox("Special Ability", &abilityChecked)) {
        abilityEnabled = abilityChecked;
        if (abilityEnabled && !abilityThread.joinable()) {
            abilityThread = std::thread([handle, moduleBase]() { gui::AbilityFunction(handle, moduleBase); });
        }
        else if (!abilityEnabled && abilityThread.joinable()) {
            abilityThread.join();
        }
    }

    ImGui::InputText("", name, IM_ARRAYSIZE(name));

    if (ImGui::Button("Change Name")) {
        uintptr_t user_address = RPM<uintptr_t>(handle, moduleBase + self::UsernameAddress) + self::UsernameOffset;
        WPM(handle, user_address, std::string(name));
        std::string logMessage = "Name changed to: " + std::string(name);
        gui::LogToFile(logMessage);
    }

    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("About")) {
        MessageBoxA(NULL, "COD WWII Zombies Menu\nVersion 1.0\nMade by suspectedesp on UnknownCheats", "About", MB_OK | MB_ICONINFORMATION);
    }

    ImGui::End();
}