#include "stdafx.h"
#include "Scene.h"
#include "Shader.h"
#include "Material.h"
#include "Mesh.h"

CScene::CScene( ID3D11Device* pd3dDevice, std::string strName, ObjectLayer iLayer, ObjectType iType ) : CBaseObject( strName, iLayer, iType )
{
	CLight* light = new CDirectionalLight( XMFLOAT4( 0.4f, 0.4f, 0.4f, 1.0f ), XMFLOAT4( 0.4f, 0.4f, 0.4f, 1.0f ), XMFLOAT4( 0.0, 0.0, 0.0, 0.0 ),
		XMFLOAT3( 0.0f, -1.0f, 0.0f ), "DirectionalLight", ObjectLayer::LAYER_SCENE, ObjectType::TYPE_DIRECTIONALLIGHT );
	m_vpLights.push_back( light );

	m_pLights = new LIGHTS;

	D3D11_BUFFER_DESC d3dBufferDesc;
	::ZeroMemory( &d3dBufferDesc, sizeof( d3dBufferDesc ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof( LIGHTS );
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	::ZeroMemory( &d3dBufferData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	d3dBufferData.pSysMem = m_pLights;
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dcbLights );

	m_pController = nullptr;
	m_pMainCamera = nullptr;
}

CScene::~CScene( )
{
	for (int i = 0; i < m_vpLights.size( ); i++)
		delete m_vpLights[i];
	for (int i = 0; i < m_vpCameras.size( ); i++)
		delete m_vpCameras[i];
	for (int i = 0; i < m_vpObjects.size( ); i++)
		delete m_vpObjects[i];

	SAFE_DELETE( m_pController );
	SAFE_DELETE( m_pLights );
	SAFE_RELEASE( m_pd3dcbLights );
}

void CScene::AddCamera( CCamera *pCamera )
{
	if (pCamera)
	{
		if ( m_pMainCamera == nullptr )
			m_pMainCamera = pCamera;
	
		m_vpCameras.push_back( pCamera );
	}
}

void CScene::AddObject( CGameObject *pObject )
{
	if (pObject)
		m_vpObjects.push_back( pObject );
}

void CScene::AddLight( CLight* pLight )
{
	if (pLight)
		m_vpLights.push_back( pLight );
}

void CScene::SetMainCamera( CCamera *pCamera )
{
	for (int i = 0; i < m_vpCameras.size( ); i++)
	{
		if (pCamera == m_vpCameras[i])
			m_pMainCamera = m_vpCameras[i];
		else
		{
			m_pMainCamera = pCamera;
			m_vpCameras.push_back( pCamera );
		}
	}
}

void CScene::SetMainCamera( std::string strCamName )
{
	CCamera* cam = GetCamera( strCamName );
	SetMainCamera( cam );
}

void CScene::SetContorller( CController* pController )
{
	if (pController)
		m_pController = pController;
}

void CScene::CameraLookAtObject( std::string strCamName, std::string strObjName )
{
	CCamera* cam = GetCamera( strCamName );
	CGameObject* obj = GetGameObject( strObjName );

	cam->SetLookAtObject( obj );
}

CCamera* CScene::GetCamera( std::string strCamName ) const
{
	for (int i = 0; i < m_vpCameras.size( ); i++)
	{
		if (m_vpCameras[i]->GetName( ) == strCamName)
			return m_vpCameras[i];
	}
}

CCamera* CScene::GetCamera(  ) const
{
	if (m_vpCameras[0])
		return m_vpCameras[0];
	else
		nullptr;
}

CCamera* CScene::GetMainCamera( ) const
{
	return m_pMainCamera;
}

CController* CScene::GetController( ) const
{
	return m_pController;
}

CGameObject* CScene::GetGameObject( std::string strObjName ) const
{
	for (int i = 0; i < m_vpObjects.size( ); i++)
	{
		if (m_vpObjects[i]->GetName( ) == strObjName)
			return m_vpObjects[i];
	}
}

CLight* CScene::GetLight( std::string strLightName ) const
{
	for (int i = 0; i < m_vpLights.size( ); i++)
	{
		if (m_vpLights[i]->GetName( ) == strLightName)
			return m_vpLights[i];
	}
}

std::vector<CCamera* > CScene::GetCameras( ) const
{
	return m_vpCameras;
}

std::vector<CGameObject *> CScene::GetGameObjects( ) const
{
	return m_vpObjects;
}

std::vector<CLight *> CScene::GetLights( ) const
{
	return m_vpLights;
}

void CScene::SetcbLights( ID3D11DeviceContext* pd3dDeviceContext, CCamera *pCamera )
{
	m_pLights->m_cGlobalAmbient = XMFLOAT4( 0.2, 0.2, 0.2, 0.2 );
	m_pLights->m_vCameraPosition = XMFLOAT4( pCamera->GetPosition( ).x, pCamera->GetPosition( ).y, pCamera->GetPosition( ).z, 1.0f );
	// 상수버퍼 연결용 버퍼에 내 라이트들 세팅
	if (m_vpLights.size( ) > MAX_LIGHTS)
	{
		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			m_pLights->m_pLights[i].m_bEnable = true;
			m_pLights->m_pLights[i].m_nType = static_cast<float>(m_vpLights[i]->GetType( ));
			// 색상
			m_pLights->m_pLights[i].m_cAmbient = m_vpLights[i]->GetAmbient( );
			m_pLights->m_pLights[i].m_cSpecular = m_vpLights[i]->GetSpecular( );
			m_pLights->m_pLights[i].m_cDiffuse = m_vpLights[i]->GetDiffuse( );
			// 방향과 위치, 범위
			m_pLights->m_pLights[i].m_vDirection = m_vpLights[i]->GetDirection( );
			m_pLights->m_pLights[i].m_vPosition = m_vpLights[i]->GetPosition( );
			m_pLights->m_pLights[i].m_fRange = m_vpLights[i]->GetRange( );
			// 추가 정보
			m_pLights->m_pLights[i].m_fFalloff = m_vpLights[i]->GetFallOff( );
			m_pLights->m_pLights[i].m_fPhi = m_vpLights[i]->GetPhi( );
			m_pLights->m_pLights[i].m_fTheta = m_vpLights[i]->GetTheta( );
			m_pLights->m_pLights[i].m_vAttenuation = m_vpLights[i]->GetAttenuation( );
		}
	}
	else
	{
		for (int i = 0; i < m_vpLights.size( ); i++)
		{
			m_pLights->m_pLights[i].m_bEnable = true;
			m_pLights->m_pLights[i].m_nType = static_cast<float>( m_vpLights[i]->GetType( ) );
			// 색상
			m_pLights->m_pLights[i].m_cAmbient = m_vpLights[i]->GetAmbient( );
			m_pLights->m_pLights[i].m_cSpecular = m_vpLights[i]->GetSpecular( );
			m_pLights->m_pLights[i].m_cDiffuse = m_vpLights[i]->GetDiffuse( );
			// 방향과 위치, 범위
			m_pLights->m_pLights[i].m_vDirection = m_vpLights[i]->GetDirection( );
			m_pLights->m_pLights[i].m_vPosition = m_vpLights[i]->GetPosition( );
			m_pLights->m_pLights[i].m_fRange = m_vpLights[i]->GetRange( );
			// 추가 정보
			m_pLights->m_pLights[i].m_fFalloff = m_vpLights[i]->GetFallOff( );
			m_pLights->m_pLights[i].m_fPhi = m_vpLights[i]->GetPhi( );
			m_pLights->m_pLights[i].m_fTheta = m_vpLights[i]->GetTheta( );
			m_pLights->m_pLights[i].m_vAttenuation = m_vpLights[i]->GetAttenuation( );
		}
	}

	// 맵을 통해 버퍼로 연결
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map( m_pd3dcbLights, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource );
	LIGHTS *pcbLights = (LIGHTS *)d3dMappedResource.pData;
	memcpy( pcbLights, m_pLights, sizeof( LIGHTS ) );
	pd3dDeviceContext->Unmap( m_pd3dcbLights, 0 );
	pd3dDeviceContext->PSSetConstantBuffers( CLight::CB_LIGHT_SLOT , 1, &m_pd3dcbLights );
}

void CScene::Render( ID3D11DeviceContext* pd3dDeviceContext )
{
#ifdef __DEBUG_RENDERING_INFO__
	ID3D11RenderTargetView *ppRenderTargets = nullptr;
	ID3D11DepthStencilView *ppDepthStencil = nullptr;
	pd3dDeviceContext->OMGetRenderTargets( 1, &ppRenderTargets, &ppDepthStencil );
	if (ppRenderTargets)
		std::cout << "RenderTargetView On" << std::endl;
	if (ppDepthStencil)
		std::cout << "DepthStencilView On" << std::endl << std::endl;

	std::cout << "Camera Info" << std::endl;
	for (int j = 0; j < m_vpCameras.size( ); j++)
	{
		XMFLOAT3 pos = m_vpCameras[j]->GetPosition( );
		XMFLOAT3 up = m_vpCameras[j]->GetUpVector( );
		XMFLOAT3 right = m_vpCameras[j]->GetRightVector( );
		XMFLOAT3 look = m_vpCameras[j]->GetLookVector( );
		std::cout << "Name : " << m_vpCameras[j]->GetName( ).c_str( ) << std::endl;
		std::cout << "Position : " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
		std::cout << "UpVector : " << up.x << ", " << up.y << ", " << up.z << std::endl;
		std::cout << "RightVector : " << right.x << ", " << right.y << ", " << right.z << std::endl;
		std::cout << "LookVector : " << look.x << ", " << look.y << ", " << look.z << std::endl << std::endl;
	}

	std::cout <<"Object Info" << std::endl;
	for (int i = 0; i < m_vpObjects.size( ); i++)
	{
		XMFLOAT3 pos = m_vpObjects[i]->GetPosition( );
		std::cout << "Name : " << m_vpObjects[i]->GetName( ).c_str( ) << std::endl;
		std::cout << "Position : " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
		std::cout << "Number of Mesh : " << m_vpObjects[i]->GetMeshCount( ) << std::endl;
		std::cout << "Mesh Info" << std::endl;
		for (int j = 0; j < m_vpObjects[i]->GetMeshCount( ); j++)
		{
			CMesh* mesh = m_vpObjects[i]->GetMesh( j );
			XMFLOAT4 amb = m_vpObjects[i]->GetMesh(j)->GetMaterial( )->GetAmbient( );
			XMFLOAT4 dif = m_vpObjects[i]->GetMesh( j )->GetMaterial( )->GetDiffuse( );
			XMFLOAT4 spe = m_vpObjects[i]->GetMesh( j )->GetMaterial( )->GetSpecular( );
			XMFLOAT4 emi = m_vpObjects[i]->GetMesh( j )->GetMaterial( )->GetEmissive( );
			std::cout << "Name : " << mesh->GetName( ) << std::endl;
			std::cout << "Material Info" << std::endl;
			std::cout << "Material Diffuse : " << dif.x << ", " << dif.y << ", " << dif.z << ", " << dif.w << std::endl;
			std::cout << "Material Specular : " << spe.x << ", " << spe.y << ", " << spe.z << ", " << spe.w << std::endl;
			std::cout << "Material Ambient : " << amb.x << ", " << amb.y << ", " << amb.z << ", " << amb.w << std::endl;
			std::cout << "Material Emissive : " << emi.x << ", " << emi.y << ", " << emi.z << ", " << emi.w << std::endl;
			std::cout << "Number of Texture : " << m_vpObjects[i]->GetMesh( j )->GetMaterial( )->GetTextures( ).size( ) << std::endl;
			std::cout << "Texture Info" << std::endl;
			for (int k = 0; k < m_vpObjects[i]->GetMesh( j )->GetMaterial( )->GetTextures( ).size( ); k++)
			{
				CTexture* tex = m_vpObjects[i]->GetMesh( j )->GetMaterial( )->GetTexture( k );
				std::cout << "Name : " << tex->GetName( ) << std::endl;;
				std::cout << "FileName : " << tex->GetFileName( ) << std::endl;
			}
		}
		std::cout << std::endl;
	}
#endif	
	int sum = 0;
	// 그려야하는 카메라 개수만큼 물체를 그림
	for (int j = 0; j < m_vpCameras.size( ); j++)
	{
		// 라이트 세팅
		if (m_pLights && m_pd3dcbLights)
			SetcbLights( pd3dDeviceContext, m_vpCameras[j] );

		pd3dDeviceContext->RSSetViewports( 1, &m_vpCameras[j]->GetViewport( ) );	// 뷰포트 세팅

		for (int i = 0; i < m_vpObjects.size( ); i++)
		{
			sum += m_vpObjects[i]->Render( pd3dDeviceContext, m_vpCameras[j] );
		}
	}

#ifdef __DEBUG_RENDERING_INFO__
	std::cout << "RenderMeshCount : " << sum << std::endl;
#endif
}

void CScene::Update( float fTimeElapsed )
{
	if (m_pController)
	{
#ifdef __DEBUG_MODE__
		m_pController->Update( fTimeElapsed, m_pMainCamera );
#else
		m_pController->Update( fTimeElapsed, m_pPlayer );
#endif
	}
	for (int i = 0; i < m_vpCameras.size( ); i++)
		m_vpCameras[i]->Update( fTimeElapsed );

	for (int i = 0; i < m_vpObjects.size(); i++)
	{
		m_vpObjects[i]->Update(fTimeElapsed);
	}
	for (int i = 0; i < m_vpObjects.size(); i++)
	{
		// i+1을 시작위치로 하는 이유는 순차적으로 검사하기에 i 보다 적은 건 이미 체크했기 때문
		for (int j = i+1; j < m_vpObjects.size(); j++)
		{
			if (m_vpObjects[i]->CheckOOBBIntersect(m_vpObjects[j]))
			{
				//충돌
			}
		}
	}
}

void CScene::OnProcessingMouseMessage( HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam )
{
	switch (nMessageID)
	{
		case WM_LBUTTONDOWN:
			SetCapture( hWnd );
			break;
		case WM_RBUTTONDOWN:
			SetCapture( hWnd );
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			ReleaseCapture( );
			break;
		case WM_MOUSEMOVE:
			break;
		default:
			break;
	}
}

void CScene::OnProcessingKeyboardMessage( HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam )
{
	switch (nMessageID)
	{
		case WM_KEYUP:
			switch (wParam)
			{
				case VK_ESCAPE:
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}