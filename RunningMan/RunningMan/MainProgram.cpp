#include "MainProgram.h"
#include "config.h"
#pragma comment(lib, "Msimg32.lib")

// TODO:
// 金币生成方式
// 修改金币轨迹
// 障碍物（怪兽）
// 道具：无敌、冲刺
// BGM
// 游戏结束

MainProgram::MainProgram(HWND hWnd) : m_hWnd(hWnd)
{
	ResetConfig();
}

MainProgram::~MainProgram()
{

}

void MainProgram::Begin()
{
	ResetConfig();

	m_timerManager.AddTimer(Draw_Timer, 50, std::bind(&MainProgram::DoDraw, this));               // 增加绘制定时器，每50ms绘制一次
	m_timerManager.AddTimer(Check_Collide, 50, std::bind(&MainProgram::CheckCollide, this));      // 50ms检测一次人物和对象的碰撞状态     
}

void MainProgram::DoDraw()
{
	// 获取窗体DC
	HDC hDc = GetWindowDC(m_hWnd);
	if (hDc == NULL)
	{
		return;
	}

	// 使用双缓存：即先在其中一个缓存上进行绘制，然后再一次性绘制到另一个缓存上
	// 这样做的好处是防止高频率刷新时画面闪烁的现象
	HDC hMemoryDC = CreateCompatibleDC(NULL);                                                       // 创建缓存DC
	HBITMAP hTempBitmap = CreateCompatibleBitmap(hDc, BACKGROUND_WIDTH, BACKGROUND_HEIGHT);         // 从当前的dc中创建临时位图
	SelectObject(hMemoryDC, hTempBitmap);

	// 把所有内容绘制到缓存DC上
	DrawMap(hMemoryDC);
	DrawPlayer(hMemoryDC);
	DrawObject(hMemoryDC);
	DrawText(hMemoryDC);

	BitBlt(hDc, 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, hMemoryDC, 0, 0, SRCCOPY);
	
	ReleaseDC(m_hWnd, hDc);
	DeleteDC(hMemoryDC);
	DeleteObject(hTempBitmap);
}

