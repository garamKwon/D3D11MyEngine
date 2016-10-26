#include "stdafx.h"
#include "Camera.h"
#include "GameObject.h"
#include "Mesh.h"
#include <D3DX10math.h>

CCamera::CCamera( ID3D11DeviceContext *pd3dDeviceContext, std::string strName, ObjectLayer iLayer, ObjectType iType,
	XMFLOAT3 vPosition, XMFLOAT3 vLook, XMFLOAT3 vRight, XMFLOAT3 vUp, XMFLOAT3 vOffset, CGameObject* objLook,
	DWORD xStart, DWORD yStart, DWORD nWidth, DWORD nHeight, float fMinZ, float fMaxZ,
	float fNearPlaneDist, float fFarPlaneDist, float fAspectRatio, float fFOVAngle ) : CBaseObject( strName, iLayer, iType )
{
	m_vPosition = vPosition;
	m_vRight = vRight;
	m_vUp = vUp;
	m_vLook = vLook;
	m_vOffset = vOffset;
	m_pobjLookAtObject = objLook;

	m_fMoveSpeed = 100;
	m_fPitch = 0;
	m_fRoll = 0;
	m_fYaw = 0;

	GenerateProjectionMatrix( nWidth, nHeight, fNearPlaneDist, fFarPlaneDist, fAspectRatio, fFOVAngle );
	GenerateViewMatrix( );
	SetViewport( pd3dDeviceContext, xStart, yStart, nWidth, nHeight, fMinZ, fMaxZ );
	CalculateFrustumPlanes( );
}

CCamera::~CCamera( )
{
}

D3D11_VIEWPORT CCamera::GetViewport( ) const
{
	return m_d3dViewport;
}

XMFLOAT4X4 CCamera::GetViewMatrix( ) const
{ 
	return m_mtxView;
}

XMFLOAT4X4 CCamera::GetProjectionMatrix( ) const
{
	return m_mtxProjection;
}

XMFLOAT4X4 CCamera::GetOrthMatrix( ) const
{
	return m_mtxOrtho;
}

XMFLOAT3 CCamera::GetRightVector( ) const
{
	return m_vRight;
}

XMFLOAT3 CCamera::GetUpVector( ) const
{
	return m_vUp;
}

XMFLOAT3 CCamera::GetLookVector( ) const
{
	return m_vLook;
}

CGameObject* CCamera::GetLookAtObject( ) const
{
	return m_pobjLookAtObject;
}

XMFLOAT3 CCamera::GetPosition( )const
{
	return m_vPosition;
}

void CCamera::SetPosition( XMFLOAT3 pos )
{
	m_vPosition = pos;
}

XMFLOAT3 CCamera::GetOffset( ) const
{
	return m_vOffset;
}

void CCamera::SetOffset( XMFLOAT3 offset )
{
	m_vOffset = offset;
}

void CCamera::SetMoveSpeed( float speed )
{
	m_fMoveSpeed = speed; 
}

