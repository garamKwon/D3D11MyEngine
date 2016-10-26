#pragma once
#include "GameObject.h"

#define MAX_LIGHTS 8
#define DIRECTIONAL_LIGHT 0.0f
#define POINT_LIGHT 1.0f
#define SPOT_LIGHT 2.0f

// �ϳ��� ���� ����ü, 16����Ʈ�� ���߱� ���� ����ü
struct LIGHT
{
	XMFLOAT4 m_cAmbient;
	XMFLOAT4 m_cDiffuse;
	XMFLOAT4 m_cSpecular;
	XMFLOAT3 m_vPosition;
	float m_fRange;

	XMFLOAT3 m_vDirection;
	float m_nType;
	XMFLOAT3 m_vAttenuation;
	float m_fFalloff;
	float m_fTheta;
	float m_fPhi;
	float m_bEnable = false;
	float padding;
};

// ������۴� ũ�Ⱑ �ݵ�� 16����Ʈ�� ������� �Ѵ�.
struct LIGHTS
{
	LIGHT m_pLights[MAX_LIGHTS];
	XMFLOAT4 m_cGlobalAmbient;
	XMFLOAT4 m_vCameraPosition;

	double m_FogStart;
	double m_FogRange;	
	XMFLOAT4 m_FogColor;
};

// ������ �߻�Ŭ����, �𷺼ų�, ����Ʈ, ����Ʈ ���� �ڽ� Ŭ������ ���� ����
class CLight : public CBaseObject
{
protected:
	bool m_isUse;		// ������ �������� �ѹ��� ���������ο� �����ϴµ� ����ϴ��� ���� ����

	XMFLOAT3 m_vPosition;
	XMFLOAT3 m_vDirection;

	XMFLOAT4 m_cAmbient;
	XMFLOAT4 m_cDiffuse;
	XMFLOAT4 m_cSpecular;

	XMFLOAT3 m_vAttenuation;
	float m_fRange;
	float m_fFalloff;
	float m_fPhi;
	float m_fTheta;

public:
	static UINT CB_LIGHT_SLOT;

public:
	CLight( std::string strName, ObjectLayer iLayer, ObjectType iType );
	virtual ~CLight( ) = 0;

	XMFLOAT4 GetAmbient( ) const;
	XMFLOAT4 GetDiffuse( ) const;
	XMFLOAT4 GetSpecular( ) const;
	XMFLOAT3 GetDirection( ) const;
	XMFLOAT3 GetPosition( ) const;
	XMFLOAT3 GetAttenuation( ) const;
	float GetRange( ) const;
	float GetFallOff( ) const;
	float GetPhi( ) const;
	float GetTheta( ) const;

	void SetAmbient( XMFLOAT4 param );
	void SetDiffuse( XMFLOAT4 param );
	void SetSpecular( XMFLOAT4 param );
	void SetDirection( XMFLOAT3 dir );
	void SetPosition( XMFLOAT3 pos );
	void SetRange( float param );
	void SetAtteuation( XMFLOAT3 param );
	void SetFallOff( float param );
	void SetPhi( float param );
	void SetTheta( float param );
};

class CDirectionalLight : public CLight
{
	// ambient, diffuse, specular, direction �� �ʿ�
public:
	CDirectionalLight( XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular, XMFLOAT3 direction, std::string strName, ObjectLayer iLayer, ObjectType iType );
	~CDirectionalLight( );
};

class CPointLight : public CLight
{
public:
	CPointLight( std::string strName, ObjectLayer iLayer, ObjectType iType );
	~CPointLight( );
};

class CSpotLight : public CLight
{
public:
	CSpotLight( std::string strName, ObjectLayer iLayer, ObjectType iType );
	~CSpotLight( );
};