void MainProgram::DrawMap(HDC hDc)
{
	HDC hTempDc = CreateCompatibleDC(hDc);
	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, _T("images\\bg.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);      // 将图片加载进位图中
	SelectObject(hTempDc, hBitmap);

	/*
	 * 为实现滚动效果，需要绘制两张背景图
	 * 第一张背景图宽度从记录点开始，到背景图的右边界
	 * 第二张背景图宽度从背景图左边界开始，到记录点
	 * 每绘制完一次后，记录点向左移动30像素
	 */
	BitBlt(hDc, 0, 0, BACKGROUND_WIDTH - m_drawPos, BACKGROUND_HEIGHT, hTempDc, m_drawPos, 0, SRCCOPY);      // 第一张背景图
	BitBlt(hDc, BACKGROUND_WIDTH - m_drawPos, 0, m_drawPos, BACKGROUND_HEIGHT, hTempDc, 0, 0, SRCCOPY);      // 第二张背景图

	m_drawPos = (m_drawPos + 30) % BACKGROUND_WIDTH;

	DeleteDC(hTempDc);
	DeleteObject(hBitmap);
}

void MainProgram::DrawPlayer(HDC hDc)
{
	HDC hTempDc = CreateCompatibleDC(hDc);

	wchar_t filePath[256]{ 0 };
	if (m_curPlayerStatus == PlayerRun)
	{
		// 跑动的动画是通过不断的循环跑动的图片实现
		m_curPlayerRunIdx = (m_curPlayerRunIdx + 1) % 6;
		wsprintf(filePath, _T("images\\playerRun%d.bmp"), m_curPlayerRunIdx);
	}
	else if (m_curPlayerStatus == PlayerJump)
	{
		// 跳跃原理：竖直上抛运动
		// 1. 最大高度：H = (v*v) / (2*g)，目前假设跳跃的最大高度是120px     （1）
		// 2. 上抛最大高度时所用的时间：T = v/g，目前设置起跳到最高时是0.4s   （2）
		// 3. 由(1)和(2)可以算出，g = (2*H / T*T) = 1500，v = gT = 600
		// 4. 因此可以得出某一时刻上抛位移：s = v*t - g*t*t / 2，t是此时经历的时间
		// 5. 最后得出人物某时刻的位置：人物初始位置-某时刻上抛的位移
		wsprintf(filePath, _T("images\\playerJump.bmp"));
		auto passTime = (GetTickCount() - m_curTakeoffTime) / 1000.0;
		int h = INIT_SPEED * passTime - PLAYER_GRAVITY * passTime * passTime / 2.0;
		m_curPlayerPos.y = PLAYER_POS_Y - h;
	}
	else if (m_curPlayerStatus == PlayerRoll)
	{
		m_curPlayerRollIdx = (m_curPlayerRollIdx + 1) % 3;
		wsprintf(filePath, _T("images\\playerRoll%d.bmp"), m_curPlayerRollIdx);
	}
	else if (m_curPlayerStatus == PlayerDrop)
	{
		wsprintf(filePath, _T("images\\playerFall.bmp"));

		// m_curTakeoffTime是记录起跳时间
		// 起跳到翻滚耗时400ms
		// 翻滚到下落耗时400ms
		// 因此经过的时间应该为：此时的时间减去起跳到下落开始前的800ms
		auto passTime = (GetTickCount() - m_curTakeoffTime - 800) / 1000.0;

		// 下落原理：自由落体运动
		// 某时刻下落的位移：h=g*t*t/2
		// 因此人物此时的位置：初始位置-最大高度+某时刻下落的位移
		int h = PLAYER_GRAVITY * passTime * passTime / 2.0;
		m_curPlayerPos.y = PLAYER_POS_Y - JUMP_MAX_HEIGHT + h;
	}
	else if (m_curPlayerStatus == PlayerSlide)
	{
		wsprintf(filePath, _T("images\\playerSlip.bmp"));
	}

	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, filePath, IMAGE_BITMAP, PLAYER_WIDTH, PLAYER_HEIGHT, LR_LOADFROMFILE);
	SelectObject(hTempDc, hBitmap);

	TransparentBlt(hDc, m_curPlayerPos.x, m_curPlayerPos.y, PLAYER_WIDTH, PLAYER_HEIGHT, hTempDc, 0, 0, PLAYER_WIDTH, PLAYER_HEIGHT, RGB(34, 177, 76));

	DeleteDC(hTempDc);
	DeleteObject(hBitmap);
}

void MainProgram::ResetConfig()
{
	m_drawPos = 0;
	m_curPlayerStatus = PlayerRun;
	m_curPlayerRunIdx = 0;
	m_curPlayerRollIdx = 0;
	m_curPlayerPos.x = PLAYER_POS_X;
	m_curPlayerPos.y = PLAYER_POS_Y;
	m_curTakeoffTime = 0;
	
	m_goldList.clear();
	m_golds = 0;

	m_isKeyPress = false;

	InitGold();
}

void MainProgram::KeyDown(WPARAM wParam)
{
	if (m_isKeyPress)
	{
		return;
	}

	if (m_curPlayerStatus == PlayerJump || m_curPlayerStatus == PlayerRoll || m_curPlayerStatus == PlayerDrop)
	{
		// 跳起，滚动，下落的过程中不触发
		// 这3个连贯起来完成一个跳跃动作
		return;
	}

	if (wParam == VK_UP)
	{
		// 方向键上
		m_curPlayerStatus = PlayerJump;
		m_curTakeoffTime = GetTickCount();
		m_timerManager.AddTimer(Player_Jump, 400, std::bind(&MainProgram::ChangeJumpStatus, this));   // 增加定时器，用于绘制角色跳起来的时候在空中停留的时间
	}
	if (wParam == VK_DOWN)
	{
		// 方向键下
		m_curPlayerStatus = PlayerSlide;
	}
	m_isKeyPress = true;
}

