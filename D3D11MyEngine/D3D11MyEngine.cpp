// D3D11MyEngine.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
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

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.
D3DEngine gMyEngine;

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
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

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_D3D11MYENGINE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
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
			// ������ �ݺ������� ������ �۾�
			gMyEngine.FrameAdvance();
			gMyEngine.Render();
		}
	}
	// ���� ����
	gMyEngine.Destroy();

    return (int) msg.wParam;
}



//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
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
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

   gMyEngine.SetFrameBufferSize(800, 600);

   RECT rc = { 0, 0, gMyEngine.GetFrameBufferWidth(), gMyEngine.GetFrameBufferHeight() };
   DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
   AdjustWindowRect(&rc, dwStyle, FALSE);
   HWND hMainWnd = CreateWindow(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
	   rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
   if (!hMainWnd) return false;

   // ���� �ʱ�ȭ
   gMyEngine.Init(hInstance, hMainWnd, gMyEngine.GetFrameBufferWidth(), gMyEngine.GetFrameBufferHeight());

   ShowWindow(hMainWnd, nCmdShow);
   UpdateWindow(hMainWnd);

   return TRUE;
}

//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
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
		// �޴� ������ ���� �м��մϴ�.
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
		// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.
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
// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
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
