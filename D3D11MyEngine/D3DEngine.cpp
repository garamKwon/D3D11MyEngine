#include "stdafx.h"
#include "D3DEngine.h"
#include "Material.h"


D3DEngine::D3DEngine( )
{
	m_pD3DLib = nullptr;
	m_pFbxLoader = nullptr;
	m_pCollisionSys = nullptr;
	m_pParticleSys = nullptr;
	m_pUISys = nullptr;
	m_pPickingSys = nullptr;
	m_pSceneManager = nullptr;
	m_pGeometryMaker = nullptr;
}


D3DEngine::~D3DEngine( )
{
	SAFE_DELETE( m_pSceneManager );
	SAFE_DELETE( m_pPickingSys );
	SAFE_DELETE( m_pUISys );
	SAFE_DELETE( m_pParticleSys );
	SAFE_DELETE( m_pCollisionSys );
	SAFE_DELETE( m_pFbxLoader );
	SAFE_DELETE( m_pD3DLib );
}

void D3DEngine::Init( HINSTANCE hInstance, HWND hWnd, UINT nClientWidth, UINT nClientHeight )
{
	m_pD3DLib = new D3D11SetUp;
	m_pFbxLoader = new FbxLoader;
	m_pCollisionSys = new CollisionSystem;
	m_pParticleSys = new ParticleSystem;
	m_pUISys = new UISystem;
	m_pPickingSys = new PickingSystem;
	m_pSceneManager = new SceneManager;
	m_pGeometryMaker = new GeometryMaker;
	m_pGameTimer = new CGameTimer;

	m_hInstance = hInstance;
	m_hWnd = hWnd;

	m_pD3DLib->Init( hWnd, nClientWidth, nClientHeight );
}

void D3DEngine::Destroy( )
{
	SAFE_DELETE( m_pGameTimer );
	SAFE_DELETE( m_pGeometryMaker );
	SAFE_DELETE( m_pSceneManager );
	SAFE_DELETE( m_pPickingSys );
	SAFE_DELETE( m_pUISys );
	SAFE_DELETE( m_pParticleSys );
	SAFE_DELETE( m_pCollisionSys );
	SAFE_DELETE( m_pFbxLoader );
	SAFE_DELETE( m_pD3DLib );
}

void D3DEngine::Render( )
{
	// 후면 버퍼 초기화
	float fClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_pD3DLib->ClearRenderTargetView( fClearColor );
	m_pD3DLib->ClearDepthStencilView( D3D11_CLEAR_DEPTH, 1.0f, 0 );

	// 씬에 있는 객체들 렌더링
	m_pSceneManager->RenderScene( m_pD3DLib->GetDeviceContext() );

	// 플리핑
	m_pD3DLib->Present( 0, 0 );
}

// 렌더링 이외에 논리적인 연산 수행
void D3DEngine::FrameAdvance( )
{
	// 타이머 증가
	m_pGameTimer->Tick( );

	// Scene의 객체들 갱신
	m_pSceneManager->UpdateScene( m_pGameTimer->GetTimeElapsed( ) );
}

void D3DEngine::SetFrameBufferSize( int nWidth, int nHeight )
{
	m_nFrameBufferHeight = nHeight;
	m_nFrameBufferWidth = nWidth;
}

int D3DEngine::GetFrameBufferWidth( )const
{
	return m_nFrameBufferWidth;
}

int D3DEngine::GetFrameBufferHeight( )const
{
	return m_nFrameBufferHeight;
}

CScene* D3DEngine::CreateScene( std::string strName )
{
	return m_pSceneManager->CreateScene( m_pD3DLib->GetDevice(), strName );
}

CCamera* D3DEngine::CreateCamera( std::string strName, ObjectLayer iLayer, XMFLOAT3 vPosition, XMFLOAT3 vLook, XMFLOAT3 vRight, XMFLOAT3 vUp, XMFLOAT3 vOffset, CGameObject* objLook,
	DWORD xStart, DWORD yStart, float fMinZ, float fMaxZ, float fNearPlaneDist, float fFarPlaneDist, float fFOVAngle )
{
	CScene* curScene = m_pSceneManager->GetCurrentScene( );
	CCamera* camera = new CCamera( m_pD3DLib->GetDeviceContext( ), strName, iLayer, ObjectType::TYPE_CAMERA, vPosition, vLook, vRight, vUp, vOffset, objLook,
		xStart, yStart, m_nFrameBufferWidth, m_nFrameBufferHeight, fMinZ, fMaxZ, fNearPlaneDist, fFarPlaneDist, (float)m_nFrameBufferWidth / (float)m_nFrameBufferHeight, fFOVAngle );

	if(m_pSceneManager->GetDebugCamera())
		curScene->AddCamera( camera );
	
	return camera;
}

CGameObject* D3DEngine::CreateObject( std::string strName, ObjectLayer iLayer, CMesh* pMesh, ObjectType iType, XMFLOAT3 vPosition )
{
	CGameObject* obj = new CGameObject( m_pD3DLib->GetDevice( ), strName, iLayer, iType );
	obj->SetPosition( vPosition );
	obj->SetMesh( pMesh );
	pMesh->SetParentObject(obj);

	CScene* curScene = m_pSceneManager->GetCurrentScene( );
	curScene->AddObject( obj );

	return obj;
}
CSkyboxObject* D3DEngine::CreateSkyboxObject( std::string strName, ObjectLayer iLayer, CMesh* pMesh, ObjectType iType )
{
	CSkyboxObject* obj = new CSkyboxObject( m_pD3DLib->GetDevice( ), strName, iLayer, iType );
	obj->SetMesh( pMesh );
	pMesh->SetParentObject(obj);

	CScene* curScene = m_pSceneManager->GetCurrentScene( );
	curScene->AddObject( obj );

	return obj;
}