void MainProgram::KeyUp(WPARAM wParam)
{
	if (wParam != VK_UP && wParam != VK_DOWN)
	{
		return;
	}

	// 松开下滑键的时候，恢复跑步状态
	if (m_curPlayerStatus == PlayerSlide)
	{
		m_curPlayerStatus = PlayerRun;
	}

	m_isKeyPress = false;
}

void MainProgram::ChangeJumpStatus()
{
	if (m_curPlayerStatus == PlayerJump)
	{
		m_curPlayerStatus = PlayerRoll;
	}
	else if (m_curPlayerStatus == PlayerRoll)
	{
		m_curPlayerStatus = PlayerDrop;
	}
	else if (m_curPlayerStatus == PlayerDrop)
	{
		// 此时完成了一个完整的跳跃动作，删除跳跃定时器
		m_curPlayerStatus = PlayerRun;
		m_curPlayerPos.y = PLAYER_POS_Y;
		m_timerManager.DeleteTimer(Player_Jump);
	}
}

void MainProgram::DrawObject(HDC hDc)
{
	// 绘制金币
	HDC hTempDC = CreateCompatibleDC(hDc);
	HBITMAP hGoldBitmap = (HBITMAP)LoadImage(NULL, _T("images\\gold.bmp"), IMAGE_BITMAP, 69, 64, LR_LOADFROMFILE);
	SelectObject(hTempDC, hGoldBitmap);

	for (auto iter = m_goldList.begin(); iter != m_goldList.end(); )
	{
		iter->x -= 30;
		if (iter->x > 2000)
		{
			// 超出屏幕右边太多则不再绘制
			iter++;
			continue;
		}

		TransparentBlt(hDc, iter->x, iter->y, 69, 64, hTempDC, 0, 0, 69, 64, RGB(34, 177, 76));
		if (iter->x <= -69)
		{
			// 跑到屏幕左边
			iter = m_goldList.erase(iter);
			continue;
		}
		++iter;
	}

	DeleteDC(hTempDC);
	DeleteObject(hGoldBitmap);
}

void MainProgram::InitGold()
{
	if (m_goldList.size() > 0)
	{
		m_goldList.clear();
	}
	// 初始化100个金币
	for (int i(0); i < 100; ++i)
	{
		POINT p;
		p.x = 960 + i * 150;
		p.y = 160;               // TODO: 弄成跳跃轨迹
		m_goldList.push_back(p);
	}
}

void MainProgram::DrawText(HDC hDc)
{
	SetBkMode(hDc, 0);
	SetTextColor(hDc, RGB(255, 204, 14));

	wchar_t strText[100]{ 0 };
	wsprintf(strText, _T("金币:%d"), m_golds);
	TextOut(hDc, 400, 50, strText, lstrlen(strText));
}

void MainProgram::CheckCollide()
{
	// 碰撞检测原理：
	// 1.计算人物图像的中点cenPt1和金币图像的中点cenPt2
	// 2.两者中点的距离如果小于它们图像的一半的和，则表示两者碰撞

	POINT goldHalfPos{ GOLD_WIDTH / 2, GOLD_HEIGHT / 2 };
	POINT playerHalfPos{ PLAYER_WIDTH / 2, PLAYER_HEIGHT / 2 };

	for (auto iter = m_goldList.begin(); iter != m_goldList.end(); )
	{
		POINT goldCenterPoint{ iter->x + goldHalfPos.x, iter->y + goldHalfPos.y };
		POINT playerCenterPoint{ m_curPlayerPos.x + playerHalfPos.x, m_curPlayerPos.y + playerHalfPos.y };

		if (abs(goldCenterPoint.x - playerCenterPoint.x) <= goldHalfPos.x + playerHalfPos.x - 10
			&& abs(goldCenterPoint.y - playerCenterPoint.y) <= goldHalfPos.y + playerHalfPos.y - 20)
		{
			// 设置部分误差值，x是10，y是20
			// 金币被吃，删除
			iter = m_goldList.erase(iter);
			m_golds += 100;
			continue;
		}
		++iter;  
	}
}