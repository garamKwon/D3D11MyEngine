#pragma once
#include "GameObject.h"

#define MAX_LIGHTS 8
#define DIRECTIONAL_LIGHT 0.0f
#define POINT_LIGHT 1.0f
#define SPOT_LIGHT 2.0f

// 하나의 조명 구조체, 16바이트를 맞추기 위한 구조체
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

// 상수버퍼는 크기가 반드시 16바이트의 배수여야 한다.
struct LIGHTS
{
	LIGHT m_pLights[MAX_LIGHTS];
	XMFLOAT4 m_cGlobalAmbient;
	XMFLOAT4 m_vCameraPosition;

	double m_FogStart;
	double m_FogRange;	
	XMFLOAT4 m_FogColor;
};

// 조명의 추상클래스, 디렉셔널, 포인트, 스포트 등의 자식 클래스를 만들 예정
class CLight : public CBaseObject
{
protected:
	bool m_isUse;		// 조명을 여러개를 한번에 파이프라인에 연결하는데 사용하는지 여부 판정

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
	// ambient, diffuse, specular, direction 만 필요
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