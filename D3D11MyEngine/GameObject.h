#pragma once
#include "Shader.h"

class CMesh;
class CMaterial;
class CTexture;
class CCamera;

enum class ObjectLayer{ LAYER_SCENE, LAYER_CONTROLLER };
enum class ObjectType{ TYPE_DIRECTIONALLIGHT, TYPE_POINTLIGHT, TYPE_SPOTLIGHT, TYPE_STATIC_MESH, TYPE_ANIMATE_MESH,
	TYPE_OBJECT, TYPE_TEXTURE, TYPE_MATERIAL, TYPE_SCENE, TYPE_CAMERA, TYPE_SKYBOX, TYPE_CONTROLLER, TYPE_TRIGGER 
};
enum class ObjectState { STATE_IDLE, STATE_MOVE, STATE_DEATH, STATE_ATTACK, STATE_DAMAGED};

// 엔진 내 시스템을 제외한 모든 클래스가 기본상속할 추상 클래스
class CBaseObject
{
protected:
	// 해당 오브젝트의 정보
	ObjectType m_iType;
	ObjectLayer m_iLayer;
	std::string m_strName;
	CBaseObject* m_pParentObject;

public:
	CBaseObject( std::string strName, ObjectLayer iLayer, ObjectType iType )
		: m_iType(iType), m_iLayer(iLayer), m_strName(strName) {
		m_pParentObject = nullptr;
	};
	virtual ~CBaseObject( ){};

	void SetType( ObjectType type );
	void SetLayer( ObjectLayer layer );
	void SetName( std::string strName );
	ObjectType GetType( ) const;
	ObjectLayer GetLayer( ) const;
	std::string GetName( ) const;

	void SetParentObject(CBaseObject* pObject);
	CBaseObject* GetParentObject() const;
};

// 게임에서 기본적으로 사용되는 오브젝트, 기본적으로 움직임이 없는 고정형 객체
class CGameObject : public CBaseObject
{
protected:
	// 변환 행렬 
	XMFLOAT4X4 m_mtxWorld;
	float m_fPitch, m_fYaw, m_fRoll;
	float m_fXScale, m_fYScale, m_fZScale;

	// 렌더링 관련
	std::vector<CMesh *>m_vpMesh;
	int m_nMeshCount;

	// 객체 속성
	float m_fMoveSpeed;
	// 머테리얼
	CMaterial *m_pMaterial;

public:
	CGameObject( ID3D11Device* pd3dDevice, std::string strName, ObjectLayer iLayer, ObjectType iType );
	virtual ~CGameObject( );

	// 객체의 위치, 로컬 x,y,z축 방향 벡터를 반환
	XMFLOAT3 GetLookAt( ) const;
	XMFLOAT3 GetUp( ) const;
	XMFLOAT3 GetRight( ) const;
	XMFLOAT3 GetPosition( ) const;
	void SetPosition( XMFLOAT3& pos );
	void SetPosition( float x, float y, float z );
	
	// 행동 관련
	virtual void Update( float fTimeElapsed );
	virtual void Animate( float fTimeElapsed );
	virtual int Render( ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera );

	// 로컬 x,y,z 축 방향으로 회전
	virtual void Rotate( float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f );
	virtual void Rotate( XMFLOAT3 *vAxis, float fAngle );
	float GetPitch( ) const;
	float GetYaw( ) const;
	float GetRoll( ) const;

	// 로컬 x,y,z 축으로 비례
	virtual void Scale( float x, float y, float z );
	float GetXScale( ) const;
	float GetYScale( ) const;
	float GetZScale( ) const;

	// 메시 관련
	void SetMesh( int idx, CMesh* pMesh );
	void SetMesh( CMesh* pMesh );
	CMesh* GetMesh( int idx );
	CMesh* GetMesh( std::string strName );
	int GetMeshCount( ) const;

	// 머테리얼 관련
	void SetMaterial(CMaterial *pMaterial);
	CMaterial* GetMaterial() const;

	// 이동
	void Move( float fTimeElapsed, DWORD dwDirection );
	void Move( XMVECTOR vShift );

	bool CheckOOBBIntersect(CGameObject* pObject);
};

class CSkyboxObject : public CGameObject
{
public:
	CSkyboxObject( ID3D11Device* pd3dDevice, std::string strName, ObjectLayer iLayer, ObjectType iType );
	~CSkyboxObject( );

	virtual int Render( ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera );
	virtual void Update( float fTimeElapsed );
};