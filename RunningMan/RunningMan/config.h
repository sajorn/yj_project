#pragma once

// ����ͼ
#define BACKGROUND_WIDTH  (960)
#define BACKGROUND_HEIGHT (640)

// ����ͼ
#define PLAYER_WIDTH      (138)
#define PLAYER_HEIGHT     (138)

// ���ͼ
#define GOLD_WIDTH        (69)
#define GOLD_HEIGHT       (64)

// �����ʼλ��
#define PLAYER_POS_X       (130)
#define PLAYER_POS_Y       (265)

// �����������
#define INIT_SPEED         (600)         // �����ܶ��ٶ�
#define PLAYER_GRAVITY     (1500)        // ��������
#define JUMP_MAX_HEIGHT    ((INIT_SPEED*INIT_SPEED) / (2*PLAYER_GRAVITY))