#pragma once
#include <cstdio>
#include <Windows.h>
#include <winbase.h>

#include "3d9.h"
#include "utils.h"

HINSTANCE h_instance;
HWND hWnd;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT:
        render_frame();
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

ATOM registerClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = 0;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wcex.lpszMenuName = L"Window";
    wcex.lpszClassName = L"Window";
    wcex.hIconSm = 0;

    return RegisterClassExW(&wcex);
}

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd) {
    h_instance = GetModuleHandle(NULL);
    registerClass(h_instance);

	hWnd = CreateWindowEx(NULL, L"Window", L"Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, SCREEN_WIDTH, SCREEN_HEIGHT, nullptr, nullptr, h_instance, nullptr);

	if (hWnd != NULL) {
        ShowWindow(hWnd, 1);
    }
    else {
        wchar_t buf[256];
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            buf, (sizeof(buf) / sizeof(wchar_t)), NULL);

        MessageBoxW(hWnd, buf, L"Error", MB_OK);
        return 0;
    }

    initD3D(hWnd);

    MSG msg;
    while (TRUE) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
            break;

    }

    cleanD3D();

    return msg.wParam;
}