#include "stdafx.h"
#include "BoundingBox.h"
#include <math.h>


void AABB::Union( XMFLOAT3& vMin, XMFLOAT3& vMax )
{
	if (vMin.x < m_vMin.x) m_vMin.x = vMin.x;
	if (vMin.y < m_vMin.y) m_vMin.y = vMin.y;
	if (vMin.z < m_vMin.z) m_vMin.z = vMin.z;
	if (vMax.x > m_vMax.x) m_vMax.x = vMax.x;
	if (vMax.y > m_vMax.y) m_vMax.y = vMax.y;
	if (vMax.z > m_vMax.z) m_vMax.z = vMax.z;
}

void AABB::Union( AABB *pAABB )
{
	Union( pAABB->m_vMin, pAABB->m_vMax );
}

void AABB::Update( XMFLOAT4X4 *pmtxTransform )
{
	XMFLOAT3 vVertices[8];
	vVertices[0] = XMFLOAT3( m_vMin.x, m_vMin.y, m_vMin.z );
	vVertices[1] = XMFLOAT3( m_vMin.x, m_vMin.y, m_vMax.z );
	vVertices[2] = XMFLOAT3( m_vMax.x, m_vMin.y, m_vMax.z );
	vVertices[3] = XMFLOAT3( m_vMax.x, m_vMin.y, m_vMin.z );
	vVertices[4] = XMFLOAT3( m_vMin.x, m_vMax.y, m_vMin.z );
	vVertices[5] = XMFLOAT3( m_vMin.x, m_vMax.y, m_vMax.z );
	vVertices[6] = XMFLOAT3( m_vMax.x, m_vMax.y, m_vMax.z );
	vVertices[7] = XMFLOAT3( m_vMax.x, m_vMax.y, m_vMin.z );
	m_vMin = XMFLOAT3( +100000, +100000, +100000 );
	m_vMax = XMFLOAT3( -100000, -100000, -100000 );

	// 8개의 정점에서 x,y,z 좌표의 최소값 최대값을 구한다.
	for (int i = 0; i < 8; i++)
	{
		XMMATRIX tmpMtx = XMLoadFloat4x4( pmtxTransform );
		XMVECTOR tmpVec = XMLoadFloat3( &(vVertices[i]) );
		XMStoreFloat3( &( vVertices[i] ), XMVector3TransformCoord( tmpVec, tmpMtx ) );

		if (vVertices[i].x < m_vMin.x) m_vMin.x = vVertices[i].x;
		if (vVertices[i].y < m_vMin.y) m_vMin.y = vVertices[i].y;
		if (vVertices[i].z < m_vMin.z) m_vMin.z = vVertices[i].z;
		if (vVertices[i].x > m_vMax.x) m_vMax.x = vVertices[i].x;
		if (vVertices[i].y > m_vMax.y) m_vMax.y = vVertices[i].y;
		if (vVertices[i].z > m_vMax.z) m_vMax.z = vVertices[i].z;
	}
}

OOBB::OOBB() : CBaseObject( "OOBB", ObjectLayer::LAYER_SCENE, ObjectType::TYPE_TRIGGER)
{
	m_vCenter = XMFLOAT3(0, 0, 0);
	m_vExtent = XMFLOAT3(+FLT_MAX, +FLT_MAX, +FLT_MAX);

	XMStoreFloat4x4(&m_mtxWorld, XMMatrixIdentity());
}

OOBB::OOBB(XMFLOAT3& vCenter, XMFLOAT3& vExtent, XMFLOAT4X4& mtxWorld, std::string strName, ObjectLayer iLayer, ObjectType iType) : CBaseObject(strName, iLayer, iType)
{
	m_vCenter = vCenter;
	m_vExtent = vExtent;
	m_mtxWorld = mtxWorld;
}

OOBB::OOBB(XMFLOAT3& vMin, XMFLOAT3& vMax, std::string strName, ObjectLayer iLayer, ObjectType iType) : CBaseObject(strName, iLayer, iType)
{
	XMVECTOR min = XMLoadFloat3(&vMin);
	XMVECTOR max = XMLoadFloat3(&vMax);

	XMVECTOR center = (min + max) / 2;
	XMVECTOR extent = max - center;

	XMStoreFloat3(&m_vCenter, center);
	XMStoreFloat3(&m_vExtent, extent);

	XMStoreFloat4x4(&m_mtxWorld, XMMatrixIdentity());
}

XMFLOAT3 OOBB::GetMinVertex()
{
	return XMFLOAT3(m_vCenter.x - m_vExtent.x, m_vCenter.y - m_vExtent.y, m_vCenter.z - m_vExtent.z);
}

