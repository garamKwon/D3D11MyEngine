#pragma once
#include <vector>
#include <map>
#include "BoundingBox.h"
#include "Shader.h"
#include "GameObject.h"
#include "Camera.h"

// static mesh의 정점정보를 가지는 구조체
struct CStaticVertex
{
	XMFLOAT3 m_position;
	XMFLOAT2 m_textureUV;
	XMFLOAT3 m_normal;
	XMFLOAT3 m_tangent;
	XMFLOAT3 m_binormal;
};

// animate mesh의 정점정보를 가지는 구조체
struct CAnimateVertex
{
	XMFLOAT3 m_position = XMFLOAT3(-10000,-10000,-10000);
	XMFLOAT2 m_textureUV;
	XMFLOAT3 m_normal;
	XMFLOAT3 m_tangent;
	XMFLOAT3 m_binormal;

	XMFLOAT3 m_weights = XMFLOAT3( 0.0f, 0.0f, 0.0f );					// 가중치
	XMFLOAT4 m_boneIndices = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );		// 이 정점에 영향을 주는 뼈대
};

// Bone 하나를 의미하는 구조체
struct Bone
{
public:
	int parentIdx;				// 뼈대 계층구조에서 부모의 인덱스
	std::string boneName;		// 자신의 이름
	std::string parentBoneName;	// 부모의 이름
};

struct Keyframe			// CKeyframe
{
	Keyframe::Keyframe() : TimePos(0.0f), Translation(0.0f, 0.0f, 0.0f),
		Scale(1.0f, 1.0f, 1.0f), RotationQuat(0.0f, 0.0f, 0.0f, 1.0f) {}

	float TimePos;
	XMFLOAT3 Translation;
	XMFLOAT3 Scale;
	XMFLOAT4 RotationQuat;
};

// 하나의 뼈대, 애니메이션 정보들을 가지고 있음
struct BoneAnimation		// CAnimation
{
	// 시간에 따른 변환 값들을 저장, 뼈대가 움직이는 모양
	std::vector<Keyframe> Keyframes;
	// 본 이름
	std::string BoneName;
	// 부모 이름
	std::string PerentBoneName;
	// 본 인덱스
	int BoneIndex;

	float GetStartTime() const;
	float GetEndTime() const;

	void Interpolate(float t, XMFLOAT4X4& M);
};

// 애니메이션 클립 만들기, 걷기, 뛰기, 공격 등의 개별적인 애니메이션을 의미함
struct AnimationClip		// CAnimationSet
{
	// 현재 클립의 모든 뼈대의 시작 시간 중 가장 빠른 것을 반환
	float GetClipStartTime() const;
	// 현재 클립의 모든 뼈대의 종료 시간 중 가장 늦은 것을 반환
	float GetClipEndTime() const;

	// 이 클립의 각 BoneAnimation을 훑으면서 애니메이션들을 보간
	void Interpolate(float t, std::vector<XMFLOAT4X4>& boneTransforms);

	// 뼈대별 애니메이션들
	std::vector<BoneAnimation> BoneAnimations;

	// 뼈대 찾기
	BoneAnimation* FindBone(int index);
	BoneAnimation* FindBone(std::string name);

	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}
};

// 스키닝 애니메이션 자료를 담을 클래스
class SkinnedData
{
public:
	UINT BoneCount() const;
	float GetClipStartTime(const int& clipName) const;
	float GetClipEndTime(const int& clipName) const;

	void Set(
		std::vector<Bone>& boneHierarchy,
		std::vector<XMFLOAT4X4>& boneOffsets,
		std::map<int, AnimationClip>& animations);

	// 최종변환을 구하는 함수, 오프셋 변환까지 마친 변환 매트릭스이다.
	void GetFinalTransforms(const int& clipName, float timePos,
		std::vector<XMFLOAT4X4>& finalTransforms);
	void SkinnedData::GetMatrixByTime(const int& clipName, float timePos, std::vector<XMFLOAT4X4>& finalTransforms);

public:
	// i번 뼈대의 부모의 이름을 담는다, i번 뼈대는 애니메이션 클립의 i번째 BoneAnimaion인스턴스에 대응된다.
	std::vector<Bone> mBoneHierarchy;
	// i번 뼈대의 오프셋 변환, 부모로 가는 오프셋 변환
	std::vector<XMFLOAT4X4> mBoneOffsets;
	// 애니메이션 정보를 클립별로 저장
	std::map<int, AnimationClip> mAnimations;
};


// 메시 정보를 가질 객체, 메시는 정점, 법선, 인덱스 등의 정보를 가진다
class CMesh : public CBaseObject
{
protected:
	std::vector<UINT> m_svIndices;
	// 인덱스버퍼 포맷
	DXGI_FORMAT m_dxgiFormat;
	// 인덱스 오프셋
	UINT m_nIndexOffset;
	// 시작 인덱스
	UINT m_nStartIndex;

	// 이 메시를 그리기 위한 기본도형 위상구조 ( Topology )
	D3D11_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;
	// 이 메시를 그리기 위한 레스터라이저 상태 객체 
	ID3D11RasterizerState *m_pd3dRasterizerState;

