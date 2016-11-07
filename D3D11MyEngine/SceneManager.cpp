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

// ���� Scene�� ��ȯ�ϴ� �Լ�, ���� Scene�� ���ٸ� nullptr ��ȯ
CScene* SceneManager::GetCurrentScene( ) const
{
	// second�� �������� CScene* �̹Ƿ� �����ϴ� �˻��ϰ� ��ȯ
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

// �Ŵ����� ������ �ִ� Scene���� �̸����� ��ȯ
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

// ���� ���� ����, ���濡 �����ϸ� true ��ȯ
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

// Scene�� �̸��� ����, ���濡 �����ϸ� true ��ȯ
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

// ���ο� Scene ����, �̸��� �ߺ����� �ʰ� ����
CScene* SceneManager::CreateScene( ID3D11Device* pd3dDevice, std::string& szSceneName )
{
	for (int i = 0; i < m_vScenes.size(); i++)
	{
		if (m_vScenes[i]->GetName() != szSceneName)
			continue;	// ���� ������ ���� �ɷ� ����
		else
		{
			// �̹� ���� �̸��� Scene�� �ִٴ� ���� �˸�
			return nullptr;		// ���� ���� ������ �Լ� ����
		}
	}
	
	// ������� ���� ���� �̸��� Scene�� ���� ���̹Ƿ� ����
	CScene *newScene = new CScene( pd3dDevice, szSceneName, ObjectLayer::LAYER_SCENE );	// �̸� �־�� ��
	m_vScenes.push_back( newScene );

	if (!m_currentScene)
		m_currentScene = newScene;

	return newScene;
}

// ���ο� Scene�� �����ϰ� ���� Scene���� ����, ������ ������ �����ϸ� true ��ȯ
bool SceneManager::CreateSceneSetCurrentScene( ID3D11Device* pd3dDevice, std::string& szSceneName )
{
	if (!CreateScene( pd3dDevice, szSceneName ))
		return false;
	if (!ChangeCurrentScene( szSceneName ))
		return false;

	return true;
}

// Scene�� ����, ���ſ� �����ϸ� true ��ȯ
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

// ī�޶� ��ü�� �ٶ󺸰� ��
void SceneManager::CameraLookAtObject( std::string strCamName, std::string strObjName )
{
	m_currentScene->CameraLookAtObject( strCamName, strObjName );
}

// ���� Scene�� �׸�
void SceneManager::RenderScene( ID3D11DeviceContext* pd3dDeviceContext )
{
	// ���� ��� ������Ʈ�� �׸� ��
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

// ���� Scene�� ����
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