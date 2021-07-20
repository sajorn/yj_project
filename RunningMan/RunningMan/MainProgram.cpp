#include "MainProgram.h"
#include "config.h"
#pragma comment(lib, "Msimg32.lib")

// TODO:
// ������ɷ�ʽ
// �޸Ľ�ҹ켣
// �ϰ�����ޣ�
// ���ߣ��޵С����
// BGM
// ��Ϸ����

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

	m_timerManager.AddTimer(Draw_Timer, 50, std::bind(&MainProgram::DoDraw, this));               // ���ӻ��ƶ�ʱ����ÿ50ms����һ��
	m_timerManager.AddTimer(Check_Collide, 50, std::bind(&MainProgram::CheckCollide, this));      // 50ms���һ������Ͷ������ײ״̬     
}

void MainProgram::DoDraw()
{
	// ��ȡ����DC
	HDC hDc = GetWindowDC(m_hWnd);
	if (hDc == NULL)
	{
		return;
	}

	// ʹ��˫���棺����������һ�������Ͻ��л��ƣ�Ȼ����һ���Ի��Ƶ���һ��������
	// �������ĺô��Ƿ�ֹ��Ƶ��ˢ��ʱ������˸������
	HDC hMemoryDC = CreateCompatibleDC(NULL);                                                       // ��������DC
	HBITMAP hTempBitmap = CreateCompatibleBitmap(hDc, BACKGROUND_WIDTH, BACKGROUND_HEIGHT);         // �ӵ�ǰ��dc�д�����ʱλͼ
	SelectObject(hMemoryDC, hTempBitmap);

	// ���������ݻ��Ƶ�����DC��
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
	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, _T("images\\bg.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);      // ��ͼƬ���ؽ�λͼ��
	SelectObject(hTempDc, hBitmap);

	/*
	 * Ϊʵ�ֹ���Ч������Ҫ�������ű���ͼ
	 * ��һ�ű���ͼ��ȴӼ�¼�㿪ʼ��������ͼ���ұ߽�
	 * �ڶ��ű���ͼ��ȴӱ���ͼ��߽翪ʼ������¼��
	 * ÿ������һ�κ󣬼�¼�������ƶ�30����
	 */
	BitBlt(hDc, 0, 0, BACKGROUND_WIDTH - m_drawPos, BACKGROUND_HEIGHT, hTempDc, m_drawPos, 0, SRCCOPY);      // ��һ�ű���ͼ
	BitBlt(hDc, BACKGROUND_WIDTH - m_drawPos, 0, m_drawPos, BACKGROUND_HEIGHT, hTempDc, 0, 0, SRCCOPY);      // �ڶ��ű���ͼ

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
		// �ܶ��Ķ�����ͨ�����ϵ�ѭ���ܶ���ͼƬʵ��
		m_curPlayerRunIdx = (m_curPlayerRunIdx + 1) % 6;
		wsprintf(filePath, _T("images\\playerRun%d.bmp"), m_curPlayerRunIdx);
	}
	else if (m_curPlayerStatus == PlayerJump)
	{
		// ��Ծԭ����ֱ�����˶�
		// 1. ���߶ȣ�H = (v*v) / (2*g)��Ŀǰ������Ծ�����߶���120px     ��1��
		// 2. �������߶�ʱ���õ�ʱ�䣺T = v/g��Ŀǰ�������������ʱ��0.4s   ��2��
		// 3. ��(1)��(2)���������g = (2*H / T*T) = 1500��v = gT = 600
		// 4. ��˿��Եó�ĳһʱ������λ�ƣ�s = v*t - g*t*t / 2��t�Ǵ�ʱ������ʱ��
		// 5. ���ó�����ĳʱ�̵�λ�ã������ʼλ��-ĳʱ�����׵�λ��
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

		// m_curTakeoffTime�Ǽ�¼����ʱ��
		// ������������ʱ400ms
		// �����������ʱ400ms
		// ��˾�����ʱ��Ӧ��Ϊ����ʱ��ʱ���ȥ���������俪ʼǰ��800ms
		auto passTime = (GetTickCount() - m_curTakeoffTime - 800) / 1000.0;

		// ����ԭ�����������˶�
		// ĳʱ�������λ�ƣ�h=g*t*t/2
		// ��������ʱ��λ�ã���ʼλ��-���߶�+ĳʱ�������λ��
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
		// ���𣬹���������Ĺ����в�����
		// ��3�������������һ����Ծ����
		return;
	}

	if (wParam == VK_UP)
	{
		// �������
		m_curPlayerStatus = PlayerJump;
		m_curTakeoffTime = GetTickCount();
		m_timerManager.AddTimer(Player_Jump, 400, std::bind(&MainProgram::ChangeJumpStatus, this));   // ���Ӷ�ʱ�������ڻ��ƽ�ɫ��������ʱ���ڿ���ͣ����ʱ��
	}
	if (wParam == VK_DOWN)
	{
		// �������
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

	// �ɿ��»�����ʱ�򣬻ָ��ܲ�״̬
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
		// ��ʱ�����һ����������Ծ������ɾ����Ծ��ʱ��
		m_curPlayerStatus = PlayerRun;
		m_curPlayerPos.y = PLAYER_POS_Y;
		m_timerManager.DeleteTimer(Player_Jump);
	}
}

