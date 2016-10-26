#pragma once
#include "D3DSetupLib.h"
#include "FbxLoader.h"
#include "CollisionSystem.h"
#include "ParticleSystem.h"
#include "SceneManager.h"
#include "UISystem.h"
#include "PickingSystem.h"
#include "GeometryMaker.h"
#include "GameTimer.h"

// 모든걸 최종적으로 가지는 엔진 클래스
// 이 엔진 객체를 생성하고 인터페이스를 이용하여 게임 제작
// init ( 내부 객체 초기화 )
// gameObject 생성 ( 게임에서 사용할 객체들 생성 )
// calculate logic ( 게임 내 로직 계산 )
// render ( 객체들 렌더링 )

class D3DEngine
{
protected:
	D3D11SetUp *m_pD3DLib;
	FbxLoader *m_pFbxLoader;
	CollisionSystem *m_pCollisionSys;
	ParticleSystem *m_pParticleSys;
	UISystem *m_pUISys;
	PickingSystem *m_pPickingSys;
	GeometryMaker *m_pGeometryMaker;
	CGameTimer	*m_pGameTimer;

	SceneManager *m_pSceneManager;

	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int m_nWndClientWidth;
	int m_nWndClientHeight;
	int m_nFrameBufferWidth;
	int m_nFrameBufferHeight;

public:
	D3DEngine( );
	~D3DEngine( );

	// 반드시 수행해야 하는 함수들
public:
	void Init( HINSTANCE hInstance, HWND hWnd, UINT nClientWidth, UINT nClientHeight );
	void Destroy( );
	void Render( );
	void FrameAdvance( );	

	// 객체 관련
	CScene* CreateScene( std::string strName );
	CCamera* CreateCamera( std::string strName, ObjectLayer iLayer, XMFLOAT3 vPosition = XMFLOAT3( 0, 0, 0 ),
		XMFLOAT3 vLook = XMFLOAT3( 0, 0, 1 ), XMFLOAT3 vRight = XMFLOAT3( 1, 0, 0 ), XMFLOAT3 vUp = XMFLOAT3( 0, 1, 0 ), XMFLOAT3 vOffset = XMFLOAT3( 0, 0, 0 ),
		CGameObject* objLook = nullptr, DWORD xStart = 0, DWORD yStart = 0, float fMinZ = 0.0f, float fMaxZ = 1.0f,
		float fNearPlaneDist = 1.01f, float fFarPlaneDist = 50000.0f, float fFOVAngle = 60.0f );
	CGameObject* CreateObject( std::string strName, ObjectLayer iLayer, CMesh* pMesh = nullptr, ObjectType iType = ObjectType::TYPE_OBJECT, XMFLOAT3 vPosition = XMFLOAT3( 0, 0, 0 ) );
	CSkyboxObject* CreateSkyboxObject( std::string strName, ObjectLayer iLayer, CMesh* pMesh = nullptr, ObjectType iType = ObjectType::TYPE_SKYBOX );

	CGameObject* GetGameObject( std::string strName )const;
	CGameObject* GetSkyboxObject( std::string strName );
	CCamera* GetCamera( std::string strName )const;
	CScene* GetCurrentScene( )const;
	CScene* GetScene( std::string strName )const;
	std::vector<CScene*> GetScenes( ) const;

	void CameraLookAtObject( std::string strCamName, std::string strObjName );

	// 윈도우 관련
	void OnProcessingMouseMessage( HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam );
	void OnProcessingKeyboardMessage( HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam );
	LRESULT CALLBACK OnProcessingWindowMessage( HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam );
	void SetFrameBufferSize( int nWidth, int nHeight );
	int GetFrameBufferWidth( )const;
	int GetFrameBufferHeight( )const;

	// 기하구조 생성
	CStaticMesh* MakeCube( std::string szName, float fWidth = 5.0f, float fHeight = 5.0f, float fDepth = 5.0f );
	CStaticMesh* MakeCircle( std::string szName );
	CStaticMesh* MakeCylinder( std::string szName );
	CStaticMesh* MakeCapsule( std::string szName );
	CSkyboxMesh* MakeSkybox( std::string szName, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.f );
	CStaticMesh* MakeStaticFbxMesh(std::string strName, const char* strFbxFileName);
	CAnimateMesh* MakeAnimateFbxMesh(std::string strName, const char* strFbxFileName);

public:

};

