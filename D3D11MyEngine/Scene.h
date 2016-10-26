#pragma once
#include "GameObject.h"
#include "Camera.h"
#include "Light.h"
#include "Controller.h"

// 게임의 한 장면, 해당 씬에 존재하는 모든 오브젝트들을 가지고 있다.
class CScene : public CBaseObject
{
	// 객체들
	std::vector<CGameObject*> m_vpObjects;
	// 카메라들, 한 씬에 여러 카메라를 사용할 수 있기 때문에 벡터에 저장
	CCamera* m_pMainCamera;
	std::vector<CCamera*> m_vpCameras;
	// 조명 정보 등을 가질 수 있음
	std::vector<CLight*> m_vpLights;
	// 상수버퍼 연결용 라이트
	LIGHTS* m_pLights;
	// 라이트 상수 버퍼
	ID3D11Buffer* m_pd3dcbLights;

	// 해당 씬의 컨트롤러
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

