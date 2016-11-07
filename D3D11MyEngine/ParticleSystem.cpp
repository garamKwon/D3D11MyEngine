#include "stdafx.h"
#include "ParticleSystem.h"


ParticleSystem::ParticleSystem( )
{
}


ParticleSystem::~ParticleSystem( )
{
	for (int i = 0; i < m_svParticles.size(); i++)
	{
		SAFE_DELETE(m_svParticles[i]);
	}
}

CParticle* ParticleSystem::AddParticle(ID3D11Device* pd3dDevice, std::string strName, CMesh* pMesh, int numParticles, ParticleInfo info, XMFLOAT3 vStartPos, XMFLOAT3 vDir, float fLivingTime, float fRadius, float fMaxSize)
{
	CParticle* pParticle = new CParticle(pd3dDevice, strName, numParticles, info, vStartPos, vDir);
	pParticle->SetLivingTime(fLivingTime);
	pParticle->SetRadius(fRadius);
	pParticle->SetMaxSize(fMaxSize);
	pParticle->SetMesh(pMesh);

	m_svParticles.push_back(pParticle);
	return pParticle;
}

void ParticleSystem::DeleteParticle(std::string strName)
{
	for (int i = 0; i < m_svParticles.size(); i++)
	{
		if (m_svParticles[i]->GetName() == strName)
			SAFE_DELETE(m_svParticles[i]);
	}
}

void ParticleSystem::StartParticle(std::string strName)
{
	for (int i = 0; i < m_svParticles.size(); i++)
	{
		if (m_svParticles[i]->GetName() == strName)
		{
			if (!m_svParticles[i]->GetIsUsing())
			{
				m_svParticles[i]->StartParticle();
				break;
			}
		}
	}
}

void ParticleSystem::Render(ID3D11DeviceContext* pd3dDeviceContext, CCamera* pCamera)
{
	for (int i = 0; i < m_svParticles.size(); i++)
	{
		m_svParticles[i]->Render(pd3dDeviceContext, pCamera);
	}
}

void ParticleSystem::Update(float fTimeElapsed)
{
	for (int i = 0; i < m_svParticles.size(); i++)
	{
		m_svParticles[i]->Update(fTimeElapsed);
	}
}