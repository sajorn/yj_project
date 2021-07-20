#pragma once
#include "framework.h"
#include "TimerManager.h"
#include <list>

class MainProgram
{
public:
	enum TimerType
	{
		Draw_Timer = 1,         // ����ͼ��Ķ�ʱ��
		Player_Jump = 2,        // ��ɫ��Ծʱ�ڿ���ͣ����ʱ��
		Check_Collide = 3       // ��ײ���
	};

	enum PlayerStatus
	{
		PlayerRun = 1,     // ��
		PlayerJump = 2,    // ��
		PlayerRoll = 3,    // ��
		PlayerDrop = 4,    // ����
		PlayerSlide = 5    // ����
	};
public:
	MainProgram(HWND hWnd = nullptr);
	~MainProgram();

public:
	void Begin();

	// ��������
	void KeyDown(WPARAM wParam);
	void KeyUp(WPARAM wParam);

private:
	void DoDraw();
	void DrawMap(HDC hDc);
	void DrawPlayer(HDC hDc);
	void DrawObject(HDC hDc);          // ���Ƴ����ϵĶ��󣨽�ҡ��ϰ��
	void DrawText(HDC hDc);            // ���Ʒ���

	void ChangeJumpStatus();           // �ı�������Ծ����

	void ResetConfig();
	void InitGold();                   // ��ʼ�����

	void CheckCollide();

private:
	HWND m_hWnd;                  // ��Ϸ���ھ��

	int m_drawPos;                // ��ǰ�ļ�¼�㣬��¼���ű���ͼƴ�ӵ�λ�ã�����ʵ�ֹ���Ч��

	// ������ر���
	PlayerStatus m_curPlayerStatus;     // ��¼��ǰ������״̬
	POINT m_curPlayerPos;               // ��¼��ǰ�����λ��

	int m_curPlayerRunIdx;              // ��ǰ�ܶ�״̬��ͼƬ����
	int m_curPlayerRollIdx;             // ��ǰ����״̬��ͼƬ����
	DWORD m_curTakeoffTime;               // ����˲���ʱ��

	bool m_isKeyPress;                  // �����Ƿ񱻰���

	std::list<POINT> m_goldList;        // ��¼��ҵ�λ��
	DWORD m_golds;                      // �����

	TimerManager m_timerManager;        // ��ʱ��������
};

