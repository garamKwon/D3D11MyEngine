// D3D11MyEngine.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "D3D11MyEngine.h"
#include "D3DEngine.h"
#include "Camera.h"
#include "Shader.h"
#include "Material.h"

#define MAX_LOADSTRING 100

UINT CShader::CB_SHADER_SLOT = 0x00;
UINT CLight::CB_LIGHT_SLOT = 0x01;
UINT CMaterial::CB_MATERIAL_SLOT = 0x02;
UINT CAnimateShader::CB_FINAL_TRANSFORM_SLOT = 0x03;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
D3DEngine gMyEngine;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_D3D11MYENGINE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	gMyEngine.CreateScene("Scene1");	
	gMyEngine.GetCurrentScene()->SetContorller(new CController("controller"));
	gMyEngine.SetDebugCamera(gMyEngine.CreateCamera("DebugCamera", ObjectLayer::LAYER_SCENE, XMFLOAT3(0, 0, -100)));
	gMyEngine.CreateCamera("Camera", ObjectLayer::LAYER_SCENE, XMFLOAT3(0, 0, 0));

	CMesh* pMesh = gMyEngine.MakeCube("Cube", 5, 5, 5);
//	CMesh* pZero = gMyEngine.MakeCube("ZeroAxis", 1, 1, 1);
	CMesh* pSkyBox = gMyEngine.MakeSkybox("Skybox");
	CMesh* pBadMesh = gMyEngine.MakeStaticFbxMesh("fbx", "res/model/Bad.FBX");
//	CMesh* pWallMesh = gMyEngine.MakeStaticFbxMesh("fbx", "res/model/Wall/Wall.FBX");
//	CMesh* pWall01Mesh = gMyEngine.MakeStaticFbxMesh("fbx", "res/model/Wall/Wall01.FBX");
//	CMesh* pAnimateMesh = gMyEngine.MakeAnimateFbxMesh("Anim", "Hero_Sword_Animation.FBX");
	CMesh* pQuad = gMyEngine.MakeQuad("Quad", 5, 5);

	gMyEngine.AddParticle("Dust", pQuad, 10, XMFLOAT3(0,0,0), XMFLOAT3(0,1.0,0), 100.0f,0.5f,10.0f, 9.8f, 1.0f, 1.0f, 50.0f);
	gMyEngine.CreateSkyboxObject("SkyBox", ObjectLayer::LAYER_SCENE, pSkyBox);
	gMyEngine.CreateObject("Axis", ObjectLayer::LAYER_SCENE, pMesh, ObjectType::TYPE_OBJECT, XMFLOAT3(-100, 0, 0));
//	gMyEngine.CreateObject("Object1", ObjectLayer::LAYER_SCENE, pAnimateMesh, ObjectType::TYPE_OBJECT, XMFLOAT3(100, 0, 0));
//	gMyEngine.CreateObject("object2", ObjectLayer::LAYER_SCENE, pBadMesh);
//	gMyEngine.CreateObject("Object3", ObjectLayer::LAYER_SCENE, pWallMesh, ObjectType::TYPE_OBJECT, XMFLOAT3(0, 0, 50));
//	gMyEngine.CreateObject("Object4", ObjectLayer::LAYER_SCENE, pWall01Mesh, ObjectType::TYPE_OBJECT, XMFLOAT3(50, 0, 0));

//	gMyEngine.ChangeTexture("object2", L"res/model/Bad_d.png", "DiffuseTexture");
//	gMyEngine.ChangeTexture("Object3", L"res/model/Wall/WallDiffuse.png", "DIffuseTexture");
//	gMyEngine.ChangeTexture("Object4", L"res/model/Wall/WallDiffuse.png", "DIffuseTexture");
//	gMyEngine.ChangeTexture("Object1", L"Hero_Sword_Texture.dds", "DiffuseTexture");
	

	//	gMyEngine.CameraLookAtObject( "Camera", "Object1" );

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_D3D11MYENGINE));

    MSG msg;

	while (1)
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		else
		{
			// 엔진이 반복적으로 수행할 작업
			gMyEngine.FrameAdvance();
			gMyEngine.Render();
		}
	}
	// 엔진 해제
	gMyEngine.Destroy();

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_D3D11MYENGINE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_D3D11MYENGINE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   gMyEngine.SetFrameBufferSize(800, 600);

   RECT rc = { 0, 0, gMyEngine.GetFrameBufferWidth(), gMyEngine.GetFrameBufferHeight() };
   DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
   AdjustWindowRect(&rc, dwStyle, FALSE);
   HWND hMainWnd = CreateWindow(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
	   rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
   if (!hMainWnd) return false;

   // 엔진 초기화
   gMyEngine.Init(hInstance, hMainWnd, gMyEngine.GetFrameBufferWidth(), gMyEngine.GetFrameBufferHeight());

   ShowWindow(hMainWnd, nCmdShow);
   UpdateWindow(hMainWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

#ifdef __DEBUG_MODE__
	AllocConsole();                 // Allocate console window
	freopen("CONOUT$", "a", stderr); // Redirect stderr to console
	freopen("CONOUT$", "a", stdout); // Redirect stdout also
	freopen("CONIN$", "r", stdin);
	SetConsoleTitleA("Debug");
#endif	

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 그리기 코드를 추가합니다.
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
	case WM_KEYUP:
		gMyEngine.StartParticle("Dust");
		gMyEngine.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