void MainProgram::DrawObject(HDC hDc)
{
	// ���ƽ��
	HDC hTempDC = CreateCompatibleDC(hDc);
	HBITMAP hGoldBitmap = (HBITMAP)LoadImage(NULL, _T("images\\gold.bmp"), IMAGE_BITMAP, 69, 64, LR_LOADFROMFILE);
	SelectObject(hTempDC, hGoldBitmap);

	for (auto iter = m_goldList.begin(); iter != m_goldList.end(); )
	{
		iter->x -= 30;
		if (iter->x > 2000)
		{
			// ������Ļ�ұ�̫�����ٻ���
			iter++;
			continue;
		}

		TransparentBlt(hDc, iter->x, iter->y, 69, 64, hTempDC, 0, 0, 69, 64, RGB(34, 177, 76));
		if (iter->x <= -69)
		{
			// �ܵ���Ļ���
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
	// ��ʼ��100�����
	for (int i(0); i < 100; ++i)
	{
		POINT p;
		p.x = 960 + i * 150;
		p.y = 160;               // TODO: Ū����Ծ�켣
		m_goldList.push_back(p);
	}
}

void MainProgram::DrawText(HDC hDc)
{
	SetBkMode(hDc, 0);
	SetTextColor(hDc, RGB(255, 204, 14));

	wchar_t strText[100]{ 0 };
	wsprintf(strText, _T("���:%d"), m_golds);
	TextOut(hDc, 400, 50, strText, lstrlen(strText));
}

void MainProgram::CheckCollide()
{
	// ��ײ���ԭ��
	// 1.��������ͼ����е�cenPt1�ͽ��ͼ����е�cenPt2
	// 2.�����е�ľ������С������ͼ���һ��ĺͣ����ʾ������ײ

	POINT goldHalfPos{ GOLD_WIDTH / 2, GOLD_HEIGHT / 2 };
	POINT playerHalfPos{ PLAYER_WIDTH / 2, PLAYER_HEIGHT / 2 };

	for (auto iter = m_goldList.begin(); iter != m_goldList.end(); )
	{
		POINT goldCenterPoint{ iter->x + goldHalfPos.x, iter->y + goldHalfPos.y };
		POINT playerCenterPoint{ m_curPlayerPos.x + playerHalfPos.x, m_curPlayerPos.y + playerHalfPos.y };

		if (abs(goldCenterPoint.x - playerCenterPoint.x) <= goldHalfPos.x + playerHalfPos.x - 10
			&& abs(goldCenterPoint.y - playerCenterPoint.y) <= goldHalfPos.y + playerHalfPos.y - 20)
		{
			// ���ò������ֵ��x��10��y��20
			// ��ұ��ԣ�ɾ��
			iter = m_goldList.erase(iter);
			m_golds += 100;
			continue;
		}
		++iter;  
	}
}