#pragma once
#include "GameObject.h"
class AABB;

// 카메라는 여러 개 일 수 있음
class CCamera : public CBaseObject
{
protected:
	// 뷰 변환 행렬과 절단 공간 변환행렬( 투영, 직교 )
	XMFLOAT4X4 m_mtxView;
	XMFLOAT4X4 m_mtxProjection;
	XMFLOAT4X4 m_mtxOrtho;

	// 뷰포트를 나타내는 멤버 변수
	D3D11_VIEWPORT m_d3dViewport;

	// 카메라의 위치 벡터
	XMFLOAT3 m_vPosition;
	// 카메라의 로컬 x,y,z축을 나타내는 벡터
	XMFLOAT3 m_vRight;
	XMFLOAT3 m_vUp;
	XMFLOAT3 m_vLook;

	// 절두체의 6개 평면
	XMFLOAT4 m_FrustumPlanes[6];

	// 이 카메라가 바라보고 있는 객체
	CGameObject* m_pobjLookAtObject;
	// 바라보고 있는 객체와의 오프셋
	XMFLOAT3 m_vOffset;

	// 카메라가 x,y,z축으로 얼마만큼 회전했는지를 나타내는 각도
	float m_fPitch;
	float m_fRoll;
	float m_fYaw;
	
	// 디버그 카메라 이동 관련
	float m_fMoveSpeed;

public:
	CCamera( ID3D11DeviceContext *pd3dDeviceContext, std::string strName, ObjectLayer iLayer, ObjectType iType,
		XMFLOAT3 vPosition = XMFLOAT3( 0, 0, 0 ), XMFLOAT3 vLook = XMFLOAT3( 0, 0, 1 ), XMFLOAT3 vRight = XMFLOAT3( 1, 0, 0 ), XMFLOAT3 vUp = XMFLOAT3( 0, 1, 0 ), XMFLOAT3 vOffset = XMFLOAT3( 0, 0, 0 ), CGameObject* objLook = nullptr,
		DWORD xStart = 0, DWORD yStart = 0, DWORD nWidth = 640, DWORD nHeight = 480, float fMinZ = 0.0f, float fMaxZ = 1.0f,
		float fNearPlaneDist = 1.01f, float fFarPlaneDist = 50000.0f, float fAspectRatio = 640.0f/480.0f, float fFOVAngle = 60.0f );
	~CCamera( );

	D3D11_VIEWPORT GetViewport( ) const;
	XMFLOAT4X4 GetViewMatrix( ) const;
	XMFLOAT4X4 GetProjectionMatrix( ) const;
	XMFLOAT4X4 GetOrthMatrix( ) const;

	XMFLOAT3 GetRightVector( ) const;
	XMFLOAT3 GetUpVector( ) const;
	XMFLOAT3 GetLookVector( ) const;

	CGameObject* GetLookAtObject( ) const;

	XMFLOAT3 GetPosition( )const;
	void SetPosition( XMFLOAT3 pos );
	XMFLOAT3 GetOffset( ) const;
	void SetOffset( XMFLOAT3 offset );

	float& GetPitch( ) { return m_fPitch; }
	float& GetRoll( ) { return m_fRoll; }
	float& GetYaw( ) { return m_fYaw; }

	void SetMoveSpeed( float speed );

	// 절두체의 6개 평면 계산
	void CalculateFrustumPlanes( );

	// 뷰 포트를 설정하는 멤버함수
	void SetViewport( ID3D11DeviceContext *pd3dDeviceContext, DWORD xStart, DWORD yStart, DWORD nWidth, DWORD nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f );
	// 절단 공간 변환 행렬 생성
	void GenerateProjectionMatrix( float nWidth, float nHeight, float fNearPlaneDist, float fFarPlaneDist, float fAspectRatio, float fFOVAngle );
	// 카메라 변환행렬을 생성
	void GenerateViewMatrix( );
	// 카메라가 여러번 회전하면 누적된 실수연산의 부적확성으로 인해 카메라의 x,y,z축이 서로 직교하지 않을 수 있다
	// 따라서 다시 직교하도록 만드는 함수를 구현한다.
	void RegenerateViewMatrix( );

	// 카메라가 바라보는 지점을 설정하는 함수, 바라보고 있는 객체를 바라본다
	virtual void SetLookAt( XMFLOAT3& vLookAt, XMFLOAT3& vUp );
	// 카메라가 바라보는 객체를 설정하는 함수
	void SetLookAtObject( CGameObject* pObject );

	// 바운딩 박스가 절두체에 완전히 포함되거나 일부라도 포함되는지 검사
	bool IsInFrustum( XMFLOAT3& vMin, XMFLOAT3& vMax );
	bool IsInFrustum( AABB *pAABB );

	void Rotate( float fTimeElapsed, float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f );
	void Move( float fTimeElapsed, DWORD dwDirection );
	void Move( XMVECTOR& vShift );
	void Update( float fTimeElapsed );
};

