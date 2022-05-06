#pragma once
#include <thread>
#include <chrono>

#pragma warning ( disable: 26495 )
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "d3d9.lib")
#pragma warning( pop ) 

#include "utils.h"

LPDIRECT3D9 d3d = nullptr;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev = nullptr;    // the pointer to the device class
LPDIRECT3DVERTEXBUFFER9 v_buffer = nullptr;

void initGraphics(void)
{
	// create three vertices using the CUSTOMVERTEX struct built earlier
	CUSTOMVERTEX vertices[] =
	{
		{ 3.0f, -3.0f, 0.0f, D3DCOLOR_XRGB(255, 0, 0), },
		{ 0.0f, 3.0f, 0.0f, D3DCOLOR_XRGB(0, 255, 0), },
		{ -3.0f, -3.0f, 0.0f, D3DCOLOR_XRGB(0, 0, 255), },
	};

	// create the vertex and store the pointer into v_buffer, which is created globally
	d3ddev->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&v_buffer,
		NULL);

	VOID* pVoid; 

	v_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, vertices, sizeof(vertices));
	v_buffer->Unlock();

	d3ddev->SetFVF(CUSTOMFVF);
}

void initD3D(HWND hWnd) {
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.Windowed = TRUE;						 // program windowed, not fullscreen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
	d3dpp.hDeviceWindow = hWnd;					 // set the window to be used by Direct3D
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;

	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev
	);

	initGraphics();

	d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE);
	d3ddev->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

void render_frame() {
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();    // begins the 3D scene
	{
		// select the vertex buffer to display
		d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));

		// SET UP THE PIPELINE

		// a matrix to store the rotation information
		D3DXMATRIX matRotateY;

		// an ever-increasing float value
		static float index = 0.0f; index += 0.05f;

		// build a matrix to rotate the model based on the increasing float value
		D3DXMatrixRotationY(&matRotateY, index);

		// tell Direct3D about our matrix
		d3ddev->SetTransform(D3DTS_WORLD, &matRotateY);

		// the view transform matrix
		D3DXMATRIX matView;

		D3DXVECTOR3 camerapos{ 0.0f, 0.0f, 10.0f };
		D3DXVECTOR3 lookatpos{ 0.0f, 0.0f, 0.0f };
		D3DXVECTOR3 updirection{ 0.0f, 1.0f, 0.0f };

		D3DXMatrixLookAtLH(&matView,
			&camerapos,
			&lookatpos,
			&updirection);

		// set the view transform to matView
		d3ddev->SetTransform(D3DTS_VIEW, &matView);

		// the projection transform matrix
		D3DXMATRIX matProjection;

		D3DXMatrixPerspectiveFovLH(&matProjection,
			D3DXToRadian(60),							// the horizontal field of view
			(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, // aspect ratio
			1.0f,										// the near view-plane
			100.0f);									// the far view-plane

		// set the projection
		d3ddev->SetTransform(D3DTS_PROJECTION, &matProjection);

		// select the vertex buffer to display
		d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));

		// copy the vertex buffer to the back buffer
		d3ddev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 1);
	}
	d3ddev->EndScene();
	d3ddev->Present(NULL, NULL, NULL, NULL);    // displays the created frame

	std::this_thread::sleep_for(std::chrono::milliseconds(7));
}

void cleanD3D() {
	d3ddev->Release();
	d3d->Release();
	v_buffer->Release();
}
