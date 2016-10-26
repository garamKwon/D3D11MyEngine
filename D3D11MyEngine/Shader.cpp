#include "stdafx.h"
#include "Shader.h"
#include "Camera.h"

ID3D11Buffer *CShader::m_pd3dcbWorldViewProectionMatrix = nullptr;
ID3D11Buffer *CAnimateShader::m_pd3dcbOffsetMatrix = nullptr;

CShader::CShader( ID3D11Device* pd3dDevice )
{
	m_pd3dVertexShader = nullptr;
	m_pd3dPixelShader = nullptr;
	m_pd3dInputLayout = nullptr;
}

CShader::~CShader( )
{
	SAFE_RELEASE( m_pd3dVertexShader );
	SAFE_RELEASE( m_pd3dPixelShader );
	SAFE_RELEASE( m_pd3dInputLayout );
	SAFE_RELEASE( m_pd3dcbWorldViewProectionMatrix );
}

void CShader::CreateVertexShaderAndInputLayout( ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName,
	D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements )
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	if (SUCCEEDED( hResult = D3DX11CompileFromFile( pszFileName, NULL, NULL, pszShaderName, "vs_5_0", dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL ) ))
	{
		// 컴파일된 셰이더 코드의 메모리 주소에서 정점 셰이더를 생성
		pd3dDevice->CreateVertexShader( pd3dShaderBlob->GetBufferPointer( ), pd3dShaderBlob->GetBufferSize( ), NULL, &m_pd3dVertexShader );
		// 컴파일된 셰이더 코드의 메모리 주소와 입력 레이아웃에서 정점 레이아웃을 생성
		pd3dDevice->CreateInputLayout( pd3dInputLayout, nElements, pd3dShaderBlob->GetBufferPointer( ), pd3dShaderBlob->GetBufferSize( ), &m_pd3dInputLayout );
		pd3dShaderBlob->Release( );

		if (!m_pd3dcbWorldViewProectionMatrix)
		{
			// 월드 변환 행렬을 위한 상수 버퍼를 생성
			D3D11_BUFFER_DESC bd;
			::ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(cbTransformMatrix);
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbWorldViewProectionMatrix);
		}
	}
}


void CShader::CreatePixelShader( ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName )
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	if (SUCCEEDED( hResult = D3DX11CompileFromFile( pszFileName, NULL, NULL, pszShaderName, "ps_4_0", dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL ) ))
	{
		// 컴파일된 셰이더 코드의 메모리 주소에서 픽셀 셰이더를 생성
		pd3dDevice->CreatePixelShader( pd3dShaderBlob->GetBufferPointer( ), pd3dShaderBlob->GetBufferSize( ), NULL, &m_pd3dPixelShader );
		pd3dShaderBlob->Release( );
	}
}

void CShader::DeleteVertexShader( )
{
	SAFE_RELEASE( m_pd3dVertexShader );
}

void CShader::DeletePixelShader( )
{
	SAFE_RELEASE( m_pd3dPixelShader );
}

void CShader::ChangeVertexShaderAndInputLayout( ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName,
	D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements )
{
	DeleteVertexShader( );
	CreateVertexShaderAndInputLayout( pd3dDevice, pszFileName, pszShaderName, pd3dInputLayout, nElements );
}
void CShader::ChangePixelShader( ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName )
{
	DeletePixelShader( );
	CreatePixelShader( pd3dDevice, pszFileName, pszShaderName );
}

void CShader::SetVertexShaderContantBuffers( ID3D11DeviceContext *pd3dDeviceContext, XMFLOAT4X4 *mtxWorld, CCamera* pCamera )
{
	if (!m_pd3dcbWorldViewProectionMatrix)
		return;

	// 월드 변환 행렬을 상수 버퍼에 복사
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map( m_pd3dcbWorldViewProectionMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource );
	cbTransformMatrix *pcbWorldMatrix = (cbTransformMatrix*)d3dMappedResource.pData;

	XMFLOAT4X4 tmtx;
	XMMATRIX mtx = XMLoadFloat4x4( mtxWorld );

	mtx = XMMatrixTranspose( mtx );
	XMStoreFloat4x4( &tmtx, mtx );
	pcbWorldMatrix->gmtxWorld = tmtx;

	mtx = XMLoadFloat4x4( &( pCamera->GetViewMatrix( ) ) );
	mtx = XMMatrixTranspose( mtx );
	XMStoreFloat4x4( &tmtx, mtx );
	pcbWorldMatrix->gmtxView = tmtx;

	mtx = XMLoadFloat4x4( &( pCamera->GetProjectionMatrix( ) ) );
	mtx = XMMatrixTranspose( mtx );
	XMStoreFloat4x4( &tmtx, mtx );
	pcbWorldMatrix->gmtxProjection = tmtx;

	pd3dDeviceContext->Unmap( m_pd3dcbWorldViewProectionMatrix, 0 );

	// 상수 버퍼를 디바이스의 슬롯에 연결
	pd3dDeviceContext->VSSetConstantBuffers( CShader::CB_SHADER_SLOT, 1, &m_pd3dcbWorldViewProectionMatrix );
}

