#pragma once
#include "framework.h"
#include <string>
#include <list>
#include <functional>

// 定时器回调函数
using TimerCallBack = std::function<void()>;

/*
 * 定时器相关参数
 */
struct TimerParam
{
	unsigned m_intervalTime{ 0 };             // 定时器间隔运行时间
	unsigned m_createTime{ 0 };               // 定时器创建时间
	unsigned m_lastRunTime{ 0 };              // 定时器上次执行
	int m_timerId{ -1 };                      // 定时器ID
	bool m_isDelete{ false };                 // 是否被删除

	TimerCallBack m_callback;                   // 回调函数
};
using timerList = std::list<TimerParam>;
using iterTimerList = timerList::iterator;

/*
 * 定时器管理类
 */
class TimerManager final
{
public:
	TimerManager();
	~TimerManager();

public:
	/*!
	 * \brief:  添加定时器
	 *
	 * \param: id 定时器id
	 * \param: ms 触发间隔（毫秒）
	 * \param: callback 回调函数
	 *
	 * \returns: void
	 */
	void AddTimer(unsigned id, unsigned interval, TimerCallBack callback);

	/*!
	 * \brief:  删除定时器
	 * \param: id 定时器id
	 * \returns: void
	 */
	void DeleteTimer(int id);

	/*!
	 * \brief:  检测定时器运行
	 * \returns: bool 定时器运行返回true
	 */
	bool IsTimerRun();

	/*!
	 * \brief:  清空所有的定时器
	 */
	void ClearTimer();

	/*!
	 * \brief:  定时检查删除的定时器
	 */
	void CheckDelTimer();

	/*!
	 * \brief:  定时器运行
	 */
	void TimerRun();

private:
	timerList m_timerList;             // 定时器列表
};

