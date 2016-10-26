#pragma once

class D3D11SetUp
{
	ID3D11Device*			m_pd3dDevice;
	ID3D11DeviceContext*	m_pd3dDeviceContext;
	IDXGISwapChain*			m_pDXGISwapChain;
	ID3D11RenderTargetView*	m_pd3dRenderTargetView;
	ID3D11Texture2D*		m_pd3dDepthStencilBuffer;
	ID3D11DepthStencilView*	m_pd3dDepthStencilView;

public:
	D3D11SetUp( );
	~D3D11SetUp( );
	bool Init( HWND hWnd, UINT clientWidth, UINT clientHeight );
	bool CreateRenderTargetDepthStencilView( UINT clientWidth, UINT clientHeight );

	// 멤버변수 값 관련
	ID3D11Device* GetDevice( ){ return m_pd3dDevice; }
	ID3D11DeviceContext* GetDeviceContext( ) { return m_pd3dDeviceContext; }
	ID3D11RenderTargetView* GetRenderTargetView( ){ return m_pd3dRenderTargetView; }
	IDXGISwapChain* GetSwapChain( ){ return m_pDXGISwapChain; }
	ID3D11Texture2D* GetDepthStencilBuffer( ){ return m_pd3dDepthStencilBuffer; }
	ID3D11DepthStencilView*	GetDepthStencilView( ) { return m_pd3dDepthStencilView; }
	void SetRenderTargetView( ID3D11RenderTargetView* pView ){ m_pd3dRenderTargetView = pView; }
	void SetDepthStencilView( ID3D11DepthStencilView* pView ){ m_pd3dDepthStencilView = pView; }

	// 멀티스레드 렌더링을 위한 지연 문맥의 생성과 해제
	ID3D11DeviceContext* CreateDeferredDeviceContext( );
	void ReleaseDeferredDeviceContext( ID3D11DeviceContext* pDefereedContext );

	// 자원 뷰 관련
	ID3D11ShaderResourceView* CreateShaderResourceView( ID3D11Resource* pRes, D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc );	
	ID3D11RenderTargetView* CreateRenderTargetView( ID3D11Resource *pRes, D3D11_RENDER_TARGET_VIEW_DESC* pDesc );
	ID3D11DepthStencilView* CreateDepthStencilView( ID3D11Resource* pRes, D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc );
	ID3D11UnorderedAccessView* CreateUnorderedAccessView( ID3D11Resource* pRes, D3D11_UNORDERED_ACCESS_VIEW_DESC* pDesc );
	ID3D11ShaderResourceView* CreateBufferSRV( ID3D11Resource* pRes, UINT offset, UINT elementWidth );
	ID3D11UnorderedAccessView* CreateBufferUAV( ID3D11Resource* pRes, UINT elementCount );
	ID3D11ShaderResourceView* CreateTexteureSRV( ID3D11Resource* pRes, UINT dimension, UINT mipLevels, UINT mostDetailedMap );
	ID3D11UnorderedAccessView* CreateTextureUAV( ID3D11Resource* pRes, UINT dimension, UINT mipSlice );
	ID3D11RenderTargetView* CreateTextureRTV( ID3D11Resource* pRes, UINT dimension, UINT mipSlice );

	// 버퍼 자원 관련
	ID3D11Buffer* CreateVertexBuffer( UINT byteWidth, bool dynamic, bool streamout, D3D11_SUBRESOURCE_DATA* pData );	
	ID3D11Buffer* CreateIndecBuffer( UINT byteWidth, bool dynamic, D3D11_SUBRESOURCE_DATA* pData );							
	ID3D11Buffer* CreateContstantBuffer( UINT byteWidth, bool dynamic, bool CPUUpdates, D3D11_SUBRESOURCE_DATA* pData );
	ID3D11Buffer* CreateStructureBuffer( UINT count, UINT structSize, bool CPUWritable, bool GPUWritable, D3D11_SUBRESOURCE_DATA* pData );
	ID3D11Buffer* CreateAppendConsumeBuffer( UINT byteWidth, UINT structSize, D3D11_SUBRESOURCE_DATA* pData );
	ID3D11Buffer* CreateByteAddressBuffer( UINT byteWidth, bool GPUWritable, D3D11_SUBRESOURCE_DATA* pData );
	ID3D11Buffer* CreateIndirectArgumentBuffer( UINT byteWidth, D3D11_SUBRESOURCE_DATA* pData );
	
	// 텍스처 관련
	ID3D11Texture1D* CreateTexture1D( D3D11_TEXTURE1D_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pData );
	ID3D11Texture2D* CreateTexture2D( D3D11_TEXTURE2D_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pData );
	ID3D11Texture3D* CreateTexture3D( D3D11_TEXTURE3D_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pData );

	// 샘플러 상태 객체
	ID3D11SamplerState* CreateSamplerState( D3D11_SAMPLER_DESC* pDesc );

	// 자원 조작
	D3D11_MAPPED_SUBRESOURCE MappingResource( ID3D11Resource* pData );
	void UnmappingResource( ID3D11Resource* pData );

	// 렌더링 관련
	void ClearRenderTargetView( const float* fClearColor );
	void ClearDepthStencilView( UINT nClarFlag = D3D11_CLEAR_DEPTH, float fDepth = 1.0f, UINT8 nStencil = 0);
	void Present( UINT nSyncInterval, UINT flag);
};