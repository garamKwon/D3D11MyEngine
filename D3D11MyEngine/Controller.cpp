#include "stdafx.h"
#include "Controller.h"
#include "Camera.h"


CController::CController( std::string strName, ObjectLayer iLayer, ObjectType iType ) : CBaseObject( strName, iLayer, iType )	
{
	m_keyVeluse.resize( 256 );

	m_keyVeluse[KEY_MOVE_BACKWARD] = 'S';
	m_keyVeluse[KEY_MOVE_FORWARD] = 'W';
	m_keyVeluse[KEY_MOVE_RIGHT] = 'D';
	m_keyVeluse[KEY_MOVE_LEFT] = 'A';
	m_keyVeluse[KEY_MOUSE_RIGHT] = VK_RBUTTON;
	m_keyVeluse[KEY_MOUSE_LEFT] = VK_LBUTTON;
	m_keyVeluse[KEY_MOVE_FORWARD1] = VK_UP;
	m_keyVeluse[KEY_MOVE_BACKWARD1] = VK_DOWN;
	m_keyVeluse[KEY_MOVE_RIGHT1] = VK_RIGHT;
	m_keyVeluse[KEY_MOVE_LEFT1] = VK_LEFT;
	m_keyVeluse[KEY_MOVE_UP] = 'Q';
	m_keyVeluse[KEY_MOVE_DOWN] = 'E';
	m_ptOldCursorPos.x = 0;
	m_ptOldCursorPos.y = 0;
}

CController::~CController( )
{
}

void CController::SetDirectionKey( UCHAR forwardKey, UCHAR backKey, UCHAR rightKey, UCHAR leftKey )
{
	m_keyVeluse[KEY_MOVE_BACKWARD] = backKey;
	m_keyVeluse[KEY_MOVE_FORWARD] = forwardKey;
	m_keyVeluse[KEY_MOVE_RIGHT] = rightKey;
	m_keyVeluse[KEY_MOVE_LEFT] = leftKey;
}

void CController::SetMouseKey( UCHAR right, UCHAR left )
{
	m_keyVeluse[KEY_MOUSE_RIGHT] = right;
	m_keyVeluse[KEY_MOUSE_LEFT] = left;
}

void CController::SetForwardKey( UCHAR key )
{
	m_keyVeluse[KEY_MOVE_FORWARD] = key;
}

void CController::SetBackwardKey( UCHAR key )
{
	m_keyVeluse[KEY_MOVE_BACKWARD] = key;
}

void CController::SetRightKey( UCHAR key )
{
	m_keyVeluse[KEY_MOVE_RIGHT] = key;
}

void CController::SetLeftKey( UCHAR key )
{
	m_keyVeluse[KEY_MOVE_LEFT] = key;
}

void CController::SetUpKey( UCHAR key )
{
	m_keyVeluse[KEY_MOVE_UP] = key;
}

void CController::SetDownKey( UCHAR key )
{
	m_keyVeluse[KEY_MOVE_DOWN] = key;
}

void CController::SetMenuKey( UCHAR key )
{
	m_keyVeluse[KEY_SYSTEM_MENU] = key;
}

void CController::SetJumpKey( UCHAR key )
{
	m_keyVeluse[KEY_MOVE_JUMP] = key;
}