void CCamera::CalculateFrustumPlanes( )
{
	XMMATRIX tmp1 = XMLoadFloat4x4( &m_mtxView );
	XMMATRIX tmp2 = XMLoadFloat4x4( &m_mtxProjection );

	XMMATRIX dest = XMMatrixMultiply( tmp1, tmp2 );

	XMFLOAT4X4 mtxViewProjection;
	XMStoreFloat4x4( &mtxViewProjection, dest );

	// ����ü�� ���� ���
	m_FrustumPlanes[0].x = -( mtxViewProjection._14 + mtxViewProjection._11 );
	m_FrustumPlanes[0].y = -( mtxViewProjection._24 + mtxViewProjection._21 );
	m_FrustumPlanes[0].z = -( mtxViewProjection._34 + mtxViewProjection._31 );
	m_FrustumPlanes[0].w = -( mtxViewProjection._44 + mtxViewProjection._41 );
	// ����ü�� ������ ���
	m_FrustumPlanes[1].x = -( mtxViewProjection._14 - mtxViewProjection._11 );
	m_FrustumPlanes[1].y = -( mtxViewProjection._24 - mtxViewProjection._21 );
	m_FrustumPlanes[1].z = -( mtxViewProjection._34 - mtxViewProjection._31 );
	m_FrustumPlanes[1].w = -( mtxViewProjection._44 - mtxViewProjection._41 );
	// ����ü�� ���� ���
	m_FrustumPlanes[2].x = -( mtxViewProjection._14 - mtxViewProjection._12 );
	m_FrustumPlanes[2].y = -( mtxViewProjection._24 - mtxViewProjection._22 );
	m_FrustumPlanes[2].z = -( mtxViewProjection._34 - mtxViewProjection._32 );
	m_FrustumPlanes[2].w = -( mtxViewProjection._44 - mtxViewProjection._42 );
	// ����ü�� �Ʒ��� ���
	m_FrustumPlanes[3].x = -( mtxViewProjection._14 + mtxViewProjection._12 );
	m_FrustumPlanes[3].y = -( mtxViewProjection._24 + mtxViewProjection._22 );
	m_FrustumPlanes[3].z = -( mtxViewProjection._34 + mtxViewProjection._32 );
	m_FrustumPlanes[3].w = -( mtxViewProjection._44 + mtxViewProjection._42 );
	// ����ü�� �����
	m_FrustumPlanes[4].x = -( mtxViewProjection._13 );
	m_FrustumPlanes[4].y = -( mtxViewProjection._23 );
	m_FrustumPlanes[4].z = -( mtxViewProjection._33 );
	m_FrustumPlanes[4].w = -( mtxViewProjection._43 );
	// ����ü�� �����
	m_FrustumPlanes[5].x = -( mtxViewProjection._14 - mtxViewProjection._13 );
	m_FrustumPlanes[5].y = -( mtxViewProjection._24 - mtxViewProjection._23 );
	m_FrustumPlanes[5].z = -( mtxViewProjection._34 - mtxViewProjection._33 );
	m_FrustumPlanes[5].w = -( mtxViewProjection._44 - mtxViewProjection._43 );

	for (int i = 0; i < 6; i++)
	{
		XMVECTOR tmp = XMLoadFloat4( &m_FrustumPlanes[i] );
		tmp = XMVector3Normalize( tmp );

		XMStoreFloat4( &m_FrustumPlanes[i], tmp );
	}
}

void CCamera::SetViewport( ID3D11DeviceContext *pd3dDeviceContext, DWORD xTopLeft, DWORD yTopLeft, DWORD nWidth, DWORD nHeight, float fMinZ, float fMaxZ )
{
	m_d3dViewport.TopLeftX = float( xTopLeft );
	m_d3dViewport.TopLeftY = float( yTopLeft );
	m_d3dViewport.Width = float( nWidth );
	m_d3dViewport.Height = float( nHeight );
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
}

void CCamera::GenerateViewMatrix( )
{
	XMFLOAT3 Focus;
	if (m_pobjLookAtObject)
		Focus = m_pobjLookAtObject->GetPosition( );
	else
		Focus = XMFLOAT3( m_vLook.x, m_vLook.y, m_vLook.z*30 );

	XMVECTOR EyePosition = XMLoadFloat3( &m_vPosition );
	XMVECTOR FocusPosition = XMLoadFloat3( &Focus );
	XMVECTOR UpDirection = XMLoadFloat3( &m_vUp );

	XMMATRIX tmp = XMMatrixLookAtLH( EyePosition, FocusPosition, UpDirection );
	XMStoreFloat4x4( &m_mtxView, tmp );

	m_vRight = XMFLOAT3( m_mtxView._11, m_mtxView._21, m_mtxView._31 );
	m_vUp = XMFLOAT3( m_mtxView._12, m_mtxView._22, m_mtxView._32 );
	m_vLook = XMFLOAT3( m_mtxView._13, m_mtxView._23, m_mtxView._33 );

}

