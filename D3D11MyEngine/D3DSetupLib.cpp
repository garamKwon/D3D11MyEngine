#include "stdafx.h"
#include "D3DSetupLib.h"

D3D11SetUp::D3D11SetUp( )
{
	m_pd3dDevice = nullptr;
	m_pd3dDeviceContext = nullptr;
	m_pDXGISwapChain = nullptr;
	m_pd3dRenderTargetView = nullptr;
	m_pd3dDepthStencilBuffer = nullptr;
	m_pd3dDepthStencilView = nullptr;
}

D3D11SetUp::~D3D11SetUp( )
{
	m_pd3dDeviceContext->ClearState( );

	SAFE_RELEASE( m_pd3dDepthStencilView );
	SAFE_RELEASE( m_pd3dDepthStencilBuffer );
	SAFE_RELEASE( m_pd3dRenderTargetView );
	SAFE_RELEASE( m_pDXGISwapChain );
	SAFE_RELEASE( m_pd3dDeviceContext );
	SAFE_RELEASE( m_pd3dDevice );
}

bool D3D11SetUp::Init( HWND hWnd, UINT clientWidth, UINT clientHeight )
{
	// Device, DeviceContext, SwapChain �����ϱ�
	UINT dwCreateDeviceFlags = 0;
#ifdef _DEBUG
	dwCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	dwCreateDeviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;	

	// ����̽��� �����ϱ� ���� �õ��� ����̹� ����
	D3D_DRIVER_TYPE d3dDriverTypes[ ] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT nDriverTypes = sizeof( d3dDriverTypes ) / sizeof( D3D10_DRIVER_TYPE );

	// ����̽��� �����ϱ� ���� �õ��� Ư�� ������ ����
	D3D_FEATURE_LEVEL d3dFeatureLevels[ ] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT nFeatureLevels = sizeof( d3dFeatureLevels ) / sizeof( D3D_FEATURE_LEVEL );

	// ������ ���� ü���� �����ϴ� ����ü
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory( &dxgiSwapChainDesc, sizeof( dxgiSwapChainDesc ) );
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = clientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = clientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = true;	// true�̸� ������â, false �̸� ��üȭ��

	D3D_DRIVER_TYPE nd3dDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	HRESULT hResult = S_OK;
	// ����̽��� ����̹� ������ Ư�� ������ �����ϴ� ����̽��� ����ü���� ����
	// ������� ����̽� ������ �õ��ϰ� �����ϸ� ���� ������ ����̽� ������ �õ�
	for (UINT i = 0; i < nDriverTypes; i++)
	{
		nd3dDriverType = d3dDriverTypes[i];
		if (SUCCEEDED( hResult = D3D11CreateDeviceAndSwapChain( NULL, nd3dDriverType, NULL,
			dwCreateDeviceFlags, d3dFeatureLevels, nFeatureLevels, D3D11_SDK_VERSION,
			&dxgiSwapChainDesc, &m_pDXGISwapChain, &m_pd3dDevice, &nd3dFeatureLevel, &m_pd3dDeviceContext ) ))
			break;
	}
	return CreateRenderTargetDepthStencilView( clientWidth, clientHeight );
}
bool D3D11SetUp::CreateRenderTargetDepthStencilView( UINT clientWidth, UINT clientHeight )
{
	HRESULT hResult = S_OK;

	ID3D11Texture2D *pd3dBackBuffer;
	if (FAILED( hResult = m_pDXGISwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID *)&pd3dBackBuffer ) )) return false;
	if (FAILED( hResult = m_pd3dDevice->CreateRenderTargetView( pd3dBackBuffer, NULL, &m_pd3dRenderTargetView ) )) return false;
	if (pd3dBackBuffer) pd3dBackBuffer->Release( );

	// SwapChain�� ����۸� �޾ƿͼ� ����Ÿ�� �����
	// Depth/Stencil buffer �����
	D3D11_TEXTURE2D_DESC d3dDepthStencilBufferDesc;
	::ZeroMemory( &d3dDepthStencilBufferDesc, sizeof( D3D11_TEXTURE2D_DESC ) );
	d3dDepthStencilBufferDesc.Width = clientWidth;
	d3dDepthStencilBufferDesc.Height = clientHeight;
	d3dDepthStencilBufferDesc.MipLevels = 1;
	d3dDepthStencilBufferDesc.ArraySize = 1;
	d3dDepthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilBufferDesc.SampleDesc.Count = 1;
	d3dDepthStencilBufferDesc.SampleDesc.Quality = 0;
	d3dDepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dDepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	d3dDepthStencilBufferDesc.CPUAccessFlags = 0;
	d3dDepthStencilBufferDesc.MiscFlags = 0;

	if (FAILED( hResult = m_pd3dDevice->CreateTexture2D( &d3dDepthStencilBufferDesc, NULL, &m_pd3dDepthStencilBuffer ) )) return false;

	// ������ ���� ���ۿ� ���� �並 ����
	D3D11_DEPTH_STENCIL_VIEW_DESC d3dViewDesc;
	::ZeroMemory( &d3dViewDesc, sizeof( D3D11_DEPTH_STENCIL_VIEW_DESC ) );
	d3dViewDesc.Format = d3dDepthStencilBufferDesc.Format;
	d3dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3dViewDesc.Texture2D.MipSlice = 0;

	if (FAILED( hResult = m_pd3dDevice->CreateDepthStencilView( m_pd3dDepthStencilBuffer, &d3dViewDesc, &m_pd3dDepthStencilView ) )) return false;

	// ��º��ձ⿡ ����
	m_pd3dDeviceContext->OMSetRenderTargets( 1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView );
}

