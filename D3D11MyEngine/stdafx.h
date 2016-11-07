// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

#define DIR_FORWARD		0x01
#define DIR_BACKWARD	0x02
#define DIR_LEFT		0x04
#define DIR_RIGHT		0x08
#define DIR_UP			0x10
#define DIR_DOWN		0x20

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <iostream>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
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

// 디버그 모드 사용 시와 아닐 시
#ifdef __DEBUG_MODE__
#else
#endif	