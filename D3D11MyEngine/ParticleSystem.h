#pragma once
#include "Particle.h"

class ParticleSystem
{
	std::vector<CParticle*> m_svParticles;

public:
	ParticleSystem( );
	~ParticleSystem( );

	CParticle* AddParticle(ID3D11Device* pd3dDevice, std::string strName, CMesh* pMesh, int numParticles, ParticleInfo info, XMFLOAT3 vStartPos, XMFLOAT3 vDir, float fLivingTime, float fRadius, float fMaxSize );
	void DeleteParticle(std::string strName);

	void StartParticle(std::string strName);

	void Render(ID3D11DeviceContext* pd3dDeviceContext, CCamera* pCamera);
	void Update(float fTimeElapsed);
};