void CShader::SetShadersOnPipeline( ID3D11DeviceContext* pd3dDeviceContext )
{
	if (m_pd3dVertexShader)
		pd3dDeviceContext->VSSetShader( m_pd3dVertexShader, 0, 0 );
	if (m_pd3dPixelShader)
		pd3dDeviceContext->PSSetShader( m_pd3dPixelShader, 0, 0 );
}

ID3D11InputLayout* CShader::GetInputLayout( ) const
{
	return m_pd3dInputLayout;
}

CModelShader::CModelShader( ID3D11Device* pd3dDevice ) : CShader( pd3dDevice )
{
	m_pd3dVertexShader = nullptr;
	m_pd3dHullShader = nullptr;
	m_pd3dDomainShader = nullptr;
	m_pd3dGeometryShader = nullptr;
	m_pd3dPixelShader = nullptr;
	m_pd3dInputLayout = nullptr;
}


CModelShader::~CModelShader( )
{
	SAFE_RELEASE( m_pd3dVertexShader );
	SAFE_RELEASE( m_pd3dHullShader );
	SAFE_RELEASE( m_pd3dDomainShader );
	SAFE_RELEASE( m_pd3dGeometryShader );
	SAFE_RELEASE( m_pd3dPixelShader );
	SAFE_RELEASE( m_pd3dInputLayout );
	SAFE_RELEASE( m_pd3dcbWorldViewProectionMatrix );
}

void CModelShader::CreateHullShader( ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName )
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	if (SUCCEEDED( hResult = D3DX11CompileFromFile( pszFileName, NULL, NULL, pszShaderName, "hs_4_0", dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL ) ))
	{
		// 컴파일된 셰이더 코드의 메모리 주소에서 픽셀 셰이더를 생성
		pd3dDevice->CreateHullShader( pd3dShaderBlob->GetBufferPointer( ), pd3dShaderBlob->GetBufferSize( ), NULL, &m_pd3dHullShader );
		pd3dShaderBlob->Release( );
	}
}

void CModelShader::CreateDomainShader( ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName )
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	if (SUCCEEDED( hResult = D3DX11CompileFromFile( pszFileName, NULL, NULL, pszShaderName, "ds_4_0", dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL ) ))
	{
		// 컴파일된 셰이더 코드의 메모리 주소에서 픽셀 셰이더를 생성
		pd3dDevice->CreateDomainShader( pd3dShaderBlob->GetBufferPointer( ), pd3dShaderBlob->GetBufferSize( ), NULL, &m_pd3dDomainShader );
		pd3dShaderBlob->Release( );
	}
}

void CModelShader::CreateGeometryShader( ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName )
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	if (SUCCEEDED( hResult = D3DX11CompileFromFile( pszFileName, NULL, NULL, pszShaderName, "gs_4_0", dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL ) ))
	{
		// 컴파일된 셰이더 코드의 메모리 주소에서 픽셀 셰이더를 생성
		pd3dDevice->CreateGeometryShader( pd3dShaderBlob->GetBufferPointer( ), pd3dShaderBlob->GetBufferSize( ), NULL, &m_pd3dGeometryShader );
		pd3dShaderBlob->Release( );
	}
}

void CModelShader::DeleteHullShader( )
{
	SAFE_RELEASE( m_pd3dHullShader );
}

void CModelShader::DeleteDomainShader( )
{
	SAFE_RELEASE( m_pd3dDomainShader );
}

void CModelShader::DeleteGeometryShader( )
{
	SAFE_RELEASE( m_pd3dGeometryShader );
}

