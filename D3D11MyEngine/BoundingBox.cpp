#include "stdafx.h"
#include "BoundingBox.h"


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