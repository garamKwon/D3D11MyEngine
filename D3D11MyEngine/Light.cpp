#include "stdafx.h"
#include "Light.h"


CLight::CLight( std::string strName, ObjectLayer iLayer, ObjectType iType ) : CBaseObject( strName, iLayer, iType )
{
	m_isUse = false;
	m_vPosition = XMFLOAT3( 0, 0, 0 );
	m_vDirection = XMFLOAT3( 0, 0, 0 );
	m_cAmbient = XMFLOAT4( 0, 0, 0, 0 );
	m_cDiffuse = XMFLOAT4( 0, 0, 0, 0 );
	m_cSpecular = XMFLOAT4( 0, 0, 0, 0 );

	m_vAttenuation = XMFLOAT3(0,0,0);
	m_fRange = 0;
	m_fFalloff = 0;
	m_fPhi = 0;
	m_fTheta = 0;
}

CLight::~CLight( )
{

}

XMFLOAT4 CLight::GetAmbient( ) const
{
	return m_cAmbient;
}

XMFLOAT4 CLight::GetDiffuse( ) const
{
	return m_cDiffuse;
}

XMFLOAT4 CLight::GetSpecular( ) const
{
	return m_cSpecular;
}

XMFLOAT3 CLight::GetDirection( ) const
{
	return m_vDirection;
}

XMFLOAT3 CLight::GetPosition( ) const
{
	return m_vPosition;
}

XMFLOAT3 CLight::GetAttenuation( ) const
{
	return m_vAttenuation;
}

float CLight::GetRange( ) const
{
	return m_fRange;
}

float CLight::GetFallOff( ) const
{
	return m_fFalloff;
}

float CLight::GetPhi( ) const
{
	return m_fPhi;
}

float CLight::GetTheta( ) const
{
	return m_fTheta;
}

void CLight::SetAmbient( XMFLOAT4 param )
{
	m_cAmbient = param;
}

void CLight::SetDiffuse( XMFLOAT4 param )
{
	m_cDiffuse = param;
}

void CLight::SetSpecular( XMFLOAT4 param )
{
	m_cSpecular = param;
}

void CLight::SetDirection( XMFLOAT3 dir )
{
	m_vDirection = dir;
}

void CLight::SetPosition( XMFLOAT3 pos )
{
	m_vPosition = pos;
}

void CLight::SetRange( float param )
{
	m_fRange = param;
}

void CLight::SetAtteuation( XMFLOAT3 param )
{
	m_vAttenuation = param;
}

void CLight::SetFallOff( float param )
{
	m_fFalloff = param;
}

void CLight::SetPhi( float param )
{
	m_fPhi = param;
}

void CLight::SetTheta( float param )
{
	m_fTheta = param;
}

// directional light
CDirectionalLight::CDirectionalLight( XMFLOAT4 ambient, XMFLOAT4 diffuse, XMFLOAT4 specular, XMFLOAT3 direction, std::string strName, ObjectLayer iLayer, ObjectType iType ) : CLight( strName, iLayer, iType )
{
	m_cAmbient = ambient;
	m_cDiffuse = diffuse;
	m_cSpecular = specular;
	m_vDirection = direction;
}

CDirectionalLight::~CDirectionalLight( )
{

}


// point light
CPointLight::CPointLight( std::string strName, ObjectLayer iLayer, ObjectType iType ) : CLight( strName, iLayer, iType )
{

}
CPointLight::~CPointLight( )
{

}

// spot light
CSpotLight::CSpotLight( std::string strName, ObjectLayer iLayer, ObjectType iType ) : CLight( strName, iLayer, iType )
{

}

CSpotLight::~CSpotLight( )
{

}