void CCamera::RegenerateViewMatrix( )
{
	// ī�޶��� z�� ���͸� ����ȭ
	XMVECTOR vLook = XMLoadFloat3( &m_vLook ), vRight = XMLoadFloat3( &m_vRight ), vUp = XMLoadFloat3( &m_vUp );
	XMVECTOR vPosition = XMLoadFloat3( &m_vPosition );

	vLook = XMVector3Normalize( vLook );
	// ī�޶��� z��� y�࿡ ������ ���͸� x������ ����
	vRight = XMVector3Cross( vUp, vLook );
	// ī�޶��� x�� ���͸� ����ȭ
	vRight = XMVector3Normalize( vRight );
	// ī�޶��� z��� x�࿡ ������ ���͸� y������ �����Ѵ�.
	vUp = XMVector3Cross( vLook, vRight );
	// ī�޶��� y�� ���͸� ����ȭ
	vUp = XMVector3Normalize( vUp );

	XMStoreFloat3( &m_vLook, vLook );
	XMStoreFloat3( &m_vRight, vRight );
	XMStoreFloat3( &m_vUp, vUp );

	m_mtxView._11 = m_vRight.x; m_mtxView._12 = m_vUp.x; m_mtxView._13 = m_vLook.x;
	m_mtxView._21 = m_vRight.y; m_mtxView._22 = m_vUp.y; m_mtxView._23 = m_vLook.y;
	m_mtxView._31 = m_vRight.z; m_mtxView._32 = m_vUp.z; m_mtxView._33 = m_vLook.z;
	m_mtxView._41 = -( XMVectorGetX( XMVector3Dot( vPosition, vRight )) );
	m_mtxView._42 = -( XMVectorGetX( XMVector3Dot( vPosition, vUp )) );
	m_mtxView._43 = -( XMVectorGetX( XMVector3Dot( vPosition, vLook )) );

	CalculateFrustumPlanes( );
}

void CCamera::GenerateProjectionMatrix( float nWidth, float nHeight, float fNearPlaneDist, float fFarPlaneDist, float fAspectRatio, float fFOVAngle )
{
	XMMATRIX temp = XMMatrixPerspectiveFovLH( (float)XMConvertToRadians( fFOVAngle ), fAspectRatio, fNearPlaneDist, fFarPlaneDist );
	XMStoreFloat4x4( &m_mtxProjection, temp );

	temp = XMMatrixOrthographicLH( nWidth, nHeight, 0.1f, 1000.0f );
	XMStoreFloat4x4( &m_mtxOrtho, temp );
}

void CCamera::SetLookAt( XMFLOAT3& vLookAt, XMFLOAT3& vUp )
{
	XMVECTOR vEyePosition, vFocusPosition, vUpDirection;
	vEyePosition = XMLoadFloat3( &m_vPosition );
	vFocusPosition = XMLoadFloat3( &vLookAt );
	vUpDirection = XMLoadFloat3( &vUp );

	XMMATRIX tmp = XMMatrixLookAtLH( vEyePosition, vFocusPosition, vUpDirection );
	XMFLOAT4X4 mtxLookAt;
	XMStoreFloat4x4( &mtxLookAt, tmp );

	m_vRight = XMFLOAT3( mtxLookAt._11, mtxLookAt._21, mtxLookAt._31 );
	m_vUp = XMFLOAT3( mtxLookAt._12, mtxLookAt._22, mtxLookAt._32 );
	m_vLook = XMFLOAT3( mtxLookAt._13, mtxLookAt._23, mtxLookAt._33 );
}

void CCamera::SetLookAtObject( CGameObject* pObject )
{
	if (pObject)
	{
		m_pobjLookAtObject = pObject;
		SetLookAt( pObject->GetPosition( ), m_vUp );
		RegenerateViewMatrix( );
	}
	
}

