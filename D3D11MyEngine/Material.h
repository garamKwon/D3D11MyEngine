#pragma once
#include "Texture.h"

struct MATERIAL
{
	XMFLOAT4 m_vAmbient;
	XMFLOAT4 m_vDiffuse;
	XMFLOAT4 m_vSpecular;
	XMFLOAT4 m_vEmissive;
};

// ������ �ؽ�ó�� ������ ������ ���� ���� �������� ������.
class CMaterial
{
private:
	// �ؽ�ó�� ���� �� ���� ���� �ִ�
	std::vector<CTexture*> m_ppTextures;
	int m_nTextureCount;

	// ���� ����
	MATERIAL m_Material;

	// ���� ��� ����
	ID3D11Buffer* m_pd3dcbMaterial;

public:
	static UINT CB_MATERIAL_SLOT;

public:
	CMaterial( ID3D11Device* pd3dDevice );
	CMaterial( ID3D11Device* pd3dDevice, std::vector<CTexture*> ppTextures );
	CMaterial( ID3D11Device* pd3dDevice, std::vector<CTexture*> ppTextures, MATERIAL& mat );
	CMaterial( ID3D11Device* pd3dDevice, std::vector<CTexture*> ppTextures,
		XMFLOAT4& vDiffuse, XMFLOAT4& vSpecular, XMFLOAT4& m_vAmbient, XMFLOAT4& m_vEmissive );
	CMaterial( ID3D11Device* pd3dDevice, XMFLOAT4& vDiffuse, XMFLOAT4& vSpecular, XMFLOAT4& m_vAmbient, XMFLOAT4& m_vEmissive );
	CMaterial( ID3D11Device* pd3dDevice, MATERIAL& mat );
	~CMaterial( );

	void SetTextures( std::vector<CTexture*> ppTextures );
	void setTexture( int iIndex, CTexture* pTexture );
	std::vector<CTexture*> GetTextures( ) const;
	CTexture* GetTexture( int iIndex ) const;

	void SetMaterial( MATERIAL& mat );
	void SetSpecular( XMFLOAT4& vSpecular );
	void SetDiffuse( XMFLOAT4& vDiffuse );
	void SetAmbient( XMFLOAT4& vAmbient );
	void SetEmiisive( XMFLOAT4& vEmissive );
	MATERIAL GetMaterial( ) const;
	XMFLOAT4 GetSpecular( ) const;
	XMFLOAT4 GetDiffuse( ) const;
	XMFLOAT4 GetAmbient( ) const;
	XMFLOAT4 GetEmissive( ) const;

	void SetcbMaterial( ID3D11DeviceContext* pd3dDeviceContext );
	void SetTextureOnShader( ID3D11DeviceContext* pd3dDeviceContext );
	void SetTextureOnShader( ID3D11DeviceContext* pd3dDeviceContext, UINT idx, UINT nSlot );
};

