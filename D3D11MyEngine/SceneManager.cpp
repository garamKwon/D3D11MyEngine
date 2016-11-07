#include "stdafx.h"
#include "SceneManager.h"


SceneManager::SceneManager( )
{
	m_currentScene = nullptr;
	m_pDebugCamera = nullptr;
}


SceneManager::~SceneManager( )
{
	for (int i = 0; i < m_vScenes.size( ); i++)
	{
		if (m_vScenes[i])
			//	SAFE_DELETE( m_vScenes[i] );
			delete m_vScenes[i];
	}
	m_vScenes.clear( );
}

// 현재 Scene을 반환하는 함수, 현재 Scene이 없다면 nullptr 반환
CScene* SceneManager::GetCurrentScene( ) const
{
	// second가 실질적인 CScene* 이므로 존재하는 검사하고 반환
	if (m_currentScene)
		return m_currentScene;
	else
		return nullptr;
}

std::string SceneManager::GetCurrentSceneName( ) const
{
	if (m_currentScene)
		return m_currentScene->GetName();
	else
		return nullptr;
}

// 매니저가 가지고 있는 Scene들의 이름들을 반환
std::vector<std::string> SceneManager::GetSceneNames( )const
{
	std::vector<std::string> val;
	for (int i = 0; i < m_vScenes.size( ); i++)
		val.push_back( m_vScenes[i]->GetName() );

	return val;
}
CScene* SceneManager::GetScene( std::string strName )const
{
	for (int i = 0; i < m_vScenes.size( ); i++)
	{
		if (m_vScenes[i]->GetName( ) == strName)
		{
			return m_vScenes[i];
		}
	}
	return nullptr;
}

std::vector<CScene*> SceneManager::GetScenes( )const
{
	return m_vScenes;
}

// 현재 씬을 변경, 변경에 성공하면 true 반환
bool SceneManager::ChangeCurrentScene( std::string& szSceneName )
{
	for (int i = 0; i < m_vScenes.size( ); i++)
	{
		if (m_vScenes[i]->GetName() == szSceneName)
		{
			m_currentScene = m_vScenes[i];
			return true;
		}
	}
	return true;
}

// Scene의 이름을 변경, 변경에 성공하면 true 반환
bool SceneManager::ChangeSceneName( std::string& srcName, std::string& dstName )
{
	for (int i = 0; i < m_vScenes.size( ); i++)
	{
		if (m_vScenes[i]->GetName( ) == srcName)
		{
			m_vScenes[i]->SetName( dstName );
			return true;
		}
	}
	return false;
}

// 새로운 Scene 생성, 이름이 중복되지 않게 설정
CScene* SceneManager::CreateScene( ID3D11Device* pd3dDevice, std::string& szSceneName )
{
	for (int i = 0; i < m_vScenes.size(); i++)
	{
		if (m_vScenes[i]->GetName() != szSceneName)
			continue;	// 같지 않으면 다음 걸로 진행
		else
		{
			// 이미 같은 이름의 Scene이 있다는 것을 알림
			return nullptr;		// 같은 것이 있으면 함수 종료
		}
	}
	
	// 여기까지 오면 같은 이름의 Scene이 없는 것이므로 생성
	CScene *newScene = new CScene( pd3dDevice, szSceneName, ObjectLayer::LAYER_SCENE );	// 이름 넣어야 함
	m_vScenes.push_back( newScene );

	if (!m_currentScene)
		m_currentScene = newScene;

	return newScene;
}

// 새로운 Scene을 생성하고 현재 Scene으로 설정, 생성과 설정에 성공하면 true 반환
bool SceneManager::CreateSceneSetCurrentScene( ID3D11Device* pd3dDevice, std::string& szSceneName )
{
	if (!CreateScene( pd3dDevice, szSceneName ))
		return false;
	if (!ChangeCurrentScene( szSceneName ))
		return false;

	return true;
}

// Scene을 제거, 제거에 성공하면 true 반환
bool SceneManager::DeleteScene( std::string& szSceneName )
{
	for (auto i = m_vScenes.begin( ); i != m_vScenes.end( ); i++)
	{
		if ((*i)->GetName() == szSceneName)
		{
			SAFE_DELETE( ( *i ) );
			m_vScenes.erase( i );
			return true;
		}
	}
	return false;
}

// 카메라가 물체를 바라보게 함
void SceneManager::CameraLookAtObject( std::string strCamName, std::string strObjName )
{
	m_currentScene->CameraLookAtObject( strCamName, strObjName );
}

// 현재 Scene을 그림
void SceneManager::RenderScene( ID3D11DeviceContext* pd3dDeviceContext )
{
	// 씬의 모든 오브젝트를 그릴 것
	if (m_currentScene)
	{
#ifdef __DEBUG_RENDERING_INFO__
		std::cout << "Render Start" << std::endl;
		std::cout << "Scene Info" << std::endl;
		std::cout << "Name : " << m_currentScene->GetName( ).c_str( ) << std::endl;
		std::cout << "Number of Camera : " << m_currentScene->GetCameras( ).size( ) << std::endl;
		std::cout << "Number of Light : " << m_currentScene->GetLights( ).size( ) << std::endl;
		std::cout << "Number of Object : " << m_currentScene->GetGameObjects( ).size() << std::endl << std::endl;
		m_currentScene->Render( pd3dDeviceContext );
		std::cout << "Render End" << std::endl<<std::endl;
#endif
#ifdef __DEBUG_MODE__
		m_currentScene->DebugRender(pd3dDeviceContext, m_pDebugCamera);
#else
		m_currentScene->Render(pd3dDeviceContext);
#endif	
	}
}

// 현재 Scene을 갱신
void SceneManager::UpdateScene( float fTimeElapsed )
{
	if (m_currentScene)
	{
#ifdef __DEBUG_MODE__
		m_currentScene->Update(fTimeElapsed, m_pDebugCamera);
#else
		m_currentScene->Update(fTimeElapsed, nullptr);
#endif	
	}
}

void SceneManager::SetDebugCamera(CCamera* pCam)
{
	if (m_pDebugCamera)
		SAFE_DELETE(m_pDebugCamera);

	m_pDebugCamera = pCam;
}

CCamera* SceneManager::GetDebugCamera() const
{
	if (m_pDebugCamera)
		return m_pDebugCamera;

	return nullptr;
}