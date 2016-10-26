#pragma once
#include "GameObject.h"
class AABB;

// ī�޶�� ���� �� �� �� ����
class CCamera : public CBaseObject
{
protected:
	// �� ��ȯ ��İ� ���� ���� ��ȯ���( ����, ���� )
	XMFLOAT4X4 m_mtxView;
	XMFLOAT4X4 m_mtxProjection;
	XMFLOAT4X4 m_mtxOrtho;

	// ����Ʈ�� ��Ÿ���� ��� ����
	D3D11_VIEWPORT m_d3dViewport;

	// ī�޶��� ��ġ ����
	XMFLOAT3 m_vPosition;
	// ī�޶��� ���� x,y,z���� ��Ÿ���� ����
	XMFLOAT3 m_vRight;
	XMFLOAT3 m_vUp;
	XMFLOAT3 m_vLook;

	// ����ü�� 6�� ���
	XMFLOAT4 m_FrustumPlanes[6];

	// �� ī�޶� �ٶ󺸰� �ִ� ��ü
	CGameObject* m_pobjLookAtObject;
	// �ٶ󺸰� �ִ� ��ü���� ������
	XMFLOAT3 m_vOffset;

	// ī�޶� x,y,z������ �󸶸�ŭ ȸ���ߴ����� ��Ÿ���� ����
	float m_fPitch;
	float m_fRoll;
	float m_fYaw;
	
	// ����� ī�޶� �̵� ����
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

	// ����ü�� 6�� ��� ���
	void CalculateFrustumPlanes( );

	// �� ��Ʈ�� �����ϴ� ����Լ�
	void SetViewport( ID3D11DeviceContext *pd3dDeviceContext, DWORD xStart, DWORD yStart, DWORD nWidth, DWORD nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f );
	// ���� ���� ��ȯ ��� ����
	void GenerateProjectionMatrix( float nWidth, float nHeight, float fNearPlaneDist, float fFarPlaneDist, float fAspectRatio, float fFOVAngle );
	// ī�޶� ��ȯ����� ����
	void GenerateViewMatrix( );
	// ī�޶� ������ ȸ���ϸ� ������ �Ǽ������� ����Ȯ������ ���� ī�޶��� x,y,z���� ���� �������� ���� �� �ִ�
	// ���� �ٽ� �����ϵ��� ����� �Լ��� �����Ѵ�.
	void RegenerateViewMatrix( );

	// ī�޶� �ٶ󺸴� ������ �����ϴ� �Լ�, �ٶ󺸰� �ִ� ��ü�� �ٶ󺻴�
	virtual void SetLookAt( XMFLOAT3& vLookAt, XMFLOAT3& vUp );
	// ī�޶� �ٶ󺸴� ��ü�� �����ϴ� �Լ�
	void SetLookAtObject( CGameObject* pObject );

	// �ٿ�� �ڽ��� ����ü�� ������ ���Եǰų� �Ϻζ� ���ԵǴ��� �˻�
	bool IsInFrustum( XMFLOAT3& vMin, XMFLOAT3& vMax );
	bool IsInFrustum( AABB *pAABB );

	void Rotate( float fTimeElapsed, float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f );
	void Move( float fTimeElapsed, DWORD dwDirection );
	void Move( XMVECTOR& vShift );
	void Update( float fTimeElapsed );
};

