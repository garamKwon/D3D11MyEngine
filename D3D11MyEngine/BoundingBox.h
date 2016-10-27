#pragma once
#include "GameObject.h"

// �� ���� �ٿ�� �ڽ�
class AABB
{
public:
	// �ٿ�� �ڽ��� �ּ����� �ִ���
	XMFLOAT3 m_vMin;
	XMFLOAT3 m_vMax;

public:
	AABB( ){ m_vMin = XMFLOAT3( +FLT_MAX, +FLT_MAX, +FLT_MAX ); m_vMax = XMFLOAT3( -FLT_MAX, -FLT_MAX, -FLT_MAX ); }
	AABB( XMFLOAT3 vMin, XMFLOAT3 vMax ) { m_vMin = vMin; m_vMax = vMax; }

	// �� ���� �ٿ�� �ڽ��� ��ħ
	void Union( XMFLOAT3& vMin, XMFLOAT3& vMax );
	void Union( AABB *pAABB );

	// �ٿ�� �ڽ��� 8���� �������� ��ķ� ��ȯ�ϰ� �ּ����� �ִ����� �ٽ� ���
	void Update( XMFLOAT4X4 *pmtxTransform );
};

class OOBB : CBaseObject
{
	XMFLOAT3 m_vCenter;
	XMFLOAT3 m_vExtent;
	XMFLOAT3 m_pvAxis[3];

	// ��ȯ ��� 
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