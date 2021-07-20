#pragma once
#include "framework.h"
#include "TimerManager.h"
#include <list>

class MainProgram
{
public:
	enum TimerType
	{
		Draw_Timer = 1,         // 绘制图像的定时器
		Player_Jump = 2,        // 角色跳跃时在空中停留的时间
		Check_Collide = 3       // 碰撞检测
	};

	enum PlayerStatus
	{
		PlayerRun = 1,     // 跑
		PlayerJump = 2,    // 跳
		PlayerRoll = 3,    // 滚
		PlayerDrop = 4,    // 下落
		PlayerSlide = 5    // 滑铲
	};
public:
	MainProgram(HWND hWnd = nullptr);
	~MainProgram();

public:
	void Begin();

	// 按键处理
	void KeyDown(WPARAM wParam);
	void KeyUp(WPARAM wParam);

private:
	void DoDraw();
	void DrawMap(HDC hDc);
	void DrawPlayer(HDC hDc);
	void DrawObject(HDC hDc);          // 绘制场景上的对象（金币、障碍物）
	void DrawText(HDC hDc);            // 绘制分数

	void ChangeJumpStatus();           // 改变人物跳跃动作

	void ResetConfig();
	void InitGold();                   // 初始化金币

	void CheckCollide();

private:
	HWND m_hWnd;                  // 游戏窗口句柄

	int m_drawPos;                // 当前的记录点，记录两张背景图拼接的位置，用于实现滚动效果

	// 人物相关变量
	PlayerStatus m_curPlayerStatus;     // 记录当前的人物状态
	POINT m_curPlayerPos;               // 记录当前人物的位置

	int m_curPlayerRunIdx;              // 当前跑动状态的图片索引
	int m_curPlayerRollIdx;             // 当前滚动状态的图片索引
	DWORD m_curTakeoffTime;               // 起跳瞬间的时间

	bool m_isKeyPress;                  // 按键是否被按下

	std::list<POINT> m_goldList;        // 记录金币的位置
	DWORD m_golds;                      // 金币数

	TimerManager m_timerManager;        // 定时器管理器
};

