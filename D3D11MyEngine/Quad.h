#pragma once
#include "GameObject.h"

struct ParticleInfo
{
	float m_fXSize, m_fYSize;		// 크기
	float m_fLivingTime;			// 지속시간
	float m_fGravity;				// 중력 ( 클수록 빨리 떨어짐 )
	float m_fPower;					// 날아가는 힘
	XMFLOAT3 m_vDirection;			// 날아가는 방향
};

// 4각 텍스처
class CQuad : public CBaseObject
{
protected:
	bool m_bIsBillBoard;			// 카메라를 항상 바라보는 빌보드 인지 아닌지 파악

	XMFLOAT4X4 m_mtxWorld;		// 변환 행렬
	CMaterial *m_pMaterial;		// 렌더링을 위한 머테리얼
	CMesh* m_pQuadMesh;

public:
	CQuad(ID3D11Device* pd3dDevice, std::string strName, bool isBillboard, ObjectLayer iLayer = ObjectLayer::LAYER_SCENE, ObjectType iType = ObjectType::TYPE_OBJECT);
	~CQuad();

	void SetMesh(CMesh* pMesh);
	CMesh* GetMesh() const;
	
	void SetPosition(XMFLOAT3 vPos);
	XMFLOAT3 GetPosition() const;
	void Move(XMVECTOR vShift);

	virtual void Render(ID3D11DeviceContext* pd3dDeviceContext, CCamera* pCamera);
	virtual void Update(float fTimeElapsed);
};

class CParticleQuad : public CQuad
{
	ParticleInfo m_info;
	float m_fCurPower;	// 현재 힘 정보
	bool m_bIsUsing;
	float m_fCurTime;	// 현재까지 살아온 시간
	XMFLOAT3 m_vStartPosition;
	float m_fRadius;

public:
	CParticleQuad(ID3D11Device* pd3dDevice, std::string strName, XMFLOAT3 vStartPos, ObjectLayer iLayer = ObjectLayer::LAYER_SCENE, ObjectType iType = ObjectType::TYPE_OBJECT);
	~CParticleQuad();

	void SetParticleInfo(ParticleInfo info, float radius);
	void SetSizeX(float fSizeX);
	void SetSizeY(float fSizeY);
	void SetLivingTime(float fTime);
	void SetGravity(float fg);
	void SetPower(float fPower);
	void SetDirection(XMFLOAT3 vDir);
	void SetIsUsing(bool bUse);
	void SetStartPosition(XMFLOAT3 vPos);

	ParticleInfo GetParticleInfo() const;
	float GetSizeX() const;
	float GetSizeY() const;
	float GetLivingTime() const;
	float GetGravity() const;
	float GetPower() const;
	XMFLOAT3 GetDirection() const;
	bool GetIsUsing() const;

	void Render(ID3D11DeviceContext* pd3dDeviceContext, CCamera* pCamera);
	void Update(float fTimeElapsed);
};

