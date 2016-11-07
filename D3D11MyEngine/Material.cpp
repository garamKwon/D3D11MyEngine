#include "stdafx.h"
#include "Material.h"


CMaterial::CMaterial( ID3D11Device* pd3dDevice, std::string strName, ObjectLayer iLayer, ObjectType iType) : CBaseObject(strName, iLayer, iType)
{
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dBufferDesc.ByteWidth = sizeof( MATERIAL );
	pd3dDevice->CreateBuffer( &d3dBufferDesc, NULL, &m_pd3dcbMaterial );

	m_ppTextures.resize( 0 );
	m_nTextureCount = m_ppTextures.size( );

	m_Material.m_vDiffuse = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_Material.m_vSpecular = XMFLOAT4( 1.0f, 1.0f, 1.0f, 20.0f );;
	m_Material.m_vAmbient = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_Material.m_vEmissive = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );
}

CMaterial::CMaterial( ID3D11Device* pd3dDevice, std::vector<CTexture*> ppTextures, std::string strName, ObjectLayer iLayer, ObjectType iType) : CBaseObject(strName, iLayer, iType)
{
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dBufferDesc.ByteWidth = sizeof( MATERIAL );
	pd3dDevice->CreateBuffer( &d3dBufferDesc, NULL, &m_pd3dcbMaterial );

	m_ppTextures = ppTextures;
	m_nTextureCount = m_ppTextures.size( );

	m_Material.m_vDiffuse = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_Material.m_vSpecular = XMFLOAT4( 1.0f, 1.0f, 1.0f, 20.0f );
	m_Material.m_vAmbient = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_Material.m_vEmissive = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );
}

CMaterial::CMaterial( ID3D11Device* pd3dDevice, std::vector<CTexture*> ppTextures, MATERIAL& mat, std::string strName, ObjectLayer iLayer, ObjectType iType) : CBaseObject(strName, iLayer, iType)
{
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dBufferDesc.ByteWidth = sizeof( MATERIAL );
	pd3dDevice->CreateBuffer( &d3dBufferDesc, NULL, &m_pd3dcbMaterial );

	m_ppTextures = ppTextures;
	m_nTextureCount = m_ppTextures.size( );

	m_Material = mat;
}

CMaterial::CMaterial( ID3D11Device* pd3dDevice, std::vector<CTexture*> ppTextures, XMFLOAT4& vDiffuse, XMFLOAT4& vSpecular, XMFLOAT4& m_vAmbient, XMFLOAT4& m_vEmissive, std::string strName, ObjectLayer iLayer, ObjectType iType) : CBaseObject(strName, iLayer, iType)
{
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dBufferDesc.ByteWidth = sizeof( MATERIAL );
	pd3dDevice->CreateBuffer( &d3dBufferDesc, NULL, &m_pd3dcbMaterial );

	m_ppTextures = ppTextures;
	m_nTextureCount = m_ppTextures.size();

	m_Material.m_vDiffuse = vDiffuse;
	m_Material.m_vSpecular = vSpecular;
	m_Material.m_vAmbient = m_vAmbient;
	m_Material.m_vEmissive = m_vEmissive;
}

CMaterial::CMaterial( ID3D11Device* pd3dDevice, XMFLOAT4& vDiffuse, XMFLOAT4& vSpecular, XMFLOAT4& m_vAmbient, XMFLOAT4& m_vEmissive, std::string strName, ObjectLayer iLayer, ObjectType iType) : CBaseObject(strName, iLayer, iType)
{
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dBufferDesc.ByteWidth = sizeof( MATERIAL );
	pd3dDevice->CreateBuffer( &d3dBufferDesc, NULL, &m_pd3dcbMaterial );

	m_ppTextures.resize( 0 );
	m_nTextureCount = m_ppTextures.size( );

	m_Material.m_vDiffuse = vDiffuse;
	m_Material.m_vSpecular = vSpecular;
	m_Material.m_vAmbient = m_vAmbient;
	m_Material.m_vEmissive = m_vEmissive;
}

CMaterial::CMaterial(ID3D11Device* pd3dDevice, MATERIAL& mat, std::string strName, ObjectLayer iLayer, ObjectType iType) : CBaseObject(strName, iLayer, iType)
{
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	d3dBufferDesc.ByteWidth = sizeof( MATERIAL );
	pd3dDevice->CreateBuffer( &d3dBufferDesc, NULL, &m_pd3dcbMaterial );

	m_ppTextures.resize( 0 );
	m_nTextureCount = m_ppTextures.size( );

	m_Material = mat;
}

CMaterial::~CMaterial( )
{
	if (!m_ppTextures.empty())
	{
		for (int i = 0; i < m_nTextureCount; i++)
		{
			SAFE_DELETE( m_ppTextures[i] );
		}
	}
	SAFE_RELEASE( m_pd3dcbMaterial );
}

void CMaterial::SetTextures( std::vector<CTexture*> ppTextures )
{
	if (!m_ppTextures.empty())
	{
		for (int i = 0; i < m_nTextureCount; i++)
		{
			SAFE_DELETE( m_ppTextures[i] );
		}
	}

	m_ppTextures = ppTextures;
	m_nTextureCount = m_ppTextures.size();
}

