#include "stdafx.h"
#include "Mesh.h"
#include "Material.h"

// bone Animation
float BoneAnimation::GetStartTime()const
{
	return Keyframes.front().TimePos;
}

float BoneAnimation::GetEndTime()const
{
	float f = Keyframes.back().TimePos;

	return f;
}

void BoneAnimation::Interpolate(float t, XMFLOAT4X4& M)
{
	if (t <= Keyframes.front().TimePos)
	{
		XMVECTOR S = XMLoadFloat3(&Keyframes.front().Scale);
		XMVECTOR P = XMLoadFloat3(&Keyframes.front().Translation);
		XMVECTOR Q = XMLoadFloat4(&Keyframes.front().RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else if (t >= Keyframes.back().TimePos)
	{
		XMVECTOR S = XMLoadFloat3(&Keyframes.back().Scale);
		XMVECTOR P = XMLoadFloat3(&Keyframes.back().Translation);
		XMVECTOR Q = XMLoadFloat4(&Keyframes.back().RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else
	{
		for (UINT i = 0; i < Keyframes.size() - 1; ++i)
		{
			if (t >= Keyframes[i].TimePos && t <= Keyframes[i + 1].TimePos)
			{
				float lerpPercent = (t - Keyframes[i].TimePos) / (Keyframes[i + 1].TimePos - Keyframes[i].TimePos);

				XMVECTOR s0 = XMLoadFloat3(&Keyframes[i].Scale);
				XMVECTOR s1 = XMLoadFloat3(&Keyframes[i + 1].Scale);

				XMVECTOR p0 = XMLoadFloat3(&Keyframes[i].Translation);
				XMVECTOR p1 = XMLoadFloat3(&Keyframes[i + 1].Translation);

				XMVECTOR q0 = XMLoadFloat4(&Keyframes[i].RotationQuat);
				XMVECTOR q1 = XMLoadFloat4(&Keyframes[i + 1].RotationQuat);

				XMVECTOR S = XMVectorLerp(s0, s1, lerpPercent);
				XMVECTOR P = XMVectorLerp(p0, p1, lerpPercent);
				XMVECTOR Q = XMQuaternionSlerp(q0, q1, lerpPercent);

				XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
				XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));

				break;
			}
		}
	}
}

// animation clip
float AnimationClip::GetClipStartTime()const
{
	float t = 100000.f;
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		t = Min(t, BoneAnimations[i].GetStartTime());
	}

	return t;
}

float AnimationClip::GetClipEndTime()const
{
	float t = 0.0f;
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		t = Max(t, BoneAnimations[i].GetEndTime());
	}

	return t;
}

void AnimationClip::Interpolate(float t, std::vector<XMFLOAT4X4>& boneTransforms)
{
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		BoneAnimations[i].Interpolate(t, boneTransforms[i]);
	}
}

BoneAnimation* AnimationClip::FindBone(int index)
{
	for (UINT i = 0; i < BoneAnimations.size(); i++)
	{
		BoneAnimations[i].BoneIndex == index;
		return &BoneAnimations[i];
	}
}

BoneAnimation* AnimationClip::FindBone(std::string name)
{
	for (UINT i = 0; i < BoneAnimations.size(); i++)
	{
		BoneAnimations[i].BoneName == name;
		return &BoneAnimations[i];
	}
}

// skinned data
float SkinnedData::GetClipStartTime(const int& clipName)const
{
	auto clip = mAnimations.find(clipName);
	return clip->second.GetClipStartTime();
}

float SkinnedData::GetClipEndTime(const int& clipName)const
{
	auto clip = mAnimations.find(clipName);
	return clip->second.GetClipEndTime();
}

UINT SkinnedData::BoneCount()const
{
	return mBoneHierarchy.size();
}

void SkinnedData::Set(std::vector<Bone>& boneHierarchy,
	std::vector<XMFLOAT4X4>& boneOffsets,
	std::map<int, AnimationClip>& animations)
{
	mBoneHierarchy = boneHierarchy;
	mBoneOffsets = boneOffsets;
	mAnimations = animations;
}

