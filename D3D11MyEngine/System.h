#pragma once

// 시스템들의 추상클래스
class CSystem
{
protected:
	bool m_bUse;

public:
	CSystem( );
	virtual ~CSystem( );
};