void CModelShader::ChangeHullShader( ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName )
{
	DeleteHullShader( );
	CreateHullShader( pd3dDevice, pszFileName, pszShaderName );
}

void CModelShader::ChangeDomainShader( ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName )
{
	DeleteDomainShader( );
	CreateDomainShader( pd3dDevice, pszFileName, pszShaderName );
}

void CModelShader::ChangeGeometryShader( ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName )
{
	DeleteGeometryShader( );
	CreateGeometryShader( pd3dDevice, pszFileName, pszShaderName );
}

void CModelShader::SetShadersOnPipeline( ID3D11DeviceContext* pd3dDeviceContext )
{
	if (m_pd3dVertexShader)
		pd3dDeviceContext->VSSetShader( m_pd3dVertexShader, 0, 0 );
	if (m_pd3dHullShader)
		pd3dDeviceContext->HSSetShader( m_pd3dHullShader, 0, 0 );
	if (m_pd3dDomainShader)
		pd3dDeviceContext->DSSetShader( m_pd3dDomainShader, 0, 0 );
	if (m_pd3dGeometryShader)
		pd3dDeviceContext->GSSetShader( m_pd3dGeometryShader, 0, 0 );
	if (m_pd3dPixelShader)
		pd3dDeviceContext->PSSetShader( m_pd3dPixelShader, 0, 0 );
}

CSkyboxShader::CSkyboxShader( ID3D11Device* pd3dDevice ) : CShader( pd3dDevice )
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[ ] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT nElements = ARRAYSIZE( d3dInputElements );

	CreateVertexShaderAndInputLayout( pd3dDevice, L"BaseShader.fx", "VSTexturedColor", d3dInputElements, nElements );
	CreatePixelShader( pd3dDevice, L"BaseShader.fx", "PSTexturedColor" );
}

CSkyboxShader::~CSkyboxShader( )
{
	SAFE_RELEASE( m_pd3dVertexShader );
	SAFE_RELEASE( m_pd3dPixelShader );
	SAFE_RELEASE( m_pd3dInputLayout );
	SAFE_RELEASE( m_pd3dcbWorldViewProectionMatrix );
}

CStaticShader::CStaticShader(ID3D11Device* pd3dDevice) : CModelShader(pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);

	CreateVertexShaderAndInputLayout(pd3dDevice, L"BaseShader.fx", "VSmain", d3dInputElements, nElements);
	CreatePixelShader(pd3dDevice, L"BaseShader.fx", "PSmain");
}

CAnimateShader::CAnimateShader(ID3D11Device* pd3dDevice) : CModelShader(pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 5, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 6, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT nElements = ARRAYSIZE(d3dInputElements);

	CreateVertexShaderAndInputLayout(pd3dDevice, L"BaseShader.fx", "SkinnedVS", d3dInputElements, nElements);
	CreatePixelShader(pd3dDevice, L"BaseShader.fx", "SkinnedPS");

	if (!m_pd3dcbOffsetMatrix)
	{
		D3D11_BUFFER_DESC bd;
		::ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(cbBoneOffsetMatrixes);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbOffsetMatrix);
	}
}

void CAnimateShader::SetFinalTransformMatrix(ID3D11DeviceContext *pd3dDeviceContext, std::vector<XMFLOAT4X4>& pmtxFinalTransforms )
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbOffsetMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	cbBoneOffsetMatrixes *pcbOffsetMatrix = (cbBoneOffsetMatrixes*)d3dMappedResource.pData;
	for (int i = 0; i < pmtxFinalTransforms.size(); i++) {
		XMFLOAT4X4 tmtx;
		XMMATRIX mtx = XMLoadFloat4x4(&pmtxFinalTransforms[i]);

		mtx = XMMatrixTranspose(mtx);
		XMStoreFloat4x4(&tmtx, mtx);
		pcbOffsetMatrix->m_mtxOffsets[i] = tmtx;
	}
	pd3dDeviceContext->Unmap(m_pd3dcbOffsetMatrix, 0);
	// 상수 버퍼를 디바이스의 슬롯에 연결
	pd3dDeviceContext->VSSetConstantBuffers(CAnimateShader::CB_FINAL_TRANSFORM_SLOT, 1, &m_pd3dcbOffsetMatrix);
}