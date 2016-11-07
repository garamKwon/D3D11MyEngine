#pragma once
#include "Texture.h"

struct MATERIAL
{
	XMFLOAT4 m_vAmbient;
	XMFLOAT4 m_vDiffuse;
	XMFLOAT4 m_vSpecular;
	XMFLOAT4 m_vEmissive;
};

// 재질은 텍스처를 가지고 있으며 여러 관련 정보들을 가진다.
class CMaterial : public CBaseObject
{
private:
	// 텍스처를 여러 장 가질 수도 있다
	std::vector<CTexture*> m_ppTextures;
	int m_nTextureCount;

	// 재질 정보
	MATERIAL m_Material;

	// 재질 상수 버퍼
	ID3D11Buffer* m_pd3dcbMaterial;

public:
	static UINT CB_MATERIAL_SLOT;

public:
	CMaterial( ID3D11Device* pd3dDevice, std::string strName, ObjectLayer iLayer, ObjectType iType);
	CMaterial( ID3D11Device* pd3dDevice, std::vector<CTexture*> ppTextures, std::string strName, ObjectLayer iLayer, ObjectType iType);
	CMaterial( ID3D11Device* pd3dDevice, std::vector<CTexture*> ppTextures, MATERIAL& mat, std::string strName, ObjectLayer iLayer, ObjectType iType);
	CMaterial( ID3D11Device* pd3dDevice, std::vector<CTexture*> ppTextures,
		XMFLOAT4& vDiffuse, XMFLOAT4& vSpecular, XMFLOAT4& m_vAmbient, XMFLOAT4& m_vEmissive, std::string strName, ObjectLayer iLayer, ObjectType iType);
	CMaterial( ID3D11Device* pd3dDevice, XMFLOAT4& vDiffuse, XMFLOAT4& vSpecular, XMFLOAT4& m_vAmbient, XMFLOAT4& m_vEmissive, std::string strName, ObjectLayer iLayer, ObjectType iType);
	CMaterial( ID3D11Device* pd3dDevice, MATERIAL& mat, std::string strName, ObjectLayer iLayer, ObjectType iType);
	~CMaterial( );

	void SetTextures( std::vector<CTexture*> ppTextures );
	void setTexture( int iIndex, CTexture* pTexture );
	std::vector<CTexture*> GetTextures( ) const;
	CTexture* GetTexture( int iIndex ) const;
	CTexture* GetTexture(std::string strName) const;

	// 원래 있던 텍스처 반환
	CTexture* ChangeTexture(int srcTexIdx, CTexture* dstTexture);
	CTexture* ChangeTexture(std::string& strName, CTexture* dstTexture);
	CTexture* ChangeTexture(ID3D11Device* pd3dDevice, std::string& strDstName, TCHAR* strFileName, std::string& strSrcName);

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

