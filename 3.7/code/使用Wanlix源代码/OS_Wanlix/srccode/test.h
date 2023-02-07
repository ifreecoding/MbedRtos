
#ifndef  TEST_H
#define  TEST_H


#include "global.h"


#define STATEUNCHANGE       0       /* 状态未改变 */
#define STATECHANGE         1       /* 状态改变 */

#define STATE1TIME          200     /* 状态1的时间, 单位: Tick */
#define STATE2TIME          50      /* 状态2的时间, 单位: Tick */
#define STATE3TIME          100     /* 状态3的时间, 单位: Tick */
#define STATE4TIME          50      /* 状态4的时间, 单位: Tick */

#define FLASHTIME           5       /* 闪烁的时间, 单位: Tick */


/* 灯状态定义 */
#define LEDSTATE_DARK       0       /* 常灭 */
#define LEDSTATE_LIGHT      1       /* 常亮 */
#define LEDSTATE_FLASH      2       /* 闪烁 */


/* 十字路口状态定义 */
typedef enum crossstate
{
    CROSSSTATE1 = 0,                /* 十字路口状态1 */
    CROSSSTATE2,                    /* 十字路口状态2 */
    CROSSSTATE3,                    /* 十字路口状态3 */
    CROSSSTATE4,                    /* 十字路口状态4 */
    CROSSSTATENUM                   /* 十字路口数目 */
}CROSSSTATE;


/* 十字路口每种状态的初始值 */
#define CROSSINITVALUE \
{\
    {STATE1TIME, {{LEDSTATE_DARK, LED_DARK}, {LEDSTATE_DARK, LED_DARK}, {LEDSTATE_LIGHT, LED_LIGHT}, {LEDSTATE_LIGHT, LED_LIGHT}, {LEDSTATE_DARK, LED_DARK}}},\
    {STATE2TIME, {{LEDSTATE_DARK, LED_DARK}, {LEDSTATE_LIGHT, LED_LIGHT}, {LEDSTATE_DARK, LED_DARK}, {LEDSTATE_LIGHT, LED_LIGHT}, {LEDSTATE_DARK, LED_DARK}}},\
    {STATE3TIME, {{LEDSTATE_LIGHT, LED_LIGHT}, {LEDSTATE_DARK, LED_DARK}, {LEDSTATE_DARK, LED_DARK}, {LEDSTATE_DARK, LED_DARK}, {LEDSTATE_LIGHT, LED_LIGHT}}},\
    {STATE4TIME , {{LEDSTATE_LIGHT, LED_LIGHT}, {LEDSTATE_DARK, LED_DARK}, {LEDSTATE_DARK, LED_DARK}, {LEDSTATE_DARK, LED_DARK}, {LEDSTATE_FLASH, LED_DARK}}}\
}


/* LED灯状态 */
typedef struct ledstate
{
    U32 uiLedState;     /* 灯状态 */
    U32 uiBrightness;   /* 灯亮度 */
}LEDSTATE;

/* 路口状态结构体 */
typedef struct crossstatestr
{
    U32 uiRunTime;              /* 该状态运行时间, 单位: 秒 */
    LEDSTATE astrLed[LEDNUM];   /* 灯状态 */
}CROSSSTATESTR;


/*********************************** 变量声明 *************************************/
extern CROSSSTATESTR gastrCrossSta[CROSSSTATENUM];
extern CROSSSTATESTR gstrCurCrossSta;
extern U32 guiCurSta;
extern U32 guiStaChange;


#endif