XMFLOAT3 OOBB::GetMaxVertex()
{
	return XMFLOAT3(m_vCenter.x + m_vExtent.x, m_vCenter.y + m_vExtent.y, m_vCenter.z + m_vExtent.z);
}

XMFLOAT3 OOBB::GetCenter() const
{
	return m_vCenter;
}

XMFLOAT3 OOBB::GetExtent() const
{
	return m_vExtent;
}
XMFLOAT3 OOBB::GetAxisX() const
{
	return m_pvAxis[0];
}

XMFLOAT3 OOBB::GetAxisY() const
{
	return m_pvAxis[1];
}

XMFLOAT3 OOBB::GetAxisZ() const
{
	return m_pvAxis[2];
}

void OOBB::GetAxis(XMFLOAT3* pAxis)
{
	pAxis = m_pvAxis;
}

void OOBB::Update(XMFLOAT4X4 *pmtxTransform)
{
	// 변환행렬 변환
	XMMATRIX world = XMLoadFloat4x4(&m_mtxWorld);
	XMMATRIX transform = XMLoadFloat4x4(pmtxTransform);

	world = XMMatrixMultiply(world, transform);
	XMStoreFloat4x4(&m_mtxWorld, world);

	// 축 계산하기
	XMFLOAT3 vRight(m_mtxWorld._11, m_mtxWorld._12, m_mtxWorld._13);
	XMVECTOR vTemp = XMLoadFloat3(&vRight);
	XMVECTOR vNormalize = XMVector3Normalize(vTemp);
	XMStoreFloat3(&m_pvAxis[0], vNormalize);

	XMFLOAT3 vUp(m_mtxWorld._21, m_mtxWorld._22, m_mtxWorld._23);
	vTemp = XMLoadFloat3(&vUp);
	vNormalize = XMVector3Normalize(vTemp);
	XMStoreFloat3(&m_pvAxis[1], vNormalize);

	XMFLOAT3 vLookAt(m_mtxWorld._31, m_mtxWorld._32, m_mtxWorld._33);
	vTemp = XMLoadFloat3(&vLookAt);
	vNormalize = XMVector3Normalize(vTemp);
	XMStoreFloat3(&m_pvAxis[2], vNormalize);

	// 센터 위치 변환
	XMVECTOR center = XMLoadFloat3(&m_vCenter);
	XMVector3TransformCoord(center, world);
	XMStoreFloat3(&m_vCenter, center);
}

