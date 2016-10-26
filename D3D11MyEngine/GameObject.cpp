#include "stdafx.h"
#include "GameObject.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"


void CBaseObject::SetType( ObjectType type )
{
	m_iType = type;
}

void CBaseObject::SetLayer( ObjectLayer layer )
{
	m_iLayer = layer;
}

void CBaseObject::SetName( std::string strName )
{
	m_strName = strName;
}

ObjectType CBaseObject::GetType( ) const
{
	return m_iType;
}

ObjectLayer CBaseObject::GetLayer( ) const
{
	return m_iLayer;
}

std::string CBaseObject::GetName( ) const
{
	return m_strName;
}

CGameObject::CGameObject( ID3D11Device* pd3dDevice, std::string strName, ObjectLayer iLayer, ObjectType iType ) : CBaseObject( strName, iLayer, iType )
{
	m_vpMesh.resize( 0 );
	m_nMeshCount = 0;

	m_fPitch = 0.0f;
	m_fYaw = 0.0f;
	m_fRoll = 0.0f;
	m_fXScale = 1.0f;
	m_fYScale = 1.0f;
	m_fZScale = 1.0f;

	XMStoreFloat4x4( &m_mtxWorld, XMMatrixIdentity( ) );

	//XMFLOAT4X4 trans = XMFLOAT4X4(1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1);
	//XMMATRIX mtx = XMLoadFloat4x4(&m_mtxWorld);
	//XMMATRIX transMtx = XMLoadFloat4x4(&trans);
	//XMMatrixMultiply(mtx, transMtx);
	//XMStoreFloat4x4(&m_mtxWorld, mtx);

	m_fMoveSpeed = 100.0f;
}


CGameObject::~CGameObject( )
{
}


XMFLOAT3 CGameObject::GetLookAt( ) const
{
	XMFLOAT3 vLookAt( m_mtxWorld._31, m_mtxWorld._32, m_mtxWorld._33 );
	XMVECTOR vTemp = XMLoadFloat3( &vLookAt );
	XMVECTOR vNormalize = XMVector3Normalize( vTemp );
	XMStoreFloat3( &vLookAt, vNormalize );

	return vLookAt;
}

XMFLOAT3 CGameObject::GetUp( ) const
{
	XMFLOAT3 vUp( m_mtxWorld._21, m_mtxWorld._22, m_mtxWorld._23 );
	XMVECTOR vTemp = XMLoadFloat3( &vUp );
	XMVECTOR vNormalize = XMVector3Normalize( vTemp );

	return vUp;
}

XMFLOAT3 CGameObject::GetRight( ) const
{
	XMFLOAT3 vRight( m_mtxWorld._11, m_mtxWorld._12, m_mtxWorld._13 );
	XMVECTOR vTemp = XMLoadFloat3( &vRight );
	XMVECTOR vNormalize = XMVector3Normalize( vTemp );

	return vRight;
}

XMFLOAT3 CGameObject::GetPosition( ) const
{
	return XMFLOAT3( m_mtxWorld._41, m_mtxWorld._42, m_mtxWorld._43 );
}

void CGameObject::SetPosition( XMFLOAT3& pos )
{
	SetPosition( pos.x, pos.y, pos.z );
}

void CGameObject::SetPosition( float x, float y, float z )
{
	m_mtxWorld._41 = x;
	m_mtxWorld._42 = y;
	m_mtxWorld._43 = z;
}
void CGameObject::Update( float fTimeElapsed )
{
	for (int i = 0; i < m_vpMesh.size(); i++)
	{
		m_vpMesh[i]->Update(fTimeElapsed);
	}
}

void CGameObject::Animate( float fTimeElapsed )
{

}

int CGameObject::Render( ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera )
{
	if (!pCamera)
	{
		// 카메라가 없으면 그릴 필요 없음
		return 0;
	}
	// 그릴 메시가 하나라도 있으면	
	int renderingMesh = 0;
	for (int i = 0; i < m_vpMesh.size( ); i++)
	{
		if (m_vpMesh[i])
		{
			AABB bcBoundingCube = m_vpMesh[i]->GetBoundingBox( );
			bcBoundingCube.Update( &m_mtxWorld );

			if (pCamera->IsInFrustum(&bcBoundingCube))	// 안보이면 안그림
			{
				m_vpMesh[i]->SetIsVisible(true);
				m_vpMesh[i]->GetShader( )->SetVertexShaderContantBuffers( pd3dDeviceContext, &m_mtxWorld, pCamera );
				m_vpMesh[i]->Render( pd3dDeviceContext );
				renderingMesh++;
			}
			else
				m_vpMesh[i]->SetIsVisible(false);
		}
	}
	return renderingMesh;
}

void CGameObject::Rotate( float fPitch, float fYaw, float fRoll )
{
	// 게임 객체를 주어진 각도로 회전
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw( (float)XMConvertToRadians( fPitch ), (float)XMConvertToRadians( fYaw ), (float)XMConvertToRadians( fRoll ) );
	XMMATRIX mtxWorld = XMLoadFloat4x4( &m_mtxWorld);

	XMMATRIX rotateMtx = XMMatrixMultiply( mtxRotate, mtxWorld );
	XMStoreFloat4x4( &m_mtxWorld, rotateMtx );

	m_fPitch = fPitch;
	m_fYaw = fYaw;
	m_fRoll = fRoll;
}

