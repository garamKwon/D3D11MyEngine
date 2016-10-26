#pragma once
#include <vector>
#include <map>
#include "BoundingBox.h"
#include "Shader.h"
#include "GameObject.h"
#include "Camera.h"

// static mesh�� ���������� ������ ����ü
struct CStaticVertex
{
	XMFLOAT3 m_position;
	XMFLOAT2 m_textureUV;
	XMFLOAT3 m_normal;
	XMFLOAT3 m_tangent;
	XMFLOAT3 m_binormal;
};

// animate mesh�� ���������� ������ ����ü
struct CAnimateVertex
{
	XMFLOAT3 m_position = XMFLOAT3(-10000,-10000,-10000);
	XMFLOAT2 m_textureUV;
	XMFLOAT3 m_normal;
	XMFLOAT3 m_tangent;
	XMFLOAT3 m_binormal;

	XMFLOAT3 m_weights = XMFLOAT3( 0.0f, 0.0f, 0.0f );					// ����ġ
	XMFLOAT4 m_boneIndices = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );		// �� ������ ������ �ִ� ����
};

// Bone �ϳ��� �ǹ��ϴ� ����ü
struct Bone
{
public:
	int parentIdx;				// ���� ������������ �θ��� �ε���
	std::string boneName;		// �ڽ��� �̸�
	std::string parentBoneName;	// �θ��� �̸�
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

// �ϳ��� ����, �ִϸ��̼� �������� ������ ����
struct BoneAnimation		// CAnimation
{
	// �ð��� ���� ��ȯ ������ ����, ���밡 �����̴� ���
	std::vector<Keyframe> Keyframes;
	// �� �̸�
	std::string BoneName;
	// �θ� �̸�
	std::string PerentBoneName;
	// �� �ε���
	int BoneIndex;

	float GetStartTime() const;
	float GetEndTime() const;

	void Interpolate(float t, XMFLOAT4X4& M);
};

// �ִϸ��̼� Ŭ�� �����, �ȱ�, �ٱ�, ���� ���� �������� �ִϸ��̼��� �ǹ���
struct AnimationClip		// CAnimationSet
{
	// ���� Ŭ���� ��� ������ ���� �ð� �� ���� ���� ���� ��ȯ
	float GetClipStartTime() const;
	// ���� Ŭ���� ��� ������ ���� �ð� �� ���� ���� ���� ��ȯ
	float GetClipEndTime() const;

	// �� Ŭ���� �� BoneAnimation�� �����鼭 �ִϸ��̼ǵ��� ����
	void Interpolate(float t, std::vector<XMFLOAT4X4>& boneTransforms);

	// ���뺰 �ִϸ��̼ǵ�
	std::vector<BoneAnimation> BoneAnimations;

	// ���� ã��
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

// ��Ű�� �ִϸ��̼� �ڷḦ ���� Ŭ����
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

	// ������ȯ�� ���ϴ� �Լ�, ������ ��ȯ���� ��ģ ��ȯ ��Ʈ�����̴�.
	void GetFinalTransforms(const int& clipName, float timePos,
		std::vector<XMFLOAT4X4>& finalTransforms);
	void SkinnedData::GetMatrixByTime(const int& clipName, float timePos, std::vector<XMFLOAT4X4>& finalTransforms);

public:
	// i�� ������ �θ��� �̸��� ��´�, i�� ����� �ִϸ��̼� Ŭ���� i��° BoneAnimaion�ν��Ͻ��� �����ȴ�.
	std::vector<Bone> mBoneHierarchy;
	// i�� ������ ������ ��ȯ, �θ�� ���� ������ ��ȯ
	std::vector<XMFLOAT4X4> mBoneOffsets;
	// �ִϸ��̼� ������ Ŭ������ ����
	std::map<int, AnimationClip> mAnimations;
};


// �޽� ������ ���� ��ü, �޽ô� ����, ����, �ε��� ���� ������ ������
class CMesh : public CBaseObject
{
protected:
	std::vector<UINT> m_svIndices;
	// �ε������� ����
	DXGI_FORMAT m_dxgiFormat;
	// �ε��� ������
	UINT m_nIndexOffset;
	// ���� �ε���
	UINT m_nStartIndex;

	// �� �޽ø� �׸��� ���� �⺻���� ������ ( Topology )
	D3D11_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology;
	// �� �޽ø� �׸��� ���� �����Ͷ����� ���� ��ü 
	ID3D11RasterizerState *m_pd3dRasterizerState;

