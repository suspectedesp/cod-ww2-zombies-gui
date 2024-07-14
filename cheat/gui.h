#pragma once
#include <d3d9.h>
#include <iostream>
#include <string>

using namespace std;

namespace gui
{
	// constant window size
	constexpr int WIDTH = 500;
	constexpr int HEIGHT = 300;

	extern bool isRunning;
	extern HWND window;
	extern WNDCLASSEX windowClass;
	extern POINTS position;
	extern LPDIRECT3D9 d3d;
	extern LPDIRECT3DDEVICE9 device;
	extern D3DPRESENT_PARAMETERS presentParameters;

	// handle window creation & destruction
	void CreateHWindow(const char* windowName) noexcept;
	void DestroyHWindow() noexcept;

	// handle device creation & destruction
	bool CreateDevice() noexcept;
	void ResetDevice() noexcept;
	void DestroyDevice() noexcept;

	// handle ImGui creation & destruction
	void CreateImGui() noexcept;
	void DestroyImGui() noexcept;

	void BeginRender() noexcept;
	void EndRender() noexcept;
	void Render(HANDLE handle, uintptr_t moduleBase) noexcept;
	extern void HealthFunction(HANDLE handle, uintptr_t modulebase) noexcept;
	extern void WeaponsFunction(HANDLE handle, uintptr_t modulebase) noexcept;
	extern void AbilityFunction(HANDLE handle, uintptr_t modulebase) noexcept;
	extern void LogToFile(const string& message) noexcept;
}