bool CCamera::IsInFrustum( XMFLOAT3& vMin, XMFLOAT3& vMax )
{
	XMFLOAT3 vNearPoint, vFarPoint, vNormal;
	for (int i = 0; i < 6; i++)
	{
		// ����ü�� �� ��鿡 ���Ͽ� �ٿ�� �ڽ��� ������ ���
		// �������� x,y,z ��ǥ�� ���� ������ �� ��Ұ� �����̸� �ٿ�� �ڽ��� �ִ����̰�, �׷��� ������ �ּ����� �ȴ�
		vNormal = XMFLOAT3( m_FrustumPlanes[i].x, m_FrustumPlanes[i].y, m_FrustumPlanes[i].z );
		if (vNormal.x >= 0.0f)
		{
			if (vNormal.y > 0.0f)
			{
				if (vNormal.z >= 0.0f)
				{
					// ���������� x,y,z ��ǥ�� ��ȣ�� ��� ����̹Ƿ� �������� �ٿ�� �ڽ��� �ּ���
					vNearPoint.x = vMin.x;
					vNearPoint.y = vMin.y;
					vNearPoint.z = vMin.z;
				}
				else
				{
					// ���������� x,y,z ��ǥ�� ��ȣ�� ��� ����̹Ƿ� �������� x,y��ǥ�� �ٿ�� �ڽ��� �ּ����� x,y ��ǥ�̴�
					//���� ������ z��ǥ�� �����̹Ƿ� �������� z��ǥ�� �ٿ�� �ڽ��� �ִ����� z��ǥ
					vNearPoint.x = vMin.x;
					vNearPoint.y = vMin.y;
					vNearPoint.z = vMax.z;
				}
			}
			else
			{
				if (vNormal.z >= 0.0f)
				{
					// ���������� x,z ��ǥ�� ��ȣ�� ����̹Ƿ� �������� x,z��ǥ�� �ٿ�� �ڽ��� �ּ����� x,z��ǥ
					// ���������� y��ǥ�� �����̹Ƿ� �������� y��ǥ�� �ٿ�� �ڽ��� �ִ����� y ��ǥ
					vNearPoint.x = vMin.x;
					vNearPoint.y = vMax.y;
					vNearPoint.z = vMin.z;
				}
				else
				{
					// ���������� y,z ��ǥ�� ��ȣ�� ��� �����̹Ƿ� �������� y,z��ǥ�� �ٿ�� �ڽ��� �ִ����� y,z��ǥ
					// ���������� x��ǥ�� ����̹Ƿ� �������� x��ǥ�� �ٿ�� �ڽ��� �ּ����� x ��ǥ
					vNearPoint.x = vMin.x;
					vNearPoint.y = vMax.y;
					vNearPoint.z = vMax.z;
				}
			}
		}
		else
		{
			if (vNormal.y >= 0.0f)
			{
				if (vNormal.z >= 0.0f)
				{
					//���� ������ y, z ��ǥ�� ��ȣ�� ��� ����̹Ƿ� �������� y, z ��ǥ�� �ٿ�� �ڽ��� �ּ����� y, z ��ǥ
					//���� ������ x ��ǥ�� �����̹Ƿ� �������� x ��ǥ�� �ٿ�� �ڽ��� �ִ����� x ��ǥ
					vNearPoint.x = vMax.x;
					vNearPoint.y = vMin.y;
					vNearPoint.z = vMin.z;
				}
				else
				{
					//���� ������ x, z ��ǥ�� ��ȣ�� ��� �����̹Ƿ� �������� x, z ��ǥ�� �ٿ�� �ڽ��� �ִ����� x, z ��ǥ
					//���� ������ y ��ǥ�� ����̹Ƿ� �������� y ��ǥ�� �ٿ�� �ڽ��� �ּ����� y ��ǥ�̴�.
					vNearPoint.x = vMax.x;
					vNearPoint.y = vMin.y;
					vNearPoint.z = vMax.z;
				}
			}
			else
			{
				if (vNormal.z >= 0.0f)
				{
					//���� ������ x, y ��ǥ�� ��ȣ�� ��� �����̹Ƿ� �������� x, y ��ǥ�� �ٿ�� �ڽ��� �ִ����� x, y ��ǥ
					//���� ������ z ��ǥ�� ����̹Ƿ� �������� z ��ǥ�� �ٿ�� �ڽ��� �ּ����� z ��ǥ�̴�.
					vNearPoint.x = vMax.x;
					vNearPoint.y = vMax.y;
					vNearPoint.z = vMin.z;
				}
				else
				{
					//���� ������ x, y, z ��ǥ�� ��ȣ�� ��� �����̹Ƿ� �������� �ٿ�� �ڽ��� �ִ����̴�.
					vNearPoint.x = vMax.x;
					vNearPoint.y = vMax.y;
					vNearPoint.z = vMax.z;
				}
			}
		}
		XMVECTOR vec1 = XMLoadFloat3( &vNormal );
		XMVECTOR vec2 = XMLoadFloat3( &vNearPoint );

		XMVECTOR vDot = XMVector3Dot( vec1, vec2 );
		float fDot = XMVectorGetX( vDot );

		if (fDot + m_FrustumPlanes[i].w > 0.0f)
			return false;
	}
	return true;
}

bool CCamera::IsInFrustum( AABB *pAABB )
{
	return IsInFrustum( pAABB->m_vMin, pAABB->m_vMax );
}

