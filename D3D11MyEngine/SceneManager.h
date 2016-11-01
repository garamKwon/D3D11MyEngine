#pragma once
#include "Scene.h"

// Scene ���� �����ϴ� �Ŵ��� Ŭ����, �� ���� �̸����� �з��Ѵ�
class SceneManager
{
	std::vector<CScene*>  m_vScenes;
	CScene* m_currentScene;		// �� ���������� ���� ���� ã�� ���� ���� �ʱ� ���� ���� ����
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
	bool DeleteScene( std::string& szSceneName );	// true �� ��ȯ�Ǹ� ������ ��

	void CameraLookAtObject( std::string strCamName, std::string strObjName );

	void RenderScene( ID3D11DeviceContext *pd3dDeviceContext );
	void UpdateScene( float fTimeElapsed );

	void SetDebugCamera(CCamera* pCam);
	CCamera* GetDebugCamera() const;

};