void CController::Update( float fTimeElapsed, CCamera* pCamera )
{
	static UCHAR pKeyBuffer[256];
	DWORD dwDirection = 0;
	if (GetKeyboardState( pKeyBuffer ))
	{
		if (pKeyBuffer[m_keyVeluse[KEY_MOVE_FORWARD]] & 0xF0 || pKeyBuffer[m_keyVeluse[KEY_MOVE_FORWARD1]] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[m_keyVeluse[KEY_MOVE_BACKWARD]] & 0xF0 || pKeyBuffer[m_keyVeluse[KEY_MOVE_BACKWARD1]] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[m_keyVeluse[KEY_MOVE_LEFT]] & 0xF0 || pKeyBuffer[m_keyVeluse[KEY_MOVE_LEFT1]] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[m_keyVeluse[KEY_MOVE_RIGHT]] & 0xF0 || pKeyBuffer[m_keyVeluse[KEY_MOVE_RIGHT1]] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[m_keyVeluse[KEY_MOVE_UP]] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[m_keyVeluse[KEY_MOVE_DOWN]] & 0xF0) dwDirection |= DIR_DOWN;
	}

	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;

	if (GetCapture( ))
	{
		//SetCursor( NULL );
		GetCursorPos( &ptCursorPos );
		cxDelta = (float)( ptCursorPos.x - m_ptOldCursorPos.x ) / 5.0f;
		cyDelta = (float)( ptCursorPos.y - m_ptOldCursorPos.y ) / 5.0f;
		SetCursorPos( ptCursorPos.x, ptCursorPos.y );
		m_ptOldCursorPos = ptCursorPos;
	}
	else
		GetCursorPos(&m_ptOldCursorPos);

	if (cxDelta || cyDelta)
	{
		if (pKeyBuffer[m_keyVeluse[KEY_MOUSE_LEFT]] & 0xF0)
		{
			DWORD dwDir = 0;
			// 이동하기
			if (cxDelta > 0)
				dwDir |= DIR_FORWARD;
			else if (cxDelta < 0)
				dwDir |= DIR_BACKWARD;
			pCamera->Move( fTimeElapsed / 10.0f, dwDir );
		}
		if (pKeyBuffer[m_keyVeluse[KEY_MOUSE_RIGHT]] & 0xF0)
			pCamera->Rotate( cyDelta, cxDelta, 0.0f );
	}
	pCamera->Move( fTimeElapsed, dwDirection );
}

void CController::Update( float fTimeElapsed, CGameObject* pObject )
{
	static UCHAR pKeyBuffer[256];
	DWORD dwDirection = 0;
	if (GetKeyboardState( pKeyBuffer ))
	{
		if (pKeyBuffer[m_keyVeluse[KEY_MOVE_FORWARD]] & 0xF0 || pKeyBuffer[m_keyVeluse[KEY_MOVE_FORWARD1]] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[m_keyVeluse[KEY_MOVE_BACKWARD]] & 0xF0 || pKeyBuffer[m_keyVeluse[KEY_MOVE_BACKWARD1]] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[m_keyVeluse[KEY_MOVE_LEFT]] & 0xF0 || pKeyBuffer[m_keyVeluse[KEY_MOVE_LEFT1]] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[m_keyVeluse[KEY_MOVE_RIGHT]] & 0xF0 || pKeyBuffer[m_keyVeluse[KEY_MOVE_RIGHT1]] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[m_keyVeluse[KEY_MOVE_UP]] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[m_keyVeluse[KEY_MOVE_DOWN]] & 0xF0) dwDirection |= DIR_DOWN;
	}

	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT ptCursorPos;

	if (GetCapture( ))
	{
		SetCursor( NULL );
		GetCursorPos( &ptCursorPos );
		cxDelta = (float)( ptCursorPos.x - m_ptOldCursorPos.x ) / 5.0f;
		cyDelta = (float)( ptCursorPos.y - m_ptOldCursorPos.y ) / 5.0f;
		SetCursorPos( ptCursorPos.x, ptCursorPos.y );
		m_ptOldCursorPos = ptCursorPos;
	}
	if (cxDelta || cyDelta)
	{
		if (pKeyBuffer[m_keyVeluse[KEY_MOUSE_LEFT]] & 0xF0)
		{
			DWORD dwDir = 0;
			// 이동하기
			if (cxDelta > 0)
				dwDir |= DIR_FORWARD;
			else if (cxDelta < 0)
				dwDir |= DIR_BACKWARD;
			pObject->Move( fTimeElapsed / 10.0f, dwDir );
		}
		if (pKeyBuffer[m_keyVeluse[KEY_MOUSE_RIGHT]] & 0xF0)
			pObject->Rotate( cyDelta, 0.0f, -cxDelta );
	}
	pObject->Move( fTimeElapsed, dwDirection );
}