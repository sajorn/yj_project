#include "TimerManager.h"
#include <process.h>

void CheckTimerRun(void* p)
{
	TimerManager* pTimer = static_cast<TimerManager*>(p);
	if (pTimer == nullptr)
	{
		return;
	}

	while (true)
	{
		pTimer->CheckDelTimer();

		// 检查是否有定时器要运行
		if (!pTimer->IsTimerRun())
		{
			_endthread();
		}
		pTimer->TimerRun();
		Sleep(20);
	}
}

TimerManager::TimerManager()
{

}

TimerManager::~TimerManager()
{
	ClearTimer();
}

void TimerManager::AddTimer(unsigned id, unsigned interval, TimerCallBack callback)
{
	if (interval == 0)
	{
		return;
	}

	TimerParam timerParam;
	timerParam.m_timerId = id;
	timerParam.m_isDelete = false;
	timerParam.m_intervalTime = interval;
	timerParam.m_createTime = GetTickCount();
	timerParam.m_lastRunTime = GetTickCount();
	timerParam.m_callback = callback;

	m_timerList.push_back(timerParam);

	if (m_timerList.size() == 1)
	{
		// 当存在定时器时，开启线程调用
		_beginthread(CheckTimerRun, 0, this);
	}
}

void TimerManager::DeleteTimer(int id)
{
	for (auto& timer : m_timerList)
	{
		if (timer.m_timerId == id)
		{
			timer.m_isDelete = true;
			return;
		}
	}
}

bool TimerManager::IsTimerRun()
{
	if (m_timerList.size() == 0)
	{
		return false;
	}
	return true;
}

void TimerManager::ClearTimer()
{
	for (auto& timer : m_timerList)
	{
		timer.m_isDelete = true;
	}
}

void TimerManager::CheckDelTimer()
{
	for (auto iter = m_timerList.begin(); iter != m_timerList.end();)
	{
		if (iter->m_isDelete)
		{
			m_timerList.erase(iter);
			iter = m_timerList.begin();
			continue;
		}
		++iter;
	}
}

void TimerManager::TimerRun()
{
	unsigned curTime = GetTickCount();
	for (auto& timer : m_timerList)
	{
		if (curTime - timer.m_lastRunTime >= timer.m_intervalTime)
		{
			// 达到时间间隔，执行
			timer.m_lastRunTime = curTime;
			timer.m_callback();
		}
	}
}
