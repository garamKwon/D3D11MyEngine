#pragma once

class CCamera;

struct cbTransformMatrix
{
	XMFLOAT4X4 gmtxWorld;
	XMFLOAT4X4 gmtxView;
	XMFLOAT4X4 gmtxProjection;
};

struct cbBoneOffsetMatrixes
{
	XMFLOAT4X4 m_mtxOffsets[96];
};

// 오브젝트가 가지고 있을 모든 셰이더 객체들을 가지는 클래스
class CShader
{
protected:
	// 각 세이더들 
	ID3D11VertexShader *m_pd3dVertexShader;
	ID3D11PixelShader *m_pd3dPixelShader;
	
	// 인풋 레이아웃 객체
	ID3D11InputLayout *m_pd3dInputLayout;

	// 변환 행렬을 위한 상수 버퍼
	static ID3D11Buffer *m_pd3dcbWorldViewProectionMatrix;

public:
	static UINT CB_SHADER_SLOT;

public:
	CShader( ID3D11Device* pd3dDevice );
	virtual ~CShader( );

	// 각 셰이더를 필요한 것만 생성할 수 있게 한다
	void CreateVertexShaderAndInputLayout( ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements );
	void CreatePixelShader( ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName );

	void DeleteVertexShader( );
	void DeletePixelShader( );

	void ChangeVertexShaderAndInputLayout( ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements );
	void ChangePixelShader( ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName );

	static void SetVertexShaderContantBuffers( ID3D11DeviceContext *pd3dDeviceContext, XMFLOAT4X4 *mtxWorld, CCamera* pCamera );

	virtual void SetShadersOnPipeline( ID3D11DeviceContext* pd3dDeviceContext );
	ID3D11InputLayout* GetInputLayout( ) const;
};

class CModelShader : public CShader
{
protected:
	// 각 세이더들 
	ID3D11HullShader *m_pd3dHullShader;
	ID3D11DomainShader *m_pd3dDomainShader;
	ID3D11GeometryShader *m_pd3dGeometryShader;

public:
	CModelShader(ID3D11Device* pd3dDevice);
	virtual ~CModelShader();

	// 각 셰이더를 필요한 것만 생성할 수 있게 한다
	void CreateHullShader(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName);
	void CreateDomainShader(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName);
	void CreateGeometryShader(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName);

	void DeleteHullShader();
	void DeleteDomainShader();
	void DeleteGeometryShader();

	void ChangeHullShader(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName);
	void ChangeDomainShader(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName);
	void ChangeGeometryShader(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName);

	virtual void SetShadersOnPipeline(ID3D11DeviceContext* pd3dDeviceContext);
};
class CStaticShader : public CModelShader
{
public:
	CStaticShader(ID3D11Device* pd3dDevice );
};

class CSkyboxShader : public CShader
{
public:
	CSkyboxShader( ID3D11Device* pd3dDevice );
	~CSkyboxShader( );
};

class CAnimateShader : public CModelShader
{
public:
	static UINT CB_FINAL_TRANSFORM_SLOT;
	static ID3D11Buffer *m_pd3dcbOffsetMatrix;

public:
	CAnimateShader(ID3D11Device* pd3dDevice);

	static void SetFinalTransformMatrix(ID3D11DeviceContext *pd3dDeviceContext, std::vector<XMFLOAT4X4>& pmtxFinalTransforms);
};