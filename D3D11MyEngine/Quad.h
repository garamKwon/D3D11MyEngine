#pragma once
#include "GameObject.h"

struct ParticleInfo
{
	float m_fXSize, m_fYSize;		// ũ��
	float m_fLivingTime;			// ���ӽð�
	float m_fGravity;				// �߷� ( Ŭ���� ���� ������ )
	float m_fPower;					// ���ư��� ��
	XMFLOAT3 m_vDirection;			// ���ư��� ����
};

// 4�� �ؽ�ó
class CQuad : public CBaseObject
{
protected:
	bool m_bIsBillBoard;			// ī�޶� �׻� �ٶ󺸴� ������ ���� �ƴ��� �ľ�

	XMFLOAT4X4 m_mtxWorld;		// ��ȯ ���
	CMaterial *m_pMaterial;		// �������� ���� ���׸���
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
	float m_fCurPower;	// ���� �� ����
	bool m_bIsUsing;
	float m_fCurTime;	// ������� ��ƿ� �ð�
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