	// ���� ����, ������ �����ϴ� �������� �ϳ��� ������ ����
	ID3D11Buffer **m_ppd3dVertexBuffers;
	// �ε��� ���ۿ� ���� �������̽� ������
	ID3D11Buffer *m_pd3dIndexBuffer;
	// ��ġ ���� ����
	ID3D11Buffer *m_pd3dPositionBuffer;
	// ���� ���� ����
	ID3D11Buffer *m_pd3dNormalBuffer;
	// uv��ǥ ���� ����
	ID3D11Buffer *m_pd3dUVBuffer;
	// tangent ���� ����
	ID3D11Buffer *m_pd3dTangentBuffer;
	// binormal ���� ����
	ID3D11Buffer *m_pd3dBinormalBuffer;
	// ������ �����ϱ� ���� ���� ����
	UINT m_nBuffers;
	// ������ ��ҵ��� ��Ÿ���� ���۵��� ������ ����Ʈ ���� ��Ÿ���� �迭
	UINT *m_pnVertexStrides;
	// ������ ��ҵ��� ��Ÿ���� ���۵��� ���� ��ġ�� ��Ÿ���� �迭
	UINT *m_pnVertexOffsets;
	// ���� ������ ���� ��ȣ
	UINT m_nSlot;
	// ���� ����
	UINT m_nStartVertex;
	UINT m_nBaseVertex;

	// �ø� � ����� �ٿ���ڽ�
	AABB m_bcBoundingBox;

	// ���̴�
	CShader *m_pShader;
	// ���׸���
	CMaterial *m_pMaterial;
	
	bool m_bVisible;
public:
	CMesh( std::string strName, ObjectLayer iLayer, ObjectType iType );
	// �߻�Ŭ������ ���� �ڽ� Ŭ�������� �������Ͽ� ����ؾ߸� ��
	virtual ~CMesh( ) = 0;

	// �޽��� ���� ���� ���۵��� �迭�� ����
	void AssembleToVertexBuffer( int nBuffers = 0, ID3D11Buffer **m_pd3dBuffers = NULL, UINT *pnBufferStrides = NULL, UINT *pnBufferOffset = NULL );

	// �޽� �׸���
	virtual void Render( ID3D11DeviceContext *pd3dDeviceContext ){};
	virtual void RenderInstanced( ID3D11DeviceContext *pd3dDeviceContext, int nInstances = 0, int nStartInstance = 0 ){};
	virtual void Update(float fTimeElapsed) {};

	// �����Ͷ����� ���°�ü ����
	virtual void CreateRasterizerState( ID3D11Device *pd3dDevice, bool clockWise = false, D3D11_CULL_MODE cullMode = D3D11_CULL_BACK, D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID);

	AABB GetBoundingBox( ) const;
	void SetBoundingBox( XMFLOAT3& max, XMFLOAT3& min );
	void SetBoundingBox( AABB& boundingBox );

	// ���̴� ����
	void SetShader( CShader *pShader );
	CShader *GetShader( ) const;

	// ���׸��� ����
	void SetMaterial( CMaterial *pMaterial );
	CMaterial* GetMaterial( ) const;

	void SetIsVisible(bool visible);
	bool GetIsVisible()const;
};

// ���ϸ��̼��� ���� ����ƽ �޽�
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

// ���ϸ��̼��� �ִ� �޽�
class CAnimateMesh : public CMesh
{
protected:
	std::vector<CAnimateVertex> m_svVertices;
	// �ִϸ��̼ǿ� �ʿ��� �������� �־�� ��
	// ����ġ ����
	ID3D11Buffer *m_pd3dWeightBuffer;
	// �� ���� ����
	ID3D11Buffer *m_pd3dBoneIdxBuffer;
	// �ִϸ��̼� ����
	SkinnedData m_AnimData;
	std::vector<XMFLOAT4X4> m_pmtxFinalTransforms;
	float m_fTimePos;
	int m_iAnimState;

public:
	CAnimateMesh( ID3D11Device *pd3dDevice, std::vector<CAnimateVertex>& vertices, std::vector<UINT>& indices, SkinnedData& animData, D3D11_PRIMITIVE_TOPOLOGY primitiveTopology,
		std::string strName, ObjectLayer iLayer, ObjectType iType = ObjectType::TYPE_ANIMATE_MESH );		// ���� ���
	~CAnimateMesh( );

	std::vector<CAnimateVertex> GetVertices( ) const;
	virtual void Render( ID3D11DeviceContext *pd3dDeviceContext);
	virtual void Update(float fTimeElapsed);
};

// ��ī�̹ڽ� �޽�
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