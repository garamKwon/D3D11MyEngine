// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

#define DIR_FORWARD		0x01
#define DIR_BACKWARD	0x02
#define DIR_LEFT		0x04
#define DIR_RIGHT		0x08
#define DIR_UP			0x10
#define DIR_DOWN		0x20

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <iostream>

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
#include <time.h>
#include <D3DX11.h>
#include <mmsystem.h>
#include <math.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>
#include <random>
#include <vector>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "libfbxsdk.lib")

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) \
   if(x != NULL)        \
            {           \
      x->Release();     \
      x = NULL;         \
            }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) \
   if(x != NULL)       \
               {       \
	x->Release();	   \
x = NULL;             \
               }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) \
   if(x != NULL)             \
                  {          \
	  delete[] x;		     \
      x = NULL;              \
                  }
#endif

#define __DEBUG_MODE__
#define __DEBUG_POS__
//#define __DEBUG_RENDERING_INFO__

// ����� ��� ��� �ÿ� �ƴ� ��
#ifdef __DEBUG_MODE__
#else
#endif	