void SkinnedData::GetMatrixByTime(const int& clipName, float timePos, std::vector<XMFLOAT4X4>& finalTransforms)
{
	int startTime = GetTickCount();
	// 이 클립의 모든 뼈대를 주어진 시간에 맞게 보간
	auto clip = mAnimations.find(clipName);
	clip->second.Interpolate(timePos, finalTransforms);
	int endTime = GetTickCount() - startTime;
}

void SkinnedData::GetFinalTransforms(const int& clipName, float timePos, std::vector<XMFLOAT4X4>& finalTransforms)
{
	UINT numBones = mBoneOffsets.size();

	std::vector<XMFLOAT4X4> toParentTransforms(numBones);

	// 이 클립의 모든 뼈대를 주어진 시간에 맞게 보간
	auto clip = mAnimations.find(clipName);
	clip->second.Interpolate(timePos, toParentTransforms);

	// 뼈대 계층구조를 훑으면서 모든 뼈대를 뿌리공간으로 변환
	std::vector<XMFLOAT4X4> toRootTransforms(numBones);

	toRootTransforms[0] = toParentTransforms[0];

	// 자식 뼈대들의 뿌리변환을 구함
	for (UINT i = 1; i < numBones; ++i)
	{
		// 루트노드인 경우
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);

		// 하이라키가 제대로 완성되면 수정 필요, 부모로 가는 인덱스가 필요
		int parentIndex = mBoneHierarchy[i].parentIdx;
		XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);

		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);

		XMStoreFloat4x4(&toRootTransforms[i], toRoot);
	}

	// 뼈대 오프셋 변환을 곱해 최종변환을 구함
	for (UINT i = 0; i < numBones; ++i)
	{
		XMMATRIX offset = XMLoadFloat4x4(&mBoneOffsets[i]);
		XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
		XMStoreFloat4x4(&finalTransforms[i], XMMatrixMultiply(offset, toRoot));
	}
}

// meshes
CMesh::CMesh( std::string strName, ObjectLayer iLayer, ObjectType iType ) : CBaseObject( strName, iLayer, iType )
{
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_pd3dRasterizerState = nullptr;
	m_ppd3dVertexBuffers = nullptr;
	m_pd3dIndexBuffer = nullptr;
	m_pd3dPositionBuffer = nullptr;
	m_pd3dNormalBuffer = nullptr;
	m_pd3dUVBuffer = nullptr;
	m_pd3dTangentBuffer = nullptr;
	m_pd3dBinormalBuffer = nullptr;
	m_pnVertexStrides = nullptr;
	m_pnVertexOffsets = nullptr;
	m_pShader = nullptr;

	m_nBuffers = 0;
	m_nIndexOffset = 0;
	m_nSlot = 0;
	m_nStartIndex = 0;
	m_nStartVertex = 0;
	m_nBaseVertex = 0;
	m_bVisible = false;

	m_dxgiFormat = DXGI_FORMAT_R32_UINT;
}

CMesh::~CMesh( )
{
}

void CMesh::AssembleToVertexBuffer( int nBuffers, ID3D11Buffer **ppd3dBuffers, UINT *pnBufferStrides, UINT * pnBufferOffsets )
{
	ID3D11Buffer **ppd3dNewVertexBuffers = new ID3D11Buffer*[m_nBuffers + nBuffers];
	UINT *pnNewVertexStrides = new UINT[m_nBuffers + nBuffers];
	UINT *pnNewVertexOffsets = new UINT[m_nBuffers + nBuffers];

	if (m_nBuffers > 0)
	{
		for (int i = 0; i < m_nBuffers; i++)
		{
			ppd3dNewVertexBuffers[i] = m_ppd3dVertexBuffers[i];
			pnNewVertexStrides[i] = m_pnVertexStrides[i];
			pnNewVertexOffsets[i] = m_pnVertexOffsets[i];
		}
		if (m_ppd3dVertexBuffers)	SAFE_DELETE_ARRAY( m_ppd3dVertexBuffers );
		if (m_pnVertexStrides)		SAFE_DELETE_ARRAY( m_pnVertexStrides );
		if (m_pnVertexOffsets)		SAFE_DELETE_ARRAY( m_pnVertexOffsets );
	}

	for (int i = 0; i < nBuffers; i++)
	{
		ppd3dNewVertexBuffers[m_nBuffers + i] = ppd3dBuffers[i];
		pnNewVertexStrides[m_nBuffers + i] = pnBufferStrides[i];
		pnNewVertexOffsets[m_nBuffers + i] = pnBufferOffsets[i];
	}

	m_nBuffers += nBuffers;
	m_ppd3dVertexBuffers = ppd3dNewVertexBuffers;
	m_pnVertexStrides = pnNewVertexStrides;
	m_pnVertexOffsets = pnNewVertexOffsets;
}

