#pragma once
#include <vector>
#include "GameObject.h"
#include "Quad.h"

// ��ƼŬ �ϳ�, �ϳ��� ��ƼŬ�� ���� ���� ���� �ؽ�ó�� ������
class CParticle : public CBaseObject
{
	std::vector<CParticleQuad*> m_svQuads;

	int m_nNumOfParticles;	// ��ƼŬ ������ ����
	float m_fLivingTime;	// ���ӽð�
	float m_fCurTime;		// ������� ������ �ð�
	float m_fRadius;		// ��� ���, 0�̸� �������θ� �߻�,1 �̸� ���� ���·� �߻�
	float m_fMaxSize;		// ��ƼŬ�� �ִ� ���� ( �� ������ �Ѿ�� �׳� ����� )
	XMFLOAT3 m_vStartPosition; // ������ġ
	XMFLOAT3 m_vDirection;	// ��ƼŬ�� �������� �߽� ����

	bool m_bIsUsing;		// ����ϴ��� ����
	ParticleInfo m_particlesInfo;	// ��ƼŬ���� �ִ�ġ ����

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