void CCamera::Rotate( float fPitch, float fYaw, float fRoll, float fTimeElapsed )
{
	XMFLOAT4X4 mtxRotate;
	
	/*���� x-���� �߽����� ȸ���ϴ� ���� ���� �յڷ� ���̴� ���ۿ� �ش��Ѵ�.
	�׷��Ƿ� x-���� �߽����� ȸ���ϴ� ������ -89.0~+89.0�� ���̷� �����Ѵ�.
	x�� ������ m_fPitch���� ���� ȸ���ϴ� �����̹Ƿ� x��ŭ ȸ���� ���� Pitch�� +89�� ���� ũ�ų� -89�� ���� ������ m_fPitch�� +89�� �Ǵ� -89���� �ǵ��� ȸ������(x)�� �����Ѵ�.*/
	if (fPitch != 0.0f)
	{
		m_fPitch += fPitch;
		if (m_fPitch > 360.0f) m_fPitch -= 360.0f;
		if (m_fPitch < 0.0f) m_fPitch += 360.0f;
	}
	// ���� y���� �߽����� ȸ���ϴ� ���� ������ ������ ���̹Ƿ� ȸ�� ������ ������ ����
	if (fYaw != 0.0f)
	{
		m_fYaw += fYaw;
		if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
		if (m_fYaw < 0.0f) m_fYaw += 360.0f;
	}
	/*���� z-���� �߽����� ȸ���ϴ� ���� ������ �¿�� ����̴� ���̹Ƿ� ȸ�� ������ -20.0~+20.0�� ���̷� ���ѵȴ�.
	z�� ������ m_fRoll���� ���� ȸ���ϴ� �����̹Ƿ� z��ŭ ȸ���� ���� m_fRoll�� +20�� ���� ũ�ų� -20������ ������ m_fRoll�� +20�� �Ǵ� -20���� �ǵ��� ȸ������(z)�� �����Ѵ�.*/
	if (fRoll != 0.0f)
	{
		m_fRoll += fRoll;
		if (m_fRoll > 360.0f) m_fRoll -= 360.0f;
		if (m_fRoll < 0.0f) m_fRoll += 360.0f;
	}

	if (fPitch != 0.0f)
	{
		XMVECTOR vRight = XMLoadFloat3( &m_vRight );
		XMMATRIX tmp = XMMatrixRotationAxis( vRight, (float)XMConvertToRadians( fPitch ) );
		XMStoreFloat4x4( &mtxRotate, tmp );

		XMMATRIX tmpMtx = XMLoadFloat4x4( &mtxRotate );
		XMVECTOR vLook = XMLoadFloat3( &m_vLook );
		vLook = XMVector3TransformNormal( vLook, tmpMtx );
		XMStoreFloat3( &m_vLook, vLook );

		XMVECTOR vUp = XMLoadFloat3( &m_vUp );
		vUp = XMVector3TransformNormal( vUp, tmpMtx );
		XMStoreFloat3( &m_vUp, vUp );
	}
	if (fYaw != 0.0f)
	{
		XMVECTOR vUp = XMLoadFloat3( &m_vUp );
		XMMATRIX tmp = XMMatrixRotationAxis( vUp, (float)XMConvertToRadians( fYaw ) );
		XMStoreFloat4x4( &mtxRotate, tmp );
			
		XMMATRIX tmpMtx = XMLoadFloat4x4( &mtxRotate );
		XMVECTOR vLook = XMLoadFloat3( &m_vLook );
		vLook = XMVector3TransformNormal( vLook, tmpMtx );
		XMStoreFloat3( &m_vLook, vLook );

		XMVECTOR vRight = XMLoadFloat3( &m_vRight );
		vRight = XMVector3TransformNormal( vRight, tmpMtx );
		XMStoreFloat3( &m_vRight, vRight );
	}
	if (fRoll != 0.0f)
	{
		XMVECTOR vLook = XMLoadFloat3( &m_vLook );
		XMMATRIX tmp = XMMatrixRotationAxis( vLook, (float)XMConvertToRadians( fRoll ) );
		XMStoreFloat4x4( &mtxRotate, tmp );

		XMMATRIX tmpMtx = XMLoadFloat4x4( &mtxRotate );
		XMVECTOR vUp = XMLoadFloat3( &m_vUp );
		vUp = XMVector3TransformNormal( vUp, tmpMtx );
		XMStoreFloat3( &m_vUp, vUp );

		XMVECTOR vRight = XMLoadFloat3( &m_vRight );
		vRight = XMVector3TransformNormal( vRight, tmpMtx );
		XMStoreFloat3( &m_vRight, vRight );
	}

	/*ȸ������ ���� �÷��̾��� ���� x-��, y-��, z-���� ���� �������� ���� �� �����Ƿ�
	z-��(LookAt ����)�� �������� �Ͽ� ���� �����ϰ� �������Ͱ� �ǵ��� �Ѵ�.*/
	// m_vLook ����ȭ
	XMVECTOR tmp = XMLoadFloat3( &m_vLook );
	tmp = XMVector3Normalize( tmp );
	XMStoreFloat3( &m_vLook, tmp );
	
	// m_vUp �� m_vLook �����Ͽ� m_vRight ����
	XMVECTOR v1 = XMLoadFloat3( &m_vUp );
	XMVECTOR v2 = XMLoadFloat3( &m_vLook );

	XMVECTOR vec = XMVector3Cross( v1, v2 );
	XMStoreFloat3( &m_vRight, vec );

	// m_vRight ����ȭ
	tmp = XMLoadFloat3( &m_vRight );
	tmp = XMVector3Normalize( tmp );
	XMStoreFloat3( &m_vRight, tmp );

	// m_vLook �� m_vRight �����Ͽ� m_vUp ����
	v1 = XMLoadFloat3( &m_vLook );
	v2 = XMLoadFloat3( &m_vRight );

	vec = XMVector3Cross( v1, v2 );
	XMStoreFloat3( &m_vUp, vec );

	// m_vUp ����ȭ
	tmp = XMLoadFloat3( &m_vUp );
	tmp = XMVector3Normalize( tmp );
	XMStoreFloat3( &m_vUp, tmp );

	RegenerateViewMatrix( );
}