ID3D11DeviceContext* D3D11SetUp::CreateDeferredDeviceContext( )
{
	ID3D11DeviceContext* pd3dDeferredContext = nullptr;
	HRESULT hr = m_pd3dDevice->CreateDeferredContext( 0, &pd3dDeferredContext );

	if (FAILED( hr ))
		return nullptr;
	
	return pd3dDeferredContext;
}

void D3D11SetUp::ReleaseDeferredDeviceContext( ID3D11DeviceContext* pDefereedContext )
{
	SAFE_RELEASE( pDefereedContext )
}

// ���̴� ���ҽ� ��
ID3D11ShaderResourceView* D3D11SetUp::CreateShaderResourceView( ID3D11Resource* pRes, D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc )
{
	ID3D11ShaderResourceView* pView = nullptr;
	HRESULT hr = m_pd3dDevice->CreateShaderResourceView( pRes, pDesc, &pView );

	if (FAILED( hr ))
		return nullptr;

	return pView;
}

// ���� Ÿ�� ��
ID3D11RenderTargetView* D3D11SetUp::CreateRenderTargetView( ID3D11Resource *pRes, D3D11_RENDER_TARGET_VIEW_DESC* pDesc )
{
	ID3D11RenderTargetView* pView = nullptr;
	HRESULT hr = m_pd3dDevice->CreateRenderTargetView( pRes, pDesc, &pView );

	if (FAILED( hr ))
		return nullptr;

	return pView;
}

// ���� ���ٽ� ��
ID3D11DepthStencilView* D3D11SetUp::CreateDepthStencilView( ID3D11Resource* pRes, D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc )
{
	ID3D11DepthStencilView* pView = nullptr;
	HRESULT hr = m_pd3dDevice->CreateDepthStencilView( pRes, pDesc, &pView );

	if (FAILED( hr ))
		return nullptr;

	return pView;
}

// ���� ���� ���� ��
ID3D11UnorderedAccessView* D3D11SetUp::CreateUnorderedAccessView( ID3D11Resource* pRes, D3D11_UNORDERED_ACCESS_VIEW_DESC* pDesc )
{
	ID3D11UnorderedAccessView* pView = nullptr;
	HRESULT hr = m_pd3dDevice->CreateUnorderedAccessView( pRes, pDesc, &pView );

	if (FAILED( hr ))
		return nullptr;

	return pView;
}