void D3DEngine::CameraLookAtObject( std::string strCamName, std::string strObjName )
{
	m_pSceneManager->GetCurrentScene( )->CameraLookAtObject( strCamName, strObjName );
}

void D3DEngine::SetDebugCamera(CCamera* pCam)
{
	m_pSceneManager->SetDebugCamera(pCam);
}

CGameObject* D3DEngine::GetGameObject( std::string strName )const
{
	return m_pSceneManager->GetCurrentScene( )->GetGameObject( strName );
}

CGameObject* D3DEngine::GetSkyboxObject( std::string strName )
{
	return m_pSceneManager->GetCurrentScene( )->GetGameObject( strName );
}

CCamera* D3DEngine::GetCamera( std::string strName )const
{
	return m_pSceneManager->GetCurrentScene( )->GetCamera( strName );
}

CScene* D3DEngine::GetCurrentScene( )const
{
	return m_pSceneManager->GetCurrentScene( );
}

CScene* D3DEngine::GetScene( std::string strName )const
{
	return m_pSceneManager->GetScene( strName );
}

std::vector<CScene*> D3DEngine::GetScenes( ) const
{
	return m_pSceneManager->GetScenes( );
}

void D3DEngine::OnProcessingMouseMessage( HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam )
{
	m_pSceneManager->GetCurrentScene( )->OnProcessingMouseMessage( hWnd, nMessageID, wParam, lParam );
}

void D3DEngine::OnProcessingKeyboardMessage( HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam )
{
	m_pSceneManager->GetCurrentScene( )->OnProcessingKeyboardMessage( hWnd, nMessageID, wParam, lParam );
}

LRESULT CALLBACK D3DEngine::OnProcessingWindowMessage( HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam )
{
	switch (nMessageID)
	{
		case WM_SIZE:
		{
			m_nWndClientWidth = LOWORD( lParam );
			m_nWndClientHeight = HIWORD( lParam );

			m_pD3DLib->GetDeviceContext( )->OMSetRenderTargets( 0, NULL, NULL );

			if (m_pD3DLib->GetRenderTargetView( )) m_pD3DLib->GetRenderTargetView()->Release( );
			if (m_pD3DLib->GetDepthStencilBuffer( )) m_pD3DLib->GetDepthStencilBuffer( )->Release( );
			if (m_pD3DLib->GetDepthStencilView( )) m_pD3DLib->GetDepthStencilView( )->Release( );
			
			m_pD3DLib->GetSwapChain()->ResizeBuffers( 1, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0 );
			m_pD3DLib->CreateRenderTargetDepthStencilView( m_nWndClientWidth, m_nWndClientHeight );

			if (m_pSceneManager->GetCurrentScene())
			{
				CCamera* cam = m_pSceneManager->GetCurrentScene( )->GetCamera( );
				if (cam)
					cam->SetViewport( m_pD3DLib->GetDeviceContext( ), 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f );
			}
			break;
		}

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
			OnProcessingMouseMessage( hWnd, nMessageID, wParam, lParam );
			break;
		case WM_KEYDOWN:
		case WM_KEYUP:
			OnProcessingKeyboardMessage( hWnd, nMessageID, wParam, lParam );
			break;
	}

	return 0;
}

CStaticMesh* D3DEngine::MakeCube( std::string szName, float fWidth, float fHeight, float fDepth )
{
	return m_pGeometryMaker->MakeCube( m_pD3DLib->GetDevice( ), szName, fWidth, fHeight, fDepth );
}

CStaticMesh* D3DEngine::MakeCircle( std::string szName )
{
	return m_pGeometryMaker->MakeCircle( m_pD3DLib->GetDevice( ), szName );
}

CStaticMesh* D3DEngine::MakeCylinder( std::string szName )
{
	return m_pGeometryMaker->MakeCylinder( m_pD3DLib->GetDevice( ), szName );
}

CStaticMesh* D3DEngine::MakeCapsule( std::string szName )
{
	return m_pGeometryMaker->MakeCapsule( m_pD3DLib->GetDevice( ), szName );
}

CSkyboxMesh* D3DEngine::MakeSkybox( std::string szName, float fWidth, float fHeight, float fDepth )
{
	return m_pGeometryMaker->MakeSkyBox( m_pD3DLib->GetDevice( ), szName, fWidth, fHeight, fDepth );
}

CStaticMesh* D3DEngine::MakeStaticFbxMesh(std::string strName, const char* strFbxFileName)
{
	return m_pGeometryMaker->MakeStaticMeshByFbx(m_pD3DLib->GetDevice(), m_pFbxLoader, strName, strFbxFileName);
}

CAnimateMesh* D3DEngine::MakeAnimateFbxMesh(std::string strName, const char* strFbxFileName)
{
	return m_pGeometryMaker->MakeAnimateMeshByFbx(m_pD3DLib->GetDevice(), m_pFbxLoader, strName, strFbxFileName);
}

CTexture* D3DEngine::ChangeTexture(std::string strObjName, TCHAR* strFileName, std::string strTexName, std::string strSrcName)
{
	return m_pSceneManager->GetCurrentScene()->GetGameObject(strObjName)->GetMaterial()->ChangeTexture(m_pD3DLib->GetDevice(), strTexName, strFileName, strSrcName);
}