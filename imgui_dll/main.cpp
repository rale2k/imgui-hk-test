// dllmain.cpp : Defines the entry point for the DLL application.
#include "includes.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef long(__stdcall *tEndScene)(IDirect3DDevice9* pDevice);
tEndScene oEndScene;

void* d3d9dev_vtable[119];

WNDPROC oWndProc;
LRESULT CALLBACK hkWndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, umsg, wparam, lparam))
    {
        return 1l;
    }
    return CallWindowProc(oWndProc, hwnd, umsg, wparam, lparam);
}

long _stdcall hk_EndScene(IDirect3DDevice9* pDevice) {    
    static bool init = false;
    if (!init)
    {
        init = true;
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        HWND hWnd;
        D3DDEVICE_CREATION_PARAMETERS params;
        pDevice->GetCreationParameters(&params);
        hWnd = params.hFocusWindow;


        ImGui_ImplDX9_Init(pDevice);
        ImGui_ImplWin32_Init(hWnd);

        oWndProc = WNDPROC(SetWindowLongPtrA(hWnd, GWLP_WNDPROC, LONG_PTR(hkWndProc)));
    }

    ImGui_ImplWin32_NewFrame();
    ImGui_ImplDX9_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow(NULL);

    ImGui::EndFrame();
    ImGui::Render();

    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());    
    return oEndScene(pDevice);
}

bool GetD3D9DevVtable(void** pTable, size_t Size, HWND window)
{
    if (!pTable)
        return false;

    IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);

    if (!pD3D)
        return false;

    IDirect3DDevice9* pDummyDevice = NULL;

    // options to create dummy device
    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = false;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = window;

    HRESULT dummyDeviceCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

    if (dummyDeviceCreated != S_OK)
    {
        // may fail in windowed fullscreen mode, trying again with windowed mode
        d3dpp.Windowed = !d3dpp.Windowed;

        dummyDeviceCreated = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);

        if (dummyDeviceCreated != S_OK)
        {
            pD3D->Release();
            return false;
        }
    }

    memcpy(pTable, *reinterpret_cast<void***>(pDummyDevice), Size);

    pDummyDevice->Release();
    pD3D->Release();
    return true;
}

void DllThread() {
    if (!AllocConsole()) {
        return;
    }

    FILE* fp;

    freopen_s(&fp, "CONOUT$", "w", stdout);

    HANDLE process = GetModuleHandleA("d3d9 window.exe");

    GetD3D9DevVtable(d3d9dev_vtable, sizeof(d3d9dev_vtable), FindWindowA(NULL, "Window"));

    MH_Initialize();

    MH_STATUS result = MH_CreateHook(d3d9dev_vtable[42], hk_EndScene, reinterpret_cast<void**>(&oEndScene));

    if (result == MH_OK)
    {
        MH_EnableHook(d3d9dev_vtable[42]);
    }
}

void Detach() {
    if (MH_DisableHook(&MessageBoxA) == MB_OK)
        MH_Uninitialize();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     ) {
    DisableThreadLibraryCalls(hModule);

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DllThread, NULL, 0, NULL);
        break;
    case DLL_PROCESS_DETACH:
        Detach();
        return TRUE;
    }

    return TRUE;
}