// ���� ���̴� ���ҽ� ��
ID3D11ShaderResourceView* D3D11SetUp::CreateBufferSRV( ID3D11Resource* pRes, UINT offset, UINT elementWidth )
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Buffer.ElementOffset = offset;
	desc.Buffer.ElementWidth = elementWidth;

	ID3D11ShaderResourceView* pView;
	HRESULT hr = m_pd3dDevice->CreateShaderResourceView( pRes, &desc, &pView );

	if (FAILED( hr ))
		return nullptr;

	return pView;
}

// ���� ���� ���� ���� ���̴� ���ҽ� ��
ID3D11UnorderedAccessView* D3D11SetUp::CreateBufferUAV( ID3D11Resource* pRes, UINT elementCount)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = elementCount;
	desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;

	ID3D11UnorderedAccessView* pView = nullptr;
	
	HRESULT hr = m_pd3dDevice->CreateUnorderedAccessView( pRes, &desc, &pView );

	if (FAILED( hr ))
		return nullptr;

	return pView;
}

// ���� ����
ID3D11Buffer* D3D11SetUp::CreateVertexBuffer( UINT byteWidth, bool dynamic, bool streamout, D3D11_SUBRESOURCE_DATA* pData )
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = byteWidth;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	// ��Ʈ���ƿ� ���ο� ���� BindFlags ����
	if (streamout)
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	else
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	// �������� ���ο� ���� cpu ���� ��� ����
	if (dynamic)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.CPUAccessFlags = 0;
	}

	ID3D11Buffer* pBuffer = nullptr;
	HRESULT hr = m_pd3dDevice->CreateBuffer( &desc, pData, &pBuffer );

	if (FAILED( hr ))
		return nullptr;

	return pBuffer;
}

// �ε��� ����
ID3D11Buffer* D3D11SetUp::CreateIndecBuffer( UINT byteWidth, bool dynamic, D3D11_SUBRESOURCE_DATA* pData )
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = byteWidth;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	if (dynamic)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.CPUAccessFlags = 0;
	}

	ID3D11Buffer* pBuffer = nullptr;
	HRESULT hr = m_pd3dDevice->CreateBuffer( &desc, pData, &pBuffer );

	if (FAILED( hr ))
		return nullptr;

	return pBuffer;
}

// ��� ����
ID3D11Buffer* D3D11SetUp::CreateContstantBuffer( UINT byteWidth, bool dynamic, bool CPUUpdates, D3D11_SUBRESOURCE_DATA* pData )
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = byteWidth;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	if (dynamic && CPUUpdates)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (dynamic && !CPUUpdates)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = 0;
	}
	else
	{
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.CPUAccessFlags = 0;
	}

	ID3D11Buffer* pBuffer = nullptr;
	HRESULT hr = m_pd3dDevice->CreateBuffer( &desc, pData, &pBuffer );

	if (FAILED( hr ))
		return nullptr;

	return pBuffer;
}

// ǥ��/������ ����  , ����ü �ϳ��� ��� ������ۿ� �޸� ����ü�� �ν��Ͻ���
ID3D11Buffer* D3D11SetUp::CreateStructureBuffer( UINT count, UINT structSize, bool CPUWritable, bool GPUWritable, D3D11_SUBRESOURCE_DATA* pData )
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = count * structSize;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = 0;

	if (!CPUWritable && !GPUWritable)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.CPUAccessFlags = 0;
	}
	else if (CPUWritable && !GPUWritable)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (!CPUWritable && GPUWritable)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
	}
	else if (CPUWritable && GPUWritable)
	{
		// ����, CPU�� GPU�� �� �� �ڿ��� ���� ���������� �����ؼ��� �ȵ�
		return nullptr;
	}

	ID3D11Buffer* pBuffer = nullptr;
	HRESULT hr = m_pd3dDevice->CreateBuffer( &desc, pData, &pBuffer );

	if (FAILED( hr ))
		return nullptr;

	return pBuffer;
}

// �߰�/�Һ� ���� ����
ID3D11Buffer* D3D11SetUp::CreateAppendConsumeBuffer( UINT byteWidth, UINT structSize, D3D11_SUBRESOURCE_DATA* pData )
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = byteWidth * structSize;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = structSize;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;

	ID3D11Buffer* pBuffer = nullptr;
	HRESULT hr = m_pd3dDevice->CreateBuffer( &desc, pData, &pBuffer );

	if (FAILED( hr ))
		return nullptr;

	return pBuffer;
}