void CCamera::Move( float fTimeElapsed, DWORD dwDirection )
{
	if (dwDirection)
	{
		XMVECTOR vShift = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );

		if (dwDirection & DIR_FORWARD){
			XMVECTOR vLook = XMLoadFloat3( &m_vLook );
			vLook = vLook * m_fMoveSpeed * fTimeElapsed;
			vShift = vShift + vLook;
		}
		if (dwDirection & DIR_BACKWARD){
			XMVECTOR vLook = XMLoadFloat3( &m_vLook );
			vLook = vLook * m_fMoveSpeed * fTimeElapsed;
			vShift = vShift - vLook;
		}
		if (dwDirection & DIR_RIGHT){
			XMVECTOR vRight = XMLoadFloat3( &m_vRight );
			vRight = vRight * m_fMoveSpeed * fTimeElapsed;
			vShift = vShift + vRight;
		}
		if (dwDirection & DIR_LEFT){
			XMVECTOR vRight = XMLoadFloat3( &m_vRight );
			vRight = vRight * m_fMoveSpeed * fTimeElapsed;
			vShift = vShift - vRight;
		}
		if (dwDirection & DIR_UP){
			XMVECTOR vUp = XMLoadFloat3( &m_vUp );
			vUp = vUp * m_fMoveSpeed * fTimeElapsed;
			vShift = vShift + vUp;
		}
		if (dwDirection & DIR_DOWN){
			XMVECTOR vDown = XMLoadFloat3( &m_vUp );
			vDown = vDown * m_fMoveSpeed * fTimeElapsed;
			vShift = vShift - vDown;
		}
		Move( vShift );
	}
}

void CCamera::Move( XMVECTOR& vShift )
{
	XMVECTOR pos = XMLoadFloat3( &m_vPosition );

	XMStoreFloat3( &m_vPosition, pos + vShift );

#ifdef __DEBUG_POS__
	std::cout << "Camera Postion : " << m_vPosition.x << ", " << m_vPosition.y << ", " << m_vPosition.z << std::endl;
#endif	
}

void CCamera::Update( float fTimeElapsed )
{
	// ī�޶��� ��ġ�� �ٶ󺸰� �ִ� ��ü�� ��ġ�� ��� ����
	if (m_pobjLookAtObject)
	{
		XMFLOAT3 vLookAtObjPos = m_pobjLookAtObject->GetPosition( );
		SetLookAt( vLookAtObjPos, m_vUp );
	}
	else
	{

	}
	RegenerateViewMatrix( );
}