void CMaterial::setTexture( int iIndex, CTexture* pTexture )
{
	if (iIndex < m_nTextureCount)
	{
		SAFE_DELETE( m_ppTextures[iIndex] );
		m_ppTextures[iIndex] = pTexture;
	}
}

std::vector<CTexture*> CMaterial::GetTextures( ) const
{
	return m_ppTextures;
}

CTexture* CMaterial::GetTexture( int iIndex ) const
{
	if (iIndex >= m_nTextureCount)
		return nullptr;

	return m_ppTextures[iIndex];
}

CTexture* CMaterial::GetTexture(std::string strName) const
{
	for (int i = 0; i < m_nTextureCount; i++)
	{
		if (m_ppTextures[i]->GetName() == strName)
			return m_ppTextures[i];
	}
}

CTexture* CMaterial::ChangeTexture(int srcTexIdx, CTexture* dstTexture)
{
	CTexture* temp = GetTexture(srcTexIdx);
	if (temp)
	{
		m_ppTextures[srcTexIdx] = dstTexture;
		return temp;
	}

#ifdef __DEBUG_MODE__
	std::cout << "Fail to Change Texture" << std::endl;
#endif	
	return nullptr;
}

CTexture* CMaterial::ChangeTexture(std::string& strName, CTexture* dstTexture)
{
	CTexture* temp = GetTexture(strName);
	if (temp)
	{
		for (int i = 0; i < m_nTextureCount; i++)
		{
			if (m_ppTextures[i]->GetName() == strName)
			{
				m_ppTextures[i] = dstTexture;
				return temp;
			}
		}
	}

#ifdef __DEBUG_MODE__
	std::cout << "Fail to Change Texture" << std::endl;
#endif	
	return nullptr;
}

CTexture* CMaterial::ChangeTexture(ID3D11Device* pd3dDevice,  std::string& strDstName, TCHAR* strFileName, std::string& strSrcName)
{
	CTexture* temp = GetTexture(strSrcName);
	if (temp)
	{
		CTexture* tex = new CTexture(pd3dDevice, strFileName, strDstName, ObjectLayer::LAYER_SCENE);

		for (int i = 0; i < m_nTextureCount; i++)
		{
			if (m_ppTextures[i]->GetName() == strSrcName)
			{
				m_ppTextures[i] = tex;
				return temp;
			}
		}
	}
	else
	{
#ifdef __DEBUG_MODE__
		std::cout << "Fail to Change Texture" << std::endl;
#endif	
		return nullptr;
	}
	return nullptr;
}

void CMaterial::SetMaterial( MATERIAL& mat )
{
	m_Material = mat;
}

void CMaterial::SetSpecular( XMFLOAT4& vSpecular )
{
	m_Material.m_vSpecular = vSpecular;
}

void CMaterial::SetDiffuse( XMFLOAT4& vDiffuse )
{
	m_Material.m_vDiffuse = vDiffuse;
}

void CMaterial::SetAmbient( XMFLOAT4& vAmbient )
{
	m_Material.m_vAmbient = vAmbient;
}

void CMaterial::SetEmiisive( XMFLOAT4& vEmissive )
{
	m_Material.m_vEmissive = vEmissive;
}

MATERIAL CMaterial::GetMaterial( ) const
{
	return m_Material;
}

XMFLOAT4 CMaterial::GetSpecular( ) const
{
	return m_Material.m_vSpecular;
}

XMFLOAT4 CMaterial::GetDiffuse( ) const
{
	return m_Material.m_vDiffuse;
}

XMFLOAT4 CMaterial::GetAmbient( ) const
{
	return m_Material.m_vAmbient;
}

XMFLOAT4 CMaterial::GetEmissive( ) const
{
	return m_Material.m_vEmissive;
}

void CMaterial::SetcbMaterial( ID3D11DeviceContext* pd3dDeviceContext )
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map( m_pd3dcbMaterial, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource );
	MATERIAL *pcbMaterial = (MATERIAL *)d3dMappedResource.pData;
	memcpy( pcbMaterial, &m_Material, sizeof( MATERIAL ) );
	pd3dDeviceContext->Unmap( m_pd3dcbMaterial, 0 );
	pd3dDeviceContext->PSSetConstantBuffers( CMaterial::CB_MATERIAL_SLOT, 1, &m_pd3dcbMaterial );
}

void CMaterial::SetTextureOnShader( ID3D11DeviceContext* pd3dDeviceContext )
{
	ID3D11ShaderResourceView* tempView;
	ID3D11SamplerState* tempSS;

	for (int i = 0; i < m_ppTextures.size( ); i++)
	{
		tempView = m_ppTextures[i]->GetSRV( );
		tempSS = m_ppTextures[i]->GetSamplerState( );

		pd3dDeviceContext->PSSetShaderResources( i, 1, &tempView );
		pd3dDeviceContext->PSSetSamplers( i, 1, &tempSS );
	}
}

void CMaterial::SetTextureOnShader( ID3D11DeviceContext* pd3dDeviceContext, UINT idx, UINT nSlot )
{
	ID3D11ShaderResourceView* tempView = m_ppTextures[idx]->GetSRV( );;
	ID3D11SamplerState* tempSS = m_ppTextures[idx]->GetSamplerState( );

	pd3dDeviceContext->PSSetShaderResources( nSlot, 1, &tempView );
	pd3dDeviceContext->PSSetSamplers( nSlot, 1, &tempSS );
}
