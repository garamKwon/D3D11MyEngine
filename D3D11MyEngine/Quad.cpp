#include "stdafx.h"
#include "Quad.h"
#include "Material.h"
#include "BoundingBox.h"
#include "Mesh.h"

CQuad::CQuad(ID3D11Device* pd3dDevice, std::string strName, bool isBillboard, ObjectLayer iLayer, ObjectType iType) : CBaseObject(strName, iLayer, iType)
{
	m_bIsBillBoard = isBillboard;
	XMStoreFloat4x4(&m_mtxWorld, XMMatrixIdentity());

	std::vector<CTexture*> textures;

	CTexture* tex = new CTexture(pd3dDevice, L"baseTexture.png", "DiffuseTexture", iLayer);
	textures.push_back(tex);

	m_pMaterial = new CMaterial(pd3dDevice, textures, "QuadMaterial", ObjectLayer::LAYER_SCENE, ObjectType::TYPE_MATERIAL);
}


CQuad::~CQuad()
{
	SAFE_DELETE(m_pMaterial);
	m_pQuadMesh->MinusRef();
}

void CQuad::SetMesh(CMesh* pMesh)
{
	pMesh->PlusRef();
	m_pQuadMesh = pMesh;
}

CMesh* CQuad::GetMesh() const
{
	return m_pQuadMesh;
}

void CQuad::SetPosition(XMFLOAT3 vPos)
{
	m_mtxWorld._41 = vPos.x;
	m_mtxWorld._42 = vPos.y;
	m_mtxWorld._43 = vPos.z;
}

XMFLOAT3 CQuad::GetPosition() const
{
	return XMFLOAT3(m_mtxWorld._41, m_mtxWorld._42, m_mtxWorld._43);
}

void CQuad::Move(XMVECTOR vShift)
{
	XMVECTOR pos = XMLoadFloat3(&GetPosition());
	XMFLOAT3 vPos;
	XMStoreFloat3(&vPos, pos + vShift);
	SetPosition(vPos);

#ifdef __DEBUG_POS__
	std::cout << "Object Postion : " << vPos.x << ", " << vPos.y << ", " << vPos.z << std::endl;
#endif	
}

void CQuad::Render(ID3D11DeviceContext* pd3dDeviceContext, CCamera* pCamera)
{
	if (!pCamera)
		return;

	if (m_pQuadMesh)
	{
		OOBB bcBoundingCube = m_pQuadMesh->GetBoundingBox();
		bcBoundingCube.Update(&m_mtxWorld);

		if (pCamera->IsInFrustum(bcBoundingCube.GetMinVertex(), bcBoundingCube.GetMaxVertex()))	// �Ⱥ��̸� �ȱ׸�
		{
			if (m_pMaterial)
			{
				m_pMaterial->SetcbMaterial(pd3dDeviceContext);
				m_pMaterial->SetTextureOnShader(pd3dDeviceContext);
			}
			m_pQuadMesh->SetIsVisible(true);
			m_pQuadMesh->GetShader()->SetVertexShaderContantBuffers(pd3dDeviceContext, &m_mtxWorld, pCamera);
			m_pQuadMesh->Render(pd3dDeviceContext);
		}
		else
			m_pQuadMesh->SetIsVisible(false);
	}
}

void CQuad::Update(float fTimeElapased)
{

}

CParticleQuad::CParticleQuad(ID3D11Device* pd3dDevice, std::string strName, XMFLOAT3 vStartPos, ObjectLayer iLayer, ObjectType iType) : CQuad(pd3dDevice, strName, true, iLayer, iType)
{
	m_bIsUsing = false;
	m_vStartPosition = vStartPos;
	m_fCurTime = 0.0f;
}
CParticleQuad::~CParticleQuad()
{
}

void CParticleQuad::SetParticleInfo(ParticleInfo info, float radius)
{
	std::random_device gen;
	std::uniform_real_distribution<float> liveTime(info.m_fLivingTime - info.m_fLivingTime*0.4, info.m_fLivingTime);
	std::uniform_real_distribution<float> power(info.m_fPower - info.m_fPower*0.8, info.m_fPower);
	std::uniform_real_distribution<float> Xsize(info.m_fXSize - info.m_fXSize*0.4, info.m_fXSize);
	std::uniform_real_distribution<float> Ysize(info.m_fYSize - info.m_fYSize*0.4, info.m_fYSize);

	m_info.m_fGravity = info.m_fGravity;
		// �Ʒ� �������� ���� ������� �ִ�ġ�� ���� �ʴ� �������� ����,
		// �ִ�ġ�� 20�ۼ�Ʈ ���� �ȿ��� ����ǵ��� ����
	float randNum = liveTime(gen);
	m_info.m_fLivingTime = randNum;

	randNum = power(gen);
	m_info.m_fPower = randNum;

	randNum = Xsize(gen);
	m_info.m_fXSize = randNum;

	randNum = Ysize(gen);
	m_info.m_fYSize = randNum;

	// ������ Radius�� ������ �޴´�
	std::uniform_real_distribution<float> dir(info.m_fPower - info.m_fPower*0.2, info.m_fPower);
	float x = dir(gen);
	float y = dir(gen);
	float z = dir(gen);
	m_info.m_vDirection = XMFLOAT3(info.m_vDirection.x - x, info.m_vDirection.y + y, info.m_vDirection.z - z);		// m_vDirection�� ������� ���� ����

	XMVECTOR nor = XMLoadFloat3(&m_info.m_vDirection);
	nor = XMVector3Normalize(nor);
	XMStoreFloat3(&m_info.m_vDirection, nor);

	m_fCurPower = m_info.m_fPower;
}

