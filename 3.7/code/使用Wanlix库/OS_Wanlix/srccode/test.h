
#ifndef  TEST_H
#define  TEST_H


#include "global.h"


#define STATEUNCHANGE       0       /* ״̬δ�ı� */
#define STATECHANGE         1       /* ״̬�ı� */

#define STATE1TIME          200     /* ״̬1��ʱ��, ��λ: Tick */
#define STATE2TIME          50      /* ״̬2��ʱ��, ��λ: Tick */
#define STATE3TIME          100     /* ״̬3��ʱ��, ��λ: Tick */
#define STATE4TIME          50      /* ״̬4��ʱ��, ��λ: Tick */

#define FLASHTIME           5       /* ��˸��ʱ��, ��λ: Tick */


/* ��״̬���� */
#define LEDSTATE_DARK       0       /* ���� */
#define LEDSTATE_LIGHT      1       /* ���� */
#define LEDSTATE_FLASH      2       /* ��˸ */


/* ʮ��·��״̬���� */
typedef enum crossstate
{
    CROSSSTATE1 = 0,                /* ʮ��·��״̬1 */
    CROSSSTATE2,                    /* ʮ��·��״̬2 */
    CROSSSTATE3,                    /* ʮ��·��״̬3 */
    CROSSSTATE4,                    /* ʮ��·��״̬4 */
    CROSSSTATENUM                   /* ʮ��·����Ŀ */
}CROSSSTATE;


/* ʮ��·��ÿ��״̬�ĳ�ʼֵ */
#define CROSSINITVALUE \
{\
    {STATE1TIME, {{LEDSTATE_DARK, LED_DARK}, {LEDSTATE_DARK, LED_DARK}, {LEDSTATE_LIGHT, LED_LIGHT}, {LEDSTATE_LIGHT, LED_LIGHT}, {LEDSTATE_DARK, LED_DARK}}},\
    {STATE2TIME, {{LEDSTATE_DARK, LED_DARK}, {LEDSTATE_LIGHT, LED_LIGHT}, {LEDSTATE_DARK, LED_DARK}, {LEDSTATE_LIGHT, LED_LIGHT}, {LEDSTATE_DARK, LED_DARK}}},\
    {STATE3TIME, {{LEDSTATE_LIGHT, LED_LIGHT}, {LEDSTATE_DARK, LED_DARK}, {LEDSTATE_DARK, LED_DARK}, {LEDSTATE_DARK, LED_DARK}, {LEDSTATE_LIGHT, LED_LIGHT}}},\
    {STATE4TIME , {{LEDSTATE_LIGHT, LED_LIGHT}, {LEDSTATE_DARK, LED_DARK}, {LEDSTATE_DARK, LED_DARK}, {LEDSTATE_DARK, LED_DARK}, {LEDSTATE_FLASH, LED_DARK}}}\
}


/* LED��״̬ */
typedef struct ledstate
{
    U32 uiLedState;     /* ��״̬ */
    U32 uiBrightness;   /* ������ */
}LEDSTATE;

/* ·��״̬�ṹ�� */
typedef struct crossstatestr
{
    U32 uiRunTime;              /* ��״̬����ʱ��, ��λ: �� */
    LEDSTATE astrLed[LEDNUM];   /* ��״̬ */
}CROSSSTATESTR;


/*********************************** �������� *************************************/
extern CROSSSTATESTR gastrCrossSta[CROSSSTATENUM];
extern CROSSSTATESTR gstrCurCrossSta;
extern U32 guiCurSta;
extern U32 guiStaChange;


#endif