void CMesh::CreateRasterizerState( ID3D11Device *pd3dDevice, bool clockWise, D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode )
{
	D3D11_RASTERIZER_DESC d3dRastersizerDesc;
	::ZeroMemory( &d3dRastersizerDesc, sizeof( D3D11_RASTERIZER_DESC ) );
	d3dRastersizerDesc.CullMode = cullMode;
	d3dRastersizerDesc.FrontCounterClockwise = clockWise;
	d3dRastersizerDesc.FillMode = fillMode;
	pd3dDevice->CreateRasterizerState( &d3dRastersizerDesc, &m_pd3dRasterizerState );
}

OOBB CMesh::GetBoundingBox()const
{
	return m_bcBoundingBox;
}

//void CMesh::SetBoundingBox( XMFLOAT3& max, XMFLOAT3& min )
//{
//	m_bcBoundingBox.m_vMax = max;
//	m_bcBoundingBox.m_vMin = min;
//}
//
//void CMesh::SetBoundingBox( AABB& boundingBox )
//{
//	m_bcBoundingBox = boundingBox;
//}

void CMesh::SetShader( CShader *pShader )
{
	if (m_pShader)
		delete m_pShader;

	m_pShader = pShader;
}

CShader* CMesh::GetShader( ) const
{
	if (m_pShader)
		return m_pShader;
}

void CMesh::SetIsVisible(bool visible)
{
	m_bVisible = visible;
}

bool CMesh::GetIsVisible()const
{
	return m_bVisible;
}

// CStaticMesh
CStaticMesh::CStaticMesh( ID3D11Device *pd3dDevice, std::vector<CAnimateVertex>& vertices, std::vector<UINT>& indices,
	D3D11_PRIMITIVE_TOPOLOGY primitiveTopology, std::string strName, ObjectLayer iLayer, ObjectType iType ) : CMesh( strName, iLayer, iType )
{
	m_svIndices = indices;
	
	m_d3dPrimitiveTopology = primitiveTopology;

	UINT nVertices = vertices.size( );
	m_svVertices.resize(nVertices);

	std::vector<XMFLOAT3> svPositions( nVertices );
	std::vector<XMFLOAT2> svTexCoords( nVertices );
	std::vector<XMFLOAT3> svNormals( nVertices );
	std::vector<XMFLOAT3> svTangents( nVertices );
	std::vector<XMFLOAT3> svBinormals( nVertices );

	for (int i = 0; i < nVertices; i++)
	{
		m_svVertices[i].m_binormal = vertices[i].m_binormal;
		m_svVertices[i].m_position = vertices[i].m_position;
		m_svVertices[i].m_textureUV = vertices[i].m_textureUV;
		m_svVertices[i].m_normal = vertices[i].m_normal;
		m_svVertices[i].m_tangent = vertices[i].m_tangent;

		svPositions[i] = vertices[i].m_position;
		svTexCoords[i] = vertices[i].m_textureUV;
		svNormals[i] = vertices[i].m_normal;
		svTangents[i] = vertices[i].m_tangent;
		svBinormals[i] = vertices[i].m_binormal;
	}

	// 정점 버퍼 생성
	D3D11_BUFFER_DESC d3dBufferDesc;
	::ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof( XMFLOAT3 ) * svPositions.size( );
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	::ZeroMemory( &d3dBufferData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	d3dBufferData.pSysMem = &svPositions[0];
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer );

	d3dBufferDesc.ByteWidth = sizeof( XMFLOAT2 ) * svTexCoords.size( );
	d3dBufferData.pSysMem = &svTexCoords[0];
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dUVBuffer );

	// normal 좌표
	d3dBufferDesc.ByteWidth = sizeof( XMFLOAT3 ) * svNormals.size( );
	d3dBufferData.pSysMem = &svNormals[0];
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer );

	// tangent
	d3dBufferDesc.ByteWidth = sizeof( XMFLOAT3 ) * svTangents.size( );
	d3dBufferData.pSysMem = &svTangents[0];
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dTangentBuffer );

	// binormal
	d3dBufferDesc.ByteWidth = sizeof( XMFLOAT3 ) * svBinormals.size( );
	d3dBufferData.pSysMem = &svBinormals[0];
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dBinormalBuffer );

	ID3D11Buffer *pd3dBuffers[5] = { m_pd3dPositionBuffer, m_pd3dUVBuffer, m_pd3dNormalBuffer, m_pd3dTangentBuffer, m_pd3dBinormalBuffer };
	UINT pnBufferStrides[5] = { sizeof( XMFLOAT3 ), sizeof( XMFLOAT2 ), sizeof( XMFLOAT3 ), sizeof( XMFLOAT3 ), sizeof(XMFLOAT3) };
	UINT pnBufferOffsets[5] = { 0, 0, 0, 0, 0 };
	AssembleToVertexBuffer( 5, pd3dBuffers, pnBufferStrides, pnBufferOffsets );

	::ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof( UINT ) * m_svIndices.size();
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	::ZeroMemory( &d3dBufferData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	d3dBufferData.pSysMem = &m_svIndices[0];
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer );

	CreateRasterizerState( pd3dDevice );

	m_pShader = new CStaticShader( pd3dDevice );
}

