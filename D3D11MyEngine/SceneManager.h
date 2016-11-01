#pragma once
#include "Scene.h"

// Scene 들을 관리하는 매니저 클래스, 각 씬은 이름으로 분류한다
class SceneManager
{
	std::vector<CScene*>  m_vScenes;
	CScene* m_currentScene;		// 매 렌더링마다 현재 씬을 찾는 일을 하지 않기 위해 따로 지정
	CCamera* m_pDebugCamera;

public:
	SceneManager( );
	~SceneManager( );

	CScene* GetCurrentScene( ) const;
	std::string GetCurrentSceneName( ) const;
	std::vector<std::string> GetSceneNames()const;
	CScene* GetScene( std::string strName )const;
	std::vector<CScene*> GetScenes( )const;

	bool ChangeCurrentScene( std::string& szSceneName );
	bool ChangeSceneName( std::string& srcName, std::string& dstName );
	CScene* CreateScene( ID3D11Device* pd3dDevice, std::string& szSceneName );
	bool CreateSceneSetCurrentScene( ID3D11Device* pd3dDevice, std::string& szSceneName );
	bool DeleteScene( std::string& szSceneName );	// true 가 반환되면 지워진 것

	void CameraLookAtObject( std::string strCamName, std::string strObjName );

	void RenderScene( ID3D11DeviceContext *pd3dDeviceContext );
	void UpdateScene( float fTimeElapsed );

	void SetDebugCamera(CCamera* pCam);
	CCamera* GetDebugCamera() const;

};