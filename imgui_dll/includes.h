#pragma once
#include "./imgui/imgui.h"
#include "./imgui/imgui_impl_dx9.h"
#include "./imgui/imgui_impl_win32.h"

#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#include <iostream>
#include <Windows.h>

#include "minhook/MinHook.h"
#pragma comment(lib, "libMinHook.x86.lib")