	// 정점 버퍼, 정점을 구성하는 정보들을 하나로 통합한 버퍼
	ID3D11Buffer **m_ppd3dVertexBuffers;
	// 인덱스 버퍼에 대한 인터페이스 포인터
	ID3D11Buffer *m_pd3dIndexBuffer;
	// 위치 정보 버퍼
	ID3D11Buffer *m_pd3dPositionBuffer;
	// 법선 정보 버퍼
	ID3D11Buffer *m_pd3dNormalBuffer;
	// uv좌표 정보 버퍼
	ID3D11Buffer *m_pd3dUVBuffer;
	// tangent 정보 버퍼
	ID3D11Buffer *m_pd3dTangentBuffer;
	// binormal 정보 버퍼
	ID3D11Buffer *m_pd3dBinormalBuffer;
	// 정점을 조립하기 위한 버퍼 개수
	UINT m_nBuffers;
	// 정점의 요소들을 나타내는 버퍼들의 원소의 바이트 수를 나타내는 배열
	UINT *m_pnVertexStrides;
	// 정점의 요소들을 나타내는 버퍼들의 시작 위치를 나타내는 배열
	UINT *m_pnVertexOffsets;
	// 정점 버퍼의 슬롯 번호
	UINT m_nSlot;
	// 시작 정점
	UINT m_nStartVertex;
	UINT m_nBaseVertex;

	// 컬링 등에 사용할 바운딩박스
	AABB m_bcBoundingBox;

	// 셰이더
	CShader *m_pShader;
	// 머테리얼
	CMaterial *m_pMaterial;
	
	bool m_bVisible;
public:
	CMesh( std::string strName, ObjectLayer iLayer, ObjectType iType );
	// 추상클래스로 만들어서 자식 클래스에서 재정의하여 사용해야만 함
	virtual ~CMesh( ) = 0;

	// 메시의 정점 정보 버퍼들을 배열로 조립
	void AssembleToVertexBuffer( int nBuffers = 0, ID3D11Buffer **m_pd3dBuffers = NULL, UINT *pnBufferStrides = NULL, UINT *pnBufferOffset = NULL );

	// 메시 그리기
	virtual void Render( ID3D11DeviceContext *pd3dDeviceContext ){};
	virtual void RenderInstanced( ID3D11DeviceContext *pd3dDeviceContext, int nInstances = 0, int nStartInstance = 0 ){};
	virtual void Update(float fTimeElapsed) {};

	// 레스터라이저 상태객체 생성
	virtual void CreateRasterizerState( ID3D11Device *pd3dDevice, bool clockWise = false, D3D11_CULL_MODE cullMode = D3D11_CULL_BACK, D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID);

	AABB GetBoundingBox( ) const;
	void SetBoundingBox( XMFLOAT3& max, XMFLOAT3& min );
	void SetBoundingBox( AABB& boundingBox );

	// 셰이더 관련
	void SetShader( CShader *pShader );
	CShader *GetShader( ) const;

	// 머테리얼 관련
	void SetMaterial( CMaterial *pMaterial );
	CMaterial* GetMaterial( ) const;

	void SetIsVisible(bool visible);
	bool GetIsVisible()const;
};

// 에니메이션이 없는 스태틱 메시
class CStaticMesh : public CMesh
{
protected:
	std::vector<CStaticVertex> m_svVertices;

public:
	CStaticMesh( ID3D11Device *pd3dDevice, std::vector<CAnimateVertex>& vertices, std::vector<UINT>& indices, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology,
		std::string strName, ObjectLayer iLayer, ObjectType iType = ObjectType::TYPE_STATIC_MESH );		
	~CStaticMesh( );

	std::vector<CStaticVertex> GetVertices( ) const;
	virtual void Render( ID3D11DeviceContext *pd3dDeviceContext);
};

// 에니메이션이 있는 메시
class CAnimateMesh : public CMesh
{
protected:
	std::vector<CAnimateVertex> m_svVertices;
	// 애니메이션에 필요한 정보들이 있어야 함
	// 가중치 버퍼
	ID3D11Buffer *m_pd3dWeightBuffer;
	// 본 정보 버퍼
	ID3D11Buffer *m_pd3dBoneIdxBuffer;
	// 애니메이션 정보
	SkinnedData m_AnimData;
	std::vector<XMFLOAT4X4> m_pmtxFinalTransforms;
	float m_fTimePos;
	int m_iAnimState;

public:
	CAnimateMesh( ID3D11Device *pd3dDevice, std::vector<CAnimateVertex>& vertices, std::vector<UINT>& indices, SkinnedData& animData, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology,
		std::string strName, ObjectLayer iLayer, ObjectType iType = ObjectType::TYPE_ANIMATE_MESH );		// 수정 요망
	~CAnimateMesh( );

	std::vector<CAnimateVertex> GetVertices( ) const;
	virtual void Render( ID3D11DeviceContext *pd3dDeviceContext);
	virtual void Update(float fTimeElapsed);
};

// 스카이박스 메시
class CSkyboxMesh : public CMesh
{
protected:
	std::vector<CStaticVertex> m_svVertices;
	ID3D11DepthStencilState *m_pd3dDepthStencilState;

public:
	CSkyboxMesh( ID3D11Device *pd3dDevice, std::vector<CStaticVertex>& vertices, std::vector<UINT>& indices, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology,
		std::string strName, ObjectLayer iLayer, ObjectType iType = ObjectType::TYPE_SKYBOX );	
	~CSkyboxMesh( );

	std::vector<CStaticVertex> GetVertices( ) const;
	virtual void Render( ID3D11DeviceContext *pd3dDeviceContext );
};