bool OOBB::IsCollision(const OOBB* pObject)
{
	XMFLOAT3 myExtent = GetExtent();
	XMFLOAT3 dstExtent = pObject->GetExtent();
	XMFLOAT3 D(m_vCenter.x - pObject->GetCenter().x, m_vCenter.y - pObject->GetCenter().y, m_vCenter.z - pObject->GetCenter().z );

	XMFLOAT3 C[3];
	XMFLOAT3 absC[3];
	XMFLOAT3 AD;
	float R0, R1, R;
	float R01;

	// A0
	C[0].x = MyDot(GetAxisX(), pObject->GetAxisX());
	C[0].y = MyDot(GetAxisX(), pObject->GetAxisY());
	C[0].z = MyDot(GetAxisX(), pObject->GetAxisZ());
	AD.x = MyDot(GetAxisX(), D);
	absC[0].x = fabsf(C[0].x);
	absC[0].y = fabsf(C[0].y);
	absC[0].z = fabsf(C[0].z);
	R = fabsf(AD.x);
	R1 = dstExtent.x * absC[0].x + dstExtent.y * absC[0].y + dstExtent.z * absC[0].z;
	R01 = myExtent.x + R1;
	if (R > R01) return false;

	// A1
	C[1].x = MyDot(GetAxisY(), pObject->GetAxisX());
	C[1].y = MyDot(GetAxisY(), pObject->GetAxisY());
	C[1].z = MyDot(GetAxisY(), pObject->GetAxisZ());
	AD.y = MyDot(GetAxisY(), D);
	absC[1].x = fabsf(C[1].x);
	absC[1].y = fabsf(C[1].y);
	absC[1].z = fabsf(C[1].z);
	R = fabsf(AD.y);
	R1 = dstExtent.x * absC[1].x + dstExtent.y * absC[1].y + dstExtent.z * absC[1].z;
	R01 = myExtent.y + R1;
	if (R > R01) return false;

	// A2
	C[2].x = MyDot(GetAxisZ(), pObject->GetAxisX());
	C[2].y = MyDot(GetAxisZ(), pObject->GetAxisY());
	C[2].z = MyDot(GetAxisZ(), pObject->GetAxisZ());
	AD.z = MyDot(GetAxisZ(), D);
	absC[2].x = fabsf(C[2].x);
	absC[2].y = fabsf(C[2].y);
	absC[2].z = fabsf(C[2].z);
	R = fabsf(AD.z);
	R1 = dstExtent.x * absC[2].x + dstExtent.y * absC[2].y + dstExtent.z * absC[2].z;
	R01 = myExtent.z + R1;
	if (R > R01) return false;

	// B0
	R = fabsf(MyDot(pObject->GetAxisX(), D));
	R0 = myExtent.x * absC[0].x + myExtent.y * absC[1].x + myExtent.z * absC[2].x;
	R01 = R0 + dstExtent.x;
	if (R > R01) return false;

	// B1
	R = fabsf(MyDot(pObject->GetAxisY(), D));
	R0 = myExtent.x * absC[0].y + myExtent.y * absC[1].y + myExtent.z * absC[2].y;
	R01 = R0 + dstExtent.x;
	if (R > R01) return false;

	// B2
	R = fabsf(MyDot(pObject->GetAxisZ(), D));
	R0 = myExtent.x * absC[0].z + myExtent.z * absC[1].y + myExtent.z * absC[2].z;
	R01 = R0 + dstExtent.x;
	if (R > R01) return false;

	// A0XB0
	R = fabsf(AD.z*C[1].x - AD.y*C[2].x);
	R0 = myExtent.y * absC[2].x + myExtent.z * absC[1].x;
	R1 = dstExtent.y * absC[0].z + dstExtent.z * absC[0].y;
	R01 = R0 + R1;
	if (R > R01) return false;

	// A0XB1
	R = fabsf(AD.z*C[1].y - AD.y*C[2].y);
	R0 = myExtent.y * absC[2].y + myExtent.z * absC[1].y;
	R1 = dstExtent.x * absC[0].z + dstExtent.z * absC[0].x;
	R01 = R0 + R1;
	if (R > R01) return false;

	// A0XB2
	R = fabsf(AD.z*C[1].y - AD.y*C[2].z);
	R0 = myExtent.y * absC[2].z + myExtent.z * absC[1].z;
	R1 = dstExtent.x * absC[0].y + dstExtent.y * absC[0].x;
	R01 = R0 + R1;
	if (R > R01) return false;

	// A1XB0
	R = fabsf(AD.x*C[2].x - AD.z*C[0].x);
	R0 = myExtent.x * absC[2].x + myExtent.z * absC[0].x;
	R1 = dstExtent.y * absC[1].z + dstExtent.z * absC[1].y;
	R01 = R0 + R1;
	if (R > R01) return false;

	// A1XB1
	R = fabsf(AD.x*C[2].y - AD.z*C[0].y);
	R0 = myExtent.x * absC[2].y + myExtent.z * absC[0].y;
	R1 = dstExtent.x * absC[1].z + dstExtent.z * absC[1].x;
	R01 = R0 + R1;
	if (R > R01) return false;

	// A1XB2
	R = fabsf(AD.x*C[2].z - AD.z*C[0].z);
	R0 = myExtent.x * absC[2].z + myExtent.z * absC[0].z;
	R1 = dstExtent.x * absC[1].y + dstExtent.y * absC[1].x;
	R01 = R0 + R1;
	if (R > R01) return false;

	// A2XB0
	R = fabsf(AD.y*C[0].x - AD.x*C[1].x);
	R0 = myExtent.x * absC[1].x + myExtent.y * absC[0].x;
	R1 = dstExtent.y * absC[2].y + dstExtent.z * absC[2].y;
	R01 = R0 + R1;
	if (R > R01) return false;

	// A2XB1
	R = fabsf(AD.y*C[0].y - AD.x*C[1].y);
	R0 = myExtent.x * absC[1].y + myExtent.y * absC[0].y;
	R1 = dstExtent.x * absC[2].z + dstExtent.z * absC[2].x;
	R01 = R0 + R1;
	if (R > R01) return false;

	// A2XB2
	R = fabsf(AD.y*C[0].z - AD.x*C[1].z);
	R0 = myExtent.x * absC[1].z + myExtent.y * absC[0].z;
	R1 = dstExtent.x * absC[2].y + dstExtent.y * absC[2].x;
	R01 = R0 + R1;
	if (R > R01) return false;

	// 충돌
#ifdef __DEBUG_MODE__
	std::cout << m_strName.c_str() << "과(와) " << pObject->GetName().c_str() << " 충돌" << std::endl;
#endif
	return true;
}

float OOBB::MyDot(XMFLOAT3& vec1, XMFLOAT3& vec2)
{
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.x * vec2.y;
}
