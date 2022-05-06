#pragma once
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

struct CUSTOMVERTEX
{
    FLOAT x, y, z;
    DWORD color;
};