#pragma once
#include <Windows.h>
#include <cstdint>
#include "externals/imgui/imgui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// WindowsAPI
class WinApp
{
public: // メンバ関数
	// 初期化
	void Intialize();
	// 更新
	void Update();

public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};