void CParticleQuad::SetSizeX(float fSizeX)
{
	m_info.m_fXSize = fSizeX;
}

void CParticleQuad::SetSizeY(float fSizeY)
{
	m_info.m_fYSize = fSizeY;
}

void CParticleQuad::SetLivingTime(float fTime)
{
	m_info.m_fLivingTime = fTime;
}

void CParticleQuad::SetGravity(float fg)
{
	m_info.m_fGravity = fg;
}

void CParticleQuad::SetPower(float fPower)
{
	m_info.m_fPower = fPower;
	m_fCurPower = fPower;
}

void CParticleQuad::SetDirection(XMFLOAT3 vDir)
{
	m_info.m_vDirection = vDir;
}

void CParticleQuad::SetIsUsing(bool bUse)
{
	m_bIsUsing = bUse;
}

void CParticleQuad::SetStartPosition(XMFLOAT3 vPos)
{
	m_vStartPosition = vPos;
}

ParticleInfo CParticleQuad::GetParticleInfo() const
{
	return m_info;
}

float CParticleQuad::GetSizeX() const
{
	return m_info.m_fXSize;
}

float CParticleQuad::GetSizeY() const
{
	return m_info.m_fYSize;
}

float CParticleQuad::GetLivingTime() const
{
	return m_info.m_fLivingTime;
}

float CParticleQuad::GetGravity() const
{
	return m_info.m_fGravity;
}

float CParticleQuad::GetPower() const
{
	return m_info.m_fPower;
}

XMFLOAT3 CParticleQuad::GetDirection() const
{
	return m_info.m_vDirection;
}

bool CParticleQuad::GetIsUsing() const
{
	return m_bIsUsing;
}

void CParticleQuad::Render(ID3D11DeviceContext* pd3dDeviceContext, CCamera* pCamera)
{
	if (!pCamera || !m_bIsUsing)
		return;

	if (m_pQuadMesh)
	{
		// ����޽ð� ī�޶� �ٶ󺸵��� �Ұ�

		OOBB bcBoundingCube = m_pQuadMesh->GetBoundingBox();
		bcBoundingCube.Update(&m_mtxWorld);

		if (pCamera->IsInFrustum(bcBoundingCube.GetMinVertex(), bcBoundingCube.GetMaxVertex()))	// �Ⱥ��̸� �ȱ׸�
		{
			if (m_pMaterial)
			{
				m_pMaterial->SetcbMaterial(pd3dDeviceContext);
				m_pMaterial->SetTextureOnShader(pd3dDeviceContext);
			}
			m_pQuadMesh->SetIsVisible(true);
			m_pQuadMesh->GetShader()->SetVertexShaderContantBuffers(pd3dDeviceContext, &m_mtxWorld, pCamera);
			m_pQuadMesh->Render(pd3dDeviceContext);
		}
		else
			m_pQuadMesh->SetIsVisible(false);
	}

}

void CParticleQuad::Update(float fTimeElapsed)
{
	if (!m_bIsUsing)
	{
		// ��������� ������ �����
		m_bIsUsing = true;
		SetPosition(m_vStartPosition);
		m_fCurTime = 0.0f;
		m_fCurPower = m_info.m_fPower;
	}
		
	m_fCurTime = fTimeElapsed;

	// ���� ���� �ð��� �ִ� ���� �ð����� ��� ����
	if (m_fCurTime >= m_info.m_fLivingTime)
	{
		m_bIsUsing = false;
		return;
	}

	// ���� �̵�
	XMVECTOR dir = XMLoadFloat3(&m_info.m_vDirection);
	XMVECTOR downVector = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	
	// �̵��Ÿ� =  ���ư��� ���� * �� * �ҿ�ð�
	XMVECTOR vShift = dir * m_fCurPower * fTimeElapsed;
	// �̵��Ÿ� = �̵��Ÿ� + �Ʒ����� * �߷��� * �ҿ�ð�
	vShift = vShift + downVector * m_info.m_fGravity * fTimeElapsed;

	// �̵�
	Move(vShift);

	// �� ����
	if (m_fCurPower > 0)
		m_fCurPower -= m_fCurPower * fTimeElapsed;
	else
		m_fCurPower = 0.0f;

	m_pQuadMesh->Update(fTimeElapsed);
}