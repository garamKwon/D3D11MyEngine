#pragma once
#include <string>
#include "GameObject.h"

// �ؽ�ó �������� ���� ��ü, CTexture�� ��ü�� ���� ���� ���� �ؽ�ó���� ��� ������ �ִ�.
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