void CGameObject::Rotate( XMFLOAT3 *pvAxis, float fAngle )
{
	XMVECTOR tmp = XMLoadFloat3( pvAxis );
	XMMATRIX mtxRotate = XMMatrixRotationAxis( tmp, (float)XMConvertToRadians( fAngle ) );
	XMMATRIX mtxWorld = XMLoadFloat4x4( &m_mtxWorld );

	XMMATRIX rotateMtx = XMMatrixMultiply( mtxRotate, mtxWorld );
	XMStoreFloat4x4( &m_mtxWorld, rotateMtx );
}

float CGameObject::GetPitch( ) const
{
	return m_fPitch;
}

float CGameObject::GetYaw( ) const
{
	return m_fYaw;
}

float CGameObject::GetRoll( ) const
{
	return m_fRoll;
}

void CGameObject::Scale( float x, float y, float z )
{
	XMMATRIX mtxScale = XMMatrixScaling( x, y, z );
	XMMATRIX mtxWorld = XMLoadFloat4x4( &m_mtxWorld );

	XMMATRIX scaleMtx = XMMatrixMultiply( mtxScale, mtxWorld );
	XMStoreFloat4x4( &m_mtxWorld, mtxScale );

	m_fXScale = x;
	m_fYScale = y;
	m_fZScale = z;
}

float CGameObject::GetXScale( ) const
{
	return m_fXScale;
}

float CGameObject::GetYScale( ) const
{
	return m_fYScale;
}

float CGameObject::GetZScale( ) const
{
	return m_fZScale;
}

void CGameObject::SetMesh( int idx, CMesh* pMesh )
{
	if (idx < m_nMeshCount)
	{
		SAFE_DELETE( m_vpMesh[idx] );
		m_vpMesh[idx] = pMesh;
	}
}

void CGameObject::SetMesh( CMesh* pMesh )
{
	m_vpMesh.push_back( pMesh );
	m_nMeshCount++;
}

CMesh* CGameObject::GetMesh( int idx )
{
	if (idx < m_nMeshCount)
		return m_vpMesh[idx];
}

CMesh* CGameObject::GetMesh( std::string strName )
{
	for (int i = 0; i < m_vpMesh.size( ); i++)
	{
		if (m_vpMesh[i]->GetName() == strName)
			return m_vpMesh[i];
	}
}

int CGameObject::GetMeshCount( ) const
{
	return m_nMeshCount;
}

void CGameObject::Move( float fTimeElapsed, DWORD dwDirection )
{
	if (dwDirection)
	{
		XMVECTOR vShift = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );

		if (dwDirection & DIR_FORWARD){
			XMVECTOR vLook = XMLoadFloat3( &GetLookAt( ) );
			vLook = vLook * m_fMoveSpeed * fTimeElapsed;
			vShift = vShift + vLook;
		}
		if (dwDirection & DIR_BACKWARD){
			XMVECTOR vLook = XMLoadFloat3( &GetLookAt( ) );
			vLook = vLook * m_fMoveSpeed * fTimeElapsed;
			vShift = vShift - vLook;
		}
		if (dwDirection & DIR_RIGHT){
			XMVECTOR vRight = XMLoadFloat3( &GetRight( ) );
			vRight = vRight * m_fMoveSpeed * fTimeElapsed;
			vShift = vShift + vRight;
		}
		if (dwDirection & DIR_LEFT){
			XMVECTOR vRight = XMLoadFloat3( &GetRight( ) );
			vRight = vRight * m_fMoveSpeed * fTimeElapsed;
			vShift = vShift - vRight;
		}
		if (dwDirection & DIR_UP){
			XMVECTOR vUp = XMLoadFloat3( &GetUp( ) );
			vUp = vUp * m_fMoveSpeed * fTimeElapsed;
			vShift = vShift + vUp;
		}
		if (dwDirection & DIR_DOWN){
			XMVECTOR vDown = XMLoadFloat3( &GetUp( ) );
			vDown = vDown * m_fMoveSpeed * fTimeElapsed;
			vShift = vShift - vDown;
		}
		Move( vShift );
	}
}

void CGameObject::Move( XMVECTOR vShift )
{
	XMVECTOR pos = XMLoadFloat3( &GetPosition( ) );
	XMFLOAT3 vPos;
	XMStoreFloat3( &vPos, pos + vShift );
	SetPosition( vPos );

#ifdef __DEBUG_POS__
	std::cout << "Object Postion : " << vPos.x << ", " << vPos.y << ", " << vPos.z << std::endl;
#endif	
}

CSkyboxObject::CSkyboxObject( ID3D11Device* pd3dDevice, std::string strName, ObjectLayer iLayer, ObjectType iType ) : CGameObject( pd3dDevice, strName, iLayer, iType )
{

}

CSkyboxObject::~CSkyboxObject( )
{

}

int CSkyboxObject::Render( ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera )
{
	if (!pCamera)
	{
		// 카메라가 없으면 그릴 필요 없음
		return 0;
	}
	// 그릴 메시가 하나라도 있으면	
	int renderingMesh = 0;

	// 카메라의 위치로 스카이박스 위치를 옮김
	XMFLOAT3 camPos = pCamera->GetPosition( );
	SetPosition( camPos );

	for (int i = 0; i < m_vpMesh.size( ); i++)
	{
		if (m_vpMesh[i])
		{
			m_vpMesh[i]->GetShader( )->SetVertexShaderContantBuffers( pd3dDeviceContext, &m_mtxWorld, pCamera );
			m_vpMesh[i]->Render( pd3dDeviceContext );
			renderingMesh++;
		}
	}
	return renderingMesh;
}

void CSkyboxObject::Update( float fTimeElapsed )
{

}