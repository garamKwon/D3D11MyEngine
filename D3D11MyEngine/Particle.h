#pragma once
#include <vector>
#include "GameObject.h"
#include "Quad.h"

// 파티클 하나, 하나의 파티클은 여러 개의 쿼드 텍스처를 가진다
class CParticle : public CBaseObject
{
	std::vector<CParticleQuad*> m_svQuads;

	int m_nNumOfParticles;	// 파티클 쿼드의 개수
	float m_fLivingTime;	// 지속시간
	float m_fCurTime;		// 현재까지 유지된 시간
	float m_fRadius;		// 방사 모양, 0이면 수직으로만 발사,1 이면 구의 형태로 발사
	float m_fMaxSize;		// 파티클의 최대 범위 ( 이 범위를 넘어서면 그냥 사라짐 )
	XMFLOAT3 m_vStartPosition; // 시작위치
	XMFLOAT3 m_vDirection;	// 파티클이 퍼져나갈 중심 방향

	bool m_bIsUsing;		// 사용하는지 여부
	ParticleInfo m_particlesInfo;	// 파티클들의 최대치 정보

public:
	CParticle(ID3D11Device* pd3dDevice, std::string strName, int numParticles, ParticleInfo info, XMFLOAT3 vStartPos, XMFLOAT3 vDir, ObjectLayer iLayer = ObjectLayer::LAYER_SCENE, ObjectType iType = ObjectType::TYPE_PARTICLE);
	~CParticle();

	void SetNumOfParticles(ID3D11Device* pd3dDevice, int num);
	void SetLivingTime(float fTime);
	void SetRadius(float fRadius);
	void SetMaxSize(float fSize);
	void SetStartPosition(XMFLOAT3 vPos);
	void SetParticlesInfo();
	void StartParticle();
	void EndParticle();
	void SetMesh(CMesh* pMesh);

	int GetNumOfParticles() const;
	float GetLivingTime() const;
	float GetRadius() const;
	float GetMaxSize()const;
	bool GetIsUsing() const;

	void Render(ID3D11DeviceContext* pd3dDeviceContext, CCamera* pCamera);
	void Update(float fTimeElapsed);
};

