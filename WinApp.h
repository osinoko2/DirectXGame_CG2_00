#pragma once
#include <Windows.h>
#include <cstdint>
#include "externals/imgui/imgui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// WindowsAPI
class WinApp
{
public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public: // メンバ関数
	// 初期化
	void Intialize();

	// getter
	HWND GetHwnd() const { return hwnd; }
	HINSTANCE GetHinstance() const { return wc.hInstance; }

	// メッセージの処理
	bool ProcessMessage();

	// 終了
	void Finalize();

public: // 定数
	// クライアント領域のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

private: 
	// ウィンドウハンドル
	HWND hwnd;

	// ウィンドウクラスの設定
	WNDCLASS wc{};
};