CStaticMesh::~CStaticMesh( )
{
	SAFE_RELEASE( m_pd3dRasterizerState );
	SAFE_RELEASE( m_pd3dIndexBuffer );
	SAFE_RELEASE( m_pd3dPositionBuffer );
	SAFE_RELEASE( m_pd3dNormalBuffer );
	SAFE_RELEASE( m_pd3dUVBuffer );;
	SAFE_RELEASE( m_pd3dTangentBuffer );
	SAFE_RELEASE( m_pd3dBinormalBuffer );

	delete( m_pnVertexStrides );
	delete( m_pnVertexOffsets );
}

std::vector<CStaticVertex> CStaticMesh::GetVertices( ) const
{
	return m_svVertices;
}

void CStaticMesh::Render( ID3D11DeviceContext *pd3dDeviceContext )
{
	if (m_pShader)
	{
		// InpuLayout 세팅
		pd3dDeviceContext->IASetInputLayout( m_pShader->GetInputLayout( ) );
		// 상수버퍼 값 세팅
		// 셰이더들 연결
		m_pShader->SetShadersOnPipeline( pd3dDeviceContext );
	}
	// 정점 버퍼 세팅
	pd3dDeviceContext->IASetVertexBuffers( m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets );
	// 인덱스 버퍼 세팅
	pd3dDeviceContext->IASetIndexBuffer( m_pd3dIndexBuffer, m_dxgiFormat, m_nIndexOffset );
	// 프리미티브 토폴로지 세팅
	pd3dDeviceContext->IASetPrimitiveTopology( m_d3dPrimitiveTopology );
	// 레스터라이저 상태 객체 세팅
	pd3dDeviceContext->RSSetState( m_pd3dRasterizerState );

	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexed( m_svIndices.size( ), m_nStartIndex, m_nBaseVertex );
	else
		pd3dDeviceContext->Draw( m_svVertices.size(), m_nStartVertex );
}


