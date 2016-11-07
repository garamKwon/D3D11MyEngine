#pragma once
#include <string>
#include "GameObject.h"

// 텍스처 정보들을 가질 객체, CTexture는 객체가 가질 여러 개의 텍스처들을 모두 가지고 있다.
class CTexture : public CBaseObject
{
private:
	ID3D11ShaderResourceView *m_pd3dTextureSRV;
	ID3D11SamplerState *m_pd3dSamplerStates;

	std::string m_strFileName;

public:
	CTexture( ID3D11Device* pd3dDevice, TCHAR* strFileName, std::string strName, ObjectLayer iLayer, ObjectType iType = ObjectType::TYPE_TEXTURE );
	~CTexture( );

	std::string GetName( ) const;
	std::string GetFileName( ) const;

	ID3D11ShaderResourceView* GetSRV( );
	ID3D11SamplerState* GetSamplerState( );

	static CTexture* MakeTexture(ID3D11Device* pd3dDevice, TCHAR* strFileName, const char*  strObjectName);
	static std::string TCHARToString(const TCHAR* ptsz);
};