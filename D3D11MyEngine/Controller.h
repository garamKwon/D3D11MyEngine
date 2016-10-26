#pragma once
#include "GameObject.h"

enum KeyValue{
	KEY_MOVE_FORWARD, KEY_MOVE_BACKWARD, KEY_MOVE_RIGHT, KEY_MOVE_LEFT, KEY_MOVE_JUMP, KEY_MOVE_DOWN, KEY_MOVE_UP,
	KEY_MOVE_FORWARD1, KEY_MOVE_BACKWARD1, KEY_MOVE_RIGHT1, KEY_MOVE_LEFT1, 
	KEY_MOUSE_RIGHT, KEY_MOUSE_LEFT,
	KEY_SYSTEM_MENU, };

class CController : public CBaseObject
{
	std::vector<UCHAR> m_keyVeluse;
	// 마우스 이전 좌표
	POINT m_ptOldCursorPos;
public:
	CController( std::string strName, ObjectLayer iLayer = ObjectLayer::LAYER_CONTROLLER, ObjectType iType = ObjectType::TYPE_CONTROLLER );
	~CController( );

	void SetDirectionKey( UCHAR forwardKey, UCHAR backKey, UCHAR rightKey, UCHAR leftKey );
	void SetMouseKey( UCHAR right, UCHAR left );
	void SetForwardKey( UCHAR key );
	void SetBackwardKey( UCHAR key );
	void SetRightKey( UCHAR key );
	void SetLeftKey( UCHAR key );
	void SetUpKey( UCHAR key );
	void SetDownKey( UCHAR key );
	void SetMenuKey( UCHAR key );
	void SetJumpKey( UCHAR key );

	// 지속적으로 키 입력을 받을 업데이트 함수
	virtual void Update( float fTimeElapsed, CCamera* pCamera );
	virtual void Update( float fTimeElapsed, CGameObject* pObject );
};