// CAnimateMesh
CAnimateMesh::CAnimateMesh( ID3D11Device *pd3dDevice, std::vector<CAnimateVertex>& vertices, std::vector<UINT>& indices, SkinnedData& animData,
	D3D11_PRIMITIVE_TOPOLOGY primitiveTopology, std::string strName, ObjectLayer iLayer, ObjectType iType ) : CMesh( strName, iLayer, iType )
{
	m_svIndices = indices;

	m_d3dPrimitiveTopology = primitiveTopology;
	m_AnimData = animData;
	m_pmtxFinalTransforms = animData.mBoneOffsets;
	m_fTimePos = 0.0f;
	m_iAnimState = ObjectState::STATE_IDLE;

	UINT nVertices = vertices.size();
	m_svVertices = vertices;

	std::vector<XMFLOAT3> svPositions(nVertices);
	std::vector<XMFLOAT2> svTexCoords(nVertices);
	std::vector<XMFLOAT3> svNormals(nVertices);
	std::vector<XMFLOAT3> svTangents(nVertices);
	std::vector<XMFLOAT3> svBinormals(nVertices);
	std::vector<XMFLOAT3> svWeights(nVertices);
	std::vector<XMFLOAT4> svInfluenceBones(nVertices);

	for (int i = 0; i < nVertices; i++)
	{
		svPositions[i] = vertices[i].m_position;
		svTexCoords[i] = vertices[i].m_textureUV;
		svNormals[i] = vertices[i].m_normal;
		svTangents[i] = vertices[i].m_tangent;
		svBinormals[i] = vertices[i].m_binormal;
		svInfluenceBones[i] = vertices[i].m_boneIndices;
		svWeights[i] = vertices[i].m_weights;
	}

	// 정점 버퍼 생성
	D3D11_BUFFER_DESC d3dBufferDesc;
	::ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(XMFLOAT3) * svPositions.size();
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	::ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = &svPositions[0];
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferDesc.ByteWidth = sizeof(XMFLOAT2) * svTexCoords.size();
	d3dBufferData.pSysMem = &svTexCoords[0];
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dUVBuffer);

	// normal 좌표
	d3dBufferDesc.ByteWidth = sizeof(XMFLOAT3) * svNormals.size();
	d3dBufferData.pSysMem = &svNormals[0];
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	// tangent
	d3dBufferDesc.ByteWidth = sizeof(XMFLOAT3) * svTangents.size();
	d3dBufferData.pSysMem = &svTangents[0];
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dTangentBuffer);

	// binormal
	d3dBufferDesc.ByteWidth = sizeof(XMFLOAT3) * svBinormals.size();
	d3dBufferData.pSysMem = &svBinormals[0];
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dBinormalBuffer);

	// weight
	d3dBufferDesc.ByteWidth = sizeof(XMFLOAT3) * svWeights.size();
	d3dBufferData.pSysMem = &svWeights[0];
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dWeightBuffer);

	// boneIndices
	d3dBufferDesc.ByteWidth = sizeof(XMFLOAT4) * svInfluenceBones.size();
	d3dBufferData.pSysMem = &svInfluenceBones[0];
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dBoneIdxBuffer);

	ID3D11Buffer *pd3dBuffers[7] = { m_pd3dPositionBuffer, m_pd3dUVBuffer, m_pd3dNormalBuffer, m_pd3dTangentBuffer, m_pd3dBinormalBuffer, m_pd3dWeightBuffer, m_pd3dBoneIdxBuffer };
	UINT pnBufferStrides[7] = { sizeof(XMFLOAT3), sizeof(XMFLOAT2), sizeof(XMFLOAT3), sizeof(XMFLOAT3), sizeof(XMFLOAT3), sizeof(XMFLOAT3), sizeof(XMFLOAT4) };
	UINT pnBufferOffsets[7] = { 0, 0, 0, 0, 0, 0, 0};
	AssembleToVertexBuffer(7, pd3dBuffers, pnBufferStrides, pnBufferOffsets);

	::ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(UINT) * m_svIndices.size();
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	::ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = &m_svIndices[0];
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer);

	CreateRasterizerState(pd3dDevice);

	m_pShader = new CAnimateShader(pd3dDevice);
}

