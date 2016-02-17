#pragma once

#if defined(WIN32) || defined(_WIN32)
#include <wrl.h>
#endif

#include <exception>

const int R_NUM_FRAMES_TO_MIDDLE = 32;

#if defined(WIN32) || defined(_WIN32)
class RTimer
{
private:
	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_currentTime;
	LARGE_INTEGER m_startTime;
	LARGE_INTEGER m_lastTime;
	double m_total;
	double m_delta[R_NUM_FRAMES_TO_MIDDLE];
	double m_deltaAvg;
	double m_deltaAccurate;
	int currentMidFrame;

public:
	RTimer()
	{
		if (!QueryPerformanceFrequency(&m_frequency))
		{
			throw std::exception();
		}

		currentMidFrame = 0;

		Reset();
	}

	void Reset()
	{

		m_total = 0;

		for(int i=0; i<R_NUM_FRAMES_TO_MIDDLE; i++)
		{
			m_delta[i] = 1.0f / 60.0f;
			m_deltaAccurate = 1.0f / 60.0f;
		}

		for(int i=0; i<R_NUM_FRAMES_TO_MIDDLE; i++)
		{
			Update();
		}

		m_startTime = m_currentTime;
	}

	void Update()
	{
		currentMidFrame ++;
		if(currentMidFrame == R_NUM_FRAMES_TO_MIDDLE )
		{
			currentMidFrame = 0;
		}


		if (!QueryPerformanceCounter(&m_currentTime))
		{
			throw std::exception();
		}

		m_total = static_cast<double>(
			static_cast<double>(m_currentTime.QuadPart-m_startTime.QuadPart) /
			static_cast<double>(m_frequency.QuadPart)
			);

		if (m_lastTime.QuadPart==m_startTime.QuadPart)
		{
			// If the timer was just reset, report a time delta equivalent to 60Hz frame time.
			m_delta[currentMidFrame] = 1.0f / 60.0f;
			m_deltaAccurate = 1.0f / 60.0f;
		}
		else
		{
			double d = static_cast<double>(
				static_cast<double>(m_currentTime.QuadPart-m_lastTime.QuadPart) /
				static_cast<double>(m_frequency.QuadPart)
				);


			//for(int i=0; i<NUM_FRAMES_TO_MIDDLE; i++)
			m_delta[currentMidFrame] = d;
			m_deltaAccurate = d;

		}

		m_deltaAvg = 0;
		for(int i=0; i<R_NUM_FRAMES_TO_MIDDLE; i++)
			m_deltaAvg += m_delta[i];

		m_deltaAvg /= (double)R_NUM_FRAMES_TO_MIDDLE;

		m_lastTime = m_currentTime;
	}



	double GetTotal() 
	{  
		return m_total;
	}

	void SetTotal(double t) 
	{  
		m_total=t;
	}

	double GetDelta() 
	{ 
		//return m_delta[0];


		return m_deltaAvg;
	}

	double GetDeltaAccurate()
	{
		return m_deltaAccurate;
	}

};
#else
class RTimer
{
private:
	double m_total;
	double m_delta[R_NUM_FRAMES_TO_MIDDLE];
	double m_deltaAvg;
	double m_deltaAccurate;
	int currentMidFrame;

public:
	RTimer()
	{

	}

	void Reset()
	{

	}

	void Update()
	{

	}

	double GetTotal()
	{
		return m_total;
	}

	void SetTotal(double t)
	{
		m_total=t;
	}

	double GetDelta()
	{
		return m_deltaAvg;
	}

	double GetDeltaAccurate()
	{
		return m_deltaAccurate;
	}

};
#endif