#pragma once
#include "GameObject.h"
#include "Camera.h"
#include "Light.h"
#include "Controller.h"

// ������ �� ���, �ش� ���� �����ϴ� ��� ������Ʈ���� ������ �ִ�.
class CScene : public CBaseObject
{
	// ��ü��
	std::vector<CGameObject*> m_vpObjects;
	// ī�޶��, �� ���� ���� ī�޶� ����� �� �ֱ� ������ ���Ϳ� ����
	CCamera* m_pMainCamera;
	std::vector<CCamera*> m_vpCameras;
	// ���� ���� ���� ���� �� ����
	std::vector<CLight*> m_vpLights;
	// ������� ����� ����Ʈ
	LIGHTS* m_pLights;
	// ����Ʈ ��� ����
	ID3D11Buffer* m_pd3dcbLights;

	// �ش� ���� ��Ʈ�ѷ�
	CController* m_pController;
private:
	void SetcbLights( ID3D11DeviceContext* pd3dDeviceContext, CCamera *pCamera );

public:
	CScene( ID3D11Device* pd3dDevice, std::string strName, ObjectLayer iLayer, ObjectType iType = ObjectType::TYPE_SCENE );
	~CScene( );

	void AddCamera( CCamera *pCamera );
	void AddObject( CGameObject *pObject );
	void AddLight( CLight* pLight );
	void SetMainCamera( CCamera *pCamera );
	void SetMainCamera( std::string strCamName );
	void SetContorller( CController* pController );
	void CameraLookAtObject( std::string strCamName, std::string strObjName );

	CCamera* GetCamera( std::string strCamName ) const;
	CCamera* GetCamera( ) const;
	CCamera* GetMainCamera( ) const;
	CController* GetController( ) const;
	CGameObject* GetGameObject( std::string strObjName ) const;
	CLight* GetLight( std::string strLightName ) const;	
	
	std::vector<CCamera* > GetCameras( ) const;
	std::vector<CGameObject *> GetGameObjects( ) const;
	std::vector<CLight *> GetLights( ) const;

	void Render( ID3D11DeviceContext* pd3dDeviceContext );
	void Update( float fTimeElapsed );

	void OnProcessingMouseMessage( HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam );
	void OnProcessingKeyboardMessage( HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam );
};

