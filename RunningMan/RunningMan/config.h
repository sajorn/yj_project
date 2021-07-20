#pragma once

// 背景图
#define BACKGROUND_WIDTH  (960)
#define BACKGROUND_HEIGHT (640)

// 人物图
#define PLAYER_WIDTH      (138)
#define PLAYER_HEIGHT     (138)

// 金币图
#define GOLD_WIDTH        (69)
#define GOLD_HEIGHT       (64)

// 人物初始位置
#define PLAYER_POS_X       (130)
#define PLAYER_POS_Y       (265)

// 物理引擎相关
#define INIT_SPEED         (600)         // 人物跑动速度
#define PLAYER_GRAVITY     (1500)        // 人物重力
#define JUMP_MAX_HEIGHT    ((INIT_SPEED*INIT_SPEED) / (2*PLAYER_GRAVITY))