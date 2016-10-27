#pragma once
#include "GameObject.h"

// 축 정렬 바운딩 박스
class AABB
{
public:
	// 바운딩 박스의 최소점과 최대점
	XMFLOAT3 m_vMin;
	XMFLOAT3 m_vMax;

public:
	AABB( ){ m_vMin = XMFLOAT3( +FLT_MAX, +FLT_MAX, +FLT_MAX ); m_vMax = XMFLOAT3( -FLT_MAX, -FLT_MAX, -FLT_MAX ); }
	AABB( XMFLOAT3 vMin, XMFLOAT3 vMax ) { m_vMin = vMin; m_vMax = vMax; }

	// 두 개의 바운딩 박스를 합침
	void Union( XMFLOAT3& vMin, XMFLOAT3& vMax );
	void Union( AABB *pAABB );

	// 바운딩 박스의 8개의 꼭지점을 행렬로 변환하고 최소점과 최대점을 다시 계산
	void Update( XMFLOAT4X4 *pmtxTransform );
};

class OOBB : CBaseObject
{
	XMFLOAT3 m_vCenter;
	XMFLOAT3 m_vExtent;
	XMFLOAT3 m_pvAxis[3];

	// 변환 행렬 
	XMFLOAT4X4 m_mtxWorld;
	float m_fPitch, m_fYaw, m_fRoll;
	float m_fXScale, m_fYScale, m_fZScale;

	float MyDot(XMFLOAT3& vec1, XMFLOAT3& vec2);
public:
	OOBB();
	OOBB(XMFLOAT3& vCenter, XMFLOAT3& vExtent, XMFLOAT4X4& mtxWorld, std::string strName, ObjectLayer iLayer, ObjectType iType);
	OOBB(XMFLOAT3& vMin, XMFLOAT3& vMax, std::string strName, ObjectLayer iLayer, ObjectType iType);

	XMFLOAT3 GetMinVertex();
	XMFLOAT3 GetMaxVertex();
	XMFLOAT3 GetCenter() const;
	XMFLOAT3 GetExtent() const;
	XMFLOAT3 GetAxisX() const;
	XMFLOAT3 GetAxisY() const;
	XMFLOAT3 GetAxisZ() const;
	void GetAxis( XMFLOAT3* pAxis );

	void Update(XMFLOAT4X4 *pmtxTransform);
	bool IsCollision(const OOBB* pObject);
};