// ����Ʈ �ּ� ���� ����
ID3D11Buffer* D3D11SetUp::CreateByteAddressBuffer( UINT byteWidth, bool GPUWritable, D3D11_SUBRESOURCE_DATA* pData )
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = byteWidth;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;	// �ݵ�� �̰����� ����
	desc.StructureByteStride = 0;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;

	ID3D11Buffer* pBuffer = nullptr;
	HRESULT hr = m_pd3dDevice->CreateBuffer( &desc, pData, &pBuffer );

	if (FAILED( hr ))
		return nullptr;

	return pBuffer;
}

// ���� �μ� ���� ����
ID3D11Buffer* D3D11SetUp::CreateIndirectArgumentBuffer( UINT byteWidth, D3D11_SUBRESOURCE_DATA* pData )
{
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = byteWidth;
	desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;	// �ݵ�� �̰����� ����
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.StructureByteStride = 0;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = 0;

	ID3D11Buffer* pBuffer = nullptr;
	HRESULT hr = m_pd3dDevice->CreateBuffer( &desc, pData, &pBuffer );

	if (FAILED( hr ))
		return nullptr;

	return pBuffer;
}

// 1���� �ؽ�ó ����
ID3D11Texture1D* D3D11SetUp::CreateTexture1D( D3D11_TEXTURE1D_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pData )
{
	ID3D11Texture1D* pTexture;
	HRESULT hr = m_pd3dDevice->CreateTexture1D( pDesc, pData, &pTexture );

	if (FAILED( hr ))
		return nullptr;

	return pTexture;
}

// 2���� �ؽ�ó ����
ID3D11Texture2D* D3D11SetUp::CreateTexture2D( D3D11_TEXTURE2D_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pData )
{
	ID3D11Texture2D* pTexture;
	HRESULT hr = m_pd3dDevice->CreateTexture2D( pDesc, pData, &pTexture );

	if (FAILED( hr ))
		return nullptr;

	return pTexture;
}

// 3���� �ؽ�ó ����
ID3D11Texture3D* D3D11SetUp::CreateTexture3D( D3D11_TEXTURE3D_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pData )
{
	ID3D11Texture3D* pTexture;
	HRESULT hr = m_pd3dDevice->CreateTexture3D( pDesc, pData, &pTexture );

	if (FAILED( hr ))
		return nullptr;

	return pTexture;
}

// ���÷� ���°�ü ����
ID3D11SamplerState* D3D11SetUp::CreateSamplerState( D3D11_SAMPLER_DESC* pDesc )
{
	ID3D11SamplerState* pState;
	HRESULT hr = m_pd3dDevice->CreateSamplerState( pDesc, &pState );

	if (FAILED( hr ))
		return nullptr;

	return pState;
}

D3D11_MAPPED_SUBRESOURCE D3D11SetUp::MappingResource( ID3D11Resource* pData )
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	m_pd3dDeviceContext->Map( pData, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource );

	return d3dMappedResource;
}

void D3D11SetUp::UnmappingResource( ID3D11Resource* pData )
{
	m_pd3dDeviceContext->Unmap( pData, 0 );
}

void D3D11SetUp::ClearRenderTargetView( const float* fClearColor )
{
	if (m_pd3dRenderTargetView) 
		m_pd3dDeviceContext->ClearRenderTargetView( m_pd3dRenderTargetView, fClearColor );
}

void D3D11SetUp::ClearDepthStencilView( UINT nClarFlag, float fDepth, UINT8 nStencil )
{
	if (m_pd3dDepthStencilView)
		m_pd3dDeviceContext->ClearDepthStencilView( m_pd3dDepthStencilView, nClarFlag, fDepth, nStencil );
}

void D3D11SetUp::Present( UINT nSyncInterval, UINT flag )
{
	if (m_pDXGISwapChain)
		m_pDXGISwapChain->Present( nSyncInterval, flag );
}