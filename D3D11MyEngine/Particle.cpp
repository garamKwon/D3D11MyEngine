#include "stdafx.h"
#include "Particle.h"
#include <random>

CParticle::CParticle(ID3D11Device* pd3dDevice, std::string strName, int numParticles, ParticleInfo info, XMFLOAT3 vStartPos, XMFLOAT3 vDir, ObjectLayer iLayer, ObjectType iType) : CBaseObject(strName, iLayer, iType)
{
	m_nNumOfParticles = numParticles;
	for (int i = 0; i < m_nNumOfParticles; i++)
	{
		CParticleQuad* particle = new CParticleQuad(pd3dDevice, "Particles", vStartPos);
		m_svQuads.push_back(particle);
	}
		
	m_fLivingTime = 0.0f;
	m_fMaxSize = 0.0f;
	m_fRadius = 0.0f;
	m_particlesInfo = info;
	m_vStartPosition = vStartPos;
	m_vDirection = vDir;
	m_bIsUsing = false;

	SetParticlesInfo();
}

CParticle::~CParticle()
{
	for (int i = 0; i < m_svQuads.size(); i++)
	{
		SAFE_DELETE(m_svQuads[i]);
	}
}

void CParticle::SetNumOfParticles(ID3D11Device* pd3dDevice, int num)
{
	m_nNumOfParticles = num;
	if (m_nNumOfParticles > m_svQuads.size())
		for (int i = m_svQuads.size(); i < m_nNumOfParticles; i++)
			m_svQuads.push_back(new CParticleQuad(pd3dDevice, "Particles", m_vStartPosition));
	SetParticlesInfo();
}

void CParticle::SetLivingTime(float fTime)
{
	m_fLivingTime = fTime;
}

void CParticle::SetRadius(float fRadius)
{
	m_fRadius = fRadius;
	// 방사모양이 바뀜으로써 쿼드객체들의 방향을 전부 새롭게 바꿔줘야한다
	SetParticlesInfo();
}

void CParticle::SetMaxSize(float fSize)
{
	m_fMaxSize = fSize;
}

void CParticle::SetStartPosition(XMFLOAT3 vPos)
{
	for (int i = 0; i < m_nNumOfParticles; i++)
		m_svQuads[i]->SetStartPosition(vPos);
	m_vStartPosition = vPos;
}

void CParticle::SetParticlesInfo()
{
	for (int i = 0; i < m_nNumOfParticles; i++)
	{
		// 최대치를 넘겨주면 파티클들이 알아서 범위를 지정
		m_svQuads[i]->SetParticleInfo(m_particlesInfo, m_fRadius);
	}
}

void CParticle::SetMesh(CMesh* pMesh)
{
	for (int i = 0; i < m_nNumOfParticles; i++)
		m_svQuads[i]->SetMesh(pMesh);
}

void CParticle::StartParticle()
{
	if(!m_bIsUsing)
		m_bIsUsing = true;
	for (int i = 0; i < m_nNumOfParticles; i++)
		m_svQuads[i]->SetIsUsing(true);
}

void CParticle::EndParticle()
{
	m_bIsUsing = false;
	m_fCurTime = 0.0f;
	for (int i = 0; i < m_nNumOfParticles; i++)
		m_svQuads[i]->SetIsUsing(false);
}

int CParticle::GetNumOfParticles() const
{
	return m_nNumOfParticles;
}

float CParticle::GetLivingTime() const
{
	return m_fLivingTime;
}

float CParticle::GetRadius() const
{
	return m_fRadius;
}

float CParticle::GetMaxSize()const
{
	return m_fMaxSize;
}

void CParticle::Render(ID3D11DeviceContext* pd3dDeviceContext, CCamera* pCamera)
{
	for (int i = 0; i < m_nNumOfParticles; i++)
		m_svQuads[i]->Render(pd3dDeviceContext, pCamera);
}

void CParticle::Update(float fTimeElapsed)
{
	if (!m_bIsUsing)
		return;

	m_fCurTime += fTimeElapsed;

	if (m_fCurTime > m_fLivingTime && m_fLivingTime >= 0)
	{
		m_fCurTime = 0.0f;
		for (int i = 0; i < m_nNumOfParticles; i++)
			m_svQuads[i]->SetIsUsing(false);
		return;
	}
	for (int i = 0; i < m_nNumOfParticles; i++)
	{
		if (m_svQuads[i]->GetIsUsing())
			m_svQuads[i]->Update(fTimeElapsed);
	}
}

bool CParticle::GetIsUsing() const
{
	return m_bIsUsing;
}