CAnimateMesh::~CAnimateMesh( )
{
	SAFE_RELEASE( m_pd3dRasterizerState );
	SAFE_RELEASE( m_pd3dIndexBuffer );
	SAFE_RELEASE( m_pd3dPositionBuffer );
	SAFE_RELEASE( m_pd3dNormalBuffer );
	SAFE_RELEASE( m_pd3dUVBuffer );;
	SAFE_RELEASE( m_pd3dTangentBuffer );
	SAFE_RELEASE( m_pd3dBinormalBuffer );
	SAFE_RELEASE( m_pd3dBoneIdxBuffer );
	SAFE_RELEASE( m_pd3dWeightBuffer );
	delete( m_pnVertexStrides );
	delete( m_pnVertexOffsets );
}
	
std::vector<CAnimateVertex> CAnimateMesh::GetVertices( ) const
{
	return m_svVertices;
}

void CAnimateMesh::Render( ID3D11DeviceContext *pd3dDeviceContext )
{
	// 셰이더들 연결
	if (m_pShader)
	{
		// InpuLayout 세팅
		pd3dDeviceContext->IASetInputLayout(m_pShader->GetInputLayout());
		// 셰이더들 연결
		m_pShader->SetShadersOnPipeline(pd3dDeviceContext);
		// 상수버퍼 값 세팅
		CAnimateShader::SetFinalTransformMatrix(pd3dDeviceContext, m_pmtxFinalTransforms);
	}
	// 정점 버퍼 세팅
	pd3dDeviceContext->IASetVertexBuffers( m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets );
	// 인덱스 버퍼 세팅
	pd3dDeviceContext->IASetIndexBuffer( m_pd3dIndexBuffer, m_dxgiFormat, m_nIndexOffset );
	// 프리미티브 토폴로지 세팅
	pd3dDeviceContext->IASetPrimitiveTopology( m_d3dPrimitiveTopology );
	// 레스터라이저 상태 객체 세팅
	pd3dDeviceContext->RSSetState( m_pd3dRasterizerState );

	if (m_pd3dIndexBuffer)
		pd3dDeviceContext->DrawIndexed( m_svIndices.size( ), m_nStartIndex, m_nBaseVertex );
	else
		pd3dDeviceContext->Draw( m_svVertices.size( ), m_nStartVertex );
}

void CAnimateMesh::Update(float fTimeElapsed)
{
	m_fTimePos += fTimeElapsed;

	if(m_bVisible)
		m_AnimData.GetFinalTransforms(static_cast<int>(m_iAnimState), m_fTimePos, m_pmtxFinalTransforms);

	if (m_fTimePos > m_AnimData.GetClipEndTime(static_cast<int>(m_iAnimState)))
		m_fTimePos = 0.0f;
}

void CAnimateMesh::SetAnimationState(ObjectState animState)
{
	m_iAnimState = animState;
}

ObjectState CAnimateMesh::GetAnimationState() const
{
	return m_iAnimState;
}

