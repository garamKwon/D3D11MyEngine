#include "stdafx.h"
#include "Texture.h"


CTexture::CTexture( ID3D11Device* pd3dDevice, TCHAR* strFileName, std::string strName, ObjectLayer iLayer, ObjectType iType ) : CBaseObject(strName, iLayer, iType )
{
	ID3D11ShaderResourceView *pd3dsrvTexture = NULL;
	D3DX11CreateShaderResourceViewFromFile( pd3dDevice, strFileName, NULL, NULL, &pd3dsrvTexture, NULL );

	ID3D11SamplerState *pd3dSamplerState = NULL;
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	::ZeroMemory( &d3dSamplerDesc, sizeof( D3D11_SAMPLER_DESC ) );
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = 0;
	pd3dDevice->CreateSamplerState( &d3dSamplerDesc, &pd3dSamplerState );

	m_pd3dSamplerStates = pd3dSamplerState;
	m_pd3dTextureSRV = pd3dsrvTexture;
	m_strFileName = TCHARToString( strFileName );
}


CTexture::~CTexture( )
{
	SAFE_RELEASE( m_pd3dSamplerStates );
	SAFE_RELEASE( m_pd3dTextureSRV );
}

std::string CTexture::GetName( ) const
{
	return CBaseObject::GetName();
}

std::string CTexture::GetFileName( ) const
{
	return m_strFileName;
}

ID3D11ShaderResourceView* CTexture::GetSRV( )
{
	return m_pd3dTextureSRV;
}

ID3D11SamplerState* CTexture::GetSamplerState( )
{
	return m_pd3dSamplerStates;
}