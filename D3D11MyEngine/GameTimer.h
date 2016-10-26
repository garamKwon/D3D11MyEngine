#pragma once

const ULONG MAX_SAMPLE_COUNT = 50;

class CGameTimer
{
private:
	bool m_bHardwareHasPerformanceCounter;
	float m_fTimeScale;
	float m_fTimeElapsed;
	__int64 m_nCurrentTime;
	__int64 m_nLastTime;
	__int64 m_PerformanceFrequency;

	float m_fFrameTime[MAX_SAMPLE_COUNT];
	ULONG m_nSampleCount;

	unsigned long m_nCurrentFrameRate;
	unsigned long m_FramePerSecond;
	float m_fFPSTimeElapsed;

public:
	CGameTimer( );
	virtual ~CGameTimer( );
	void Tick( float fLockFPS = 0.0f );
	unsigned long GetFrameRate( int nCharacters = 0 );
	float GetTimeElapsed( );
};