CSkyboxMesh::CSkyboxMesh( ID3D11Device *pd3dDevice, std::vector<CStaticVertex>& vertices, std::vector<UINT>& indices, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology,
	std::string strName, ObjectLayer iLayer, ObjectType iType ) : CMesh( strName, iLayer, iType )
{
	m_svIndices = indices;
	m_svVertices = vertices;
	m_d3dPrimitiveTopology = primitiveTopology;

	UINT nVertices = vertices.size( );
	std::vector<XMFLOAT3> svPositions( nVertices );
	std::vector<XMFLOAT2> svTexCoords( nVertices );

	for (int i = 0; i < nVertices; i++)
	{
		svPositions[i] = vertices[i].m_position;
		svTexCoords[i] = vertices[i].m_textureUV;
	}

	// 정점 버퍼 생성
	D3D11_BUFFER_DESC d3dBufferDesc;
	::ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof( XMFLOAT3 ) * svPositions.size( );
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	::ZeroMemory( &d3dBufferData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	d3dBufferData.pSysMem = &svPositions[0];
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer );

	d3dBufferDesc.ByteWidth = sizeof( XMFLOAT2 ) * svTexCoords.size( );
	d3dBufferData.pSysMem = &svTexCoords[0];
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dUVBuffer );

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dUVBuffer };
	UINT pnBufferStrides[2] = { sizeof( XMFLOAT3 ), sizeof( XMFLOAT2 ) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer( 2, pd3dBuffers, pnBufferStrides, pnBufferOffsets );

	m_svIndices = indices;
	::ZeroMemory( &d3dBufferDesc, sizeof( D3D11_BUFFER_DESC ) );
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof( UINT ) * m_svIndices.size( );
	d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	::ZeroMemory( &d3dBufferData, sizeof( D3D11_SUBRESOURCE_DATA ) );
	d3dBufferData.pSysMem = &m_svIndices[0];
	pd3dDevice->CreateBuffer( &d3dBufferDesc, &d3dBufferData, &m_pd3dIndexBuffer );

	D3D11_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory( &d3dDepthStencilDesc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
	// 스카이 박스 사각형들은 깊이 버퍼 알고리즘을 적용하지 않고 깊이 버퍼를 변경하지 않음
	d3dDepthStencilDesc.DepthEnable = false;
	d3dDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	d3dDepthStencilDesc.DepthFunc = D3D11_COMPARISON_NEVER;
	d3dDepthStencilDesc.StencilEnable = false;
	d3dDepthStencilDesc.StencilReadMask = 0xFF;
	d3dDepthStencilDesc.StencilWriteMask = 0xFF;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	pd3dDevice->CreateDepthStencilState( &d3dDepthStencilDesc, &m_pd3dDepthStencilState );

	CreateRasterizerState( pd3dDevice );

	m_pShader = new CSkyboxShader( pd3dDevice );
}

CSkyboxMesh::~CSkyboxMesh( )
{
	SAFE_RELEASE( m_pd3dDepthStencilState );
	SAFE_RELEASE( m_pd3dRasterizerState );
	SAFE_RELEASE( m_pd3dIndexBuffer );
	SAFE_RELEASE( m_pd3dPositionBuffer );
	SAFE_RELEASE( m_pd3dNormalBuffer );
	SAFE_RELEASE( m_pd3dUVBuffer );;
	SAFE_RELEASE( m_pd3dTangentBuffer );
	SAFE_RELEASE( m_pd3dBinormalBuffer );

	delete( m_pnVertexStrides );
	delete( m_pnVertexOffsets );
}
std::vector<CStaticVertex> CSkyboxMesh::GetVertices( ) const
{
	return m_svVertices;
}

void CSkyboxMesh::Render( ID3D11DeviceContext *pd3dDeviceContext )
{
	if (m_pShader)
	{
		// InpuLayout 세팅
		pd3dDeviceContext->IASetInputLayout( m_pShader->GetInputLayout( ) );
		// 상수버퍼 값 세팅
		// 셰이더들 연결
		m_pShader->SetShadersOnPipeline( pd3dDeviceContext );
	}
	// 정점 버퍼 세팅
	pd3dDeviceContext->IASetVertexBuffers( m_nSlot, m_nBuffers, m_ppd3dVertexBuffers, m_pnVertexStrides, m_pnVertexOffsets );
	// 인덱스 버퍼 세팅
	pd3dDeviceContext->IASetIndexBuffer( m_pd3dIndexBuffer, m_dxgiFormat, m_nIndexOffset );
	// 프리미티브 토폴로지 세팅
	pd3dDeviceContext->IASetPrimitiveTopology( m_d3dPrimitiveTopology );
	// 레스터라이저 상태 객체 세팅
	pd3dDeviceContext->RSSetState( m_pd3dRasterizerState );
	// 깊이/스텐실 버퍼 설정
	pd3dDeviceContext->OMSetDepthStencilState( m_pd3dDepthStencilState, 1 );

	for (int i = 0; i < 6; i++)
	{
		if (((CGameObject*)GetParentObject())->GetMaterial())
		{
			((CGameObject*)GetParentObject())->GetMaterial()->SetcbMaterial( pd3dDeviceContext );
			((CGameObject*)GetParentObject())->GetMaterial()->SetTextureOnShader( pd3dDeviceContext, i , 0);
		}
		pd3dDeviceContext->DrawIndexed( 4, 0, i * 4 );
	}
	pd3dDeviceContext->OMSetDepthStencilState( NULL, 1 );
}

