#pragma once
#include "framework.h"
#include <string>
#include <list>
#include <functional>

// ��ʱ���ص�����
using TimerCallBack = std::function<void()>;

/*
 * ��ʱ����ز���
 */
struct TimerParam
{
	unsigned m_intervalTime{ 0 };             // ��ʱ���������ʱ��
	unsigned m_createTime{ 0 };               // ��ʱ������ʱ��
	unsigned m_lastRunTime{ 0 };              // ��ʱ���ϴ�ִ��
	int m_timerId{ -1 };                      // ��ʱ��ID
	bool m_isDelete{ false };                 // �Ƿ�ɾ��

	TimerCallBack m_callback;                   // �ص�����
};
using timerList = std::list<TimerParam>;
using iterTimerList = timerList::iterator;

/*
 * ��ʱ��������
 */
class TimerManager final
{
public:
	TimerManager();
	~TimerManager();

public:
	/*!
	 * \brief:  ��Ӷ�ʱ��
	 *
	 * \param: id ��ʱ��id
	 * \param: ms ������������룩
	 * \param: callback �ص�����
	 *
	 * \returns: void
	 */
	void AddTimer(unsigned id, unsigned interval, TimerCallBack callback);

	/*!
	 * \brief:  ɾ����ʱ��
	 * \param: id ��ʱ��id
	 * \returns: void
	 */
	void DeleteTimer(int id);

	/*!
	 * \brief:  ��ⶨʱ������
	 * \returns: bool ��ʱ�����з���true
	 */
	bool IsTimerRun();

	/*!
	 * \brief:  ������еĶ�ʱ��
	 */
	void ClearTimer();

	/*!
	 * \brief:  ��ʱ���ɾ���Ķ�ʱ��
	 */
	void CheckDelTimer();

	/*!
	 * \brief:  ��ʱ������
	 */
	void TimerRun();

private:
	timerList m_timerList;             // ��ʱ���б�
};

