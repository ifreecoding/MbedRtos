
#ifndef  DEVICE_H
#define  DEVICE_H


#include "global.h"


#define FSMCCMDADDR         ((U32)0x60000000)           /* FSMC �����ַ */
#define FSMCDATAADDR        ((U32)0x60020000)           /* FSMC ���ݵ�ַ */


/* LCD��ʾ�궨��, �������Ͻ�Ϊ����ԭ��, X�᷽������, Y�᷽������ */
#define LCDXLEN             240                         /* LCD X�᳤��, ��λ: ���� */
#define LCDYLEN             400                         /* LCD Y�᳤��, ��λ: ���� */


/* �����л�LCD��ʾ�궨�� */
#define TSDSPXSTART         20                          /* �����л���ʾ����X����ʼ��, ��λ: ���� */
#define TSDSPYSTART         20                          /* �����л���ʾ����Y����ʼ��, ��λ: ���� */
#define TSDSPXLEN           (LCDXLEN - TSDSPXSTART - 20)/* �����л���ʾ����X�᳤��, ��λ: ���� */
#define TSDSPYLEN           (LCDYLEN - TSDSPYSTART - 10)/* �����л���ʾ����Y�᳤��, ��λ: ���� */
#define TSDSPXWID           3                           /* �����л���ʾ����X����, ��λ: ���� */
#define TSDSPYWID           3                           /* �����л���ʾ����Y����, ��λ: ���� */

#define TSDSPWID            (TSDSPXLEN - 20)            /* �����л���ʾ������, X�᷽��, ��λ: ���� */
#define TSDSPLEN            (TSDSPYLEN - 10)            /* �����л���ʾ���򳤶�, Y�᷽��, ��λ: ���� */
#define TSDSPSWTITV         2                           /* �����л���ʾ���������л�����, ��λ: ���� */
#define TSDSPSWTTICKNUM     10                          /* �����л���ʾ���������л���֮���TICK�� */
#define TSDSPBTNUM          (300 / TSDSPSWTTICKNUM)     /* �����л���ʾ����2��ʱ��̶���֮��������л����� */
#define TSDSPTIMEITV        (TSDSPSWTITV * TSDSPBTNUM)  /* �����л���ʾ����ʱ��̶���ʱ����, ��λ: ���� */
#define TSDSPTASKNUM        5                           /* �����л���������� */
#define TSDSPTASKITV        (TSDSPWID / TSDSPTASKNUM)   /* ����������ʾ���, ��λ: ���� */
#define TSDSPTIMENUM        (TSDSPLEN / TSDSPTIMEITV)   /* ʱ��̶��ߵ����� */
#define TSDSPTIMELEN        5                           /* ʱ��̶��ߵĳ���, X�᷽��, ��λ: ���� */
#define TSDSPTIMEWID        3                           /* ʱ��̶��ߵĿ��, Y�᷽��, ��λ: ���� */
#define TSDSPLINEWID        3                           /* �����л��߿�, ��λ: ���� */
#define TSDSPSWTNUM         (TSDSPLEN / TSDSPSWTITV)    /* �����л�������� */

#define TASKSWITCHMSGLEN    (TSDSPSWTNUM + 1)           /* ��������л���Ϣ���λ���ĳ��� */


/* ͼ�θ��±�־ */
#define TSFLAGUPDATA        0       /* �����л�ͼ����Ҫ���� */
#define TSFLAGNOTUPDATA     1       /* �����л�ͼ�β���Ҫ���� */


typedef struct taskswitchmsg
{
    U32 uiTask;         /* ������ */
    U32 uiRunTime;      /* �������е�ʱ��, ��ŵ����������е��л������ */
    U32 uiTolalTime;    /* �������������е���ʱ��, ��ŵ����������е��л������ */
}TASKSWITCHMSG;

typedef struct taskswitchmsgstr
{
    TASKSWITCHMSG astrMsg[TASKSWITCHMSGLEN];    /* ��������л���Ϣ�Ļ��λ��� */
    U32 uiHead;                                 /* ���λ���ͷλ�� */
    U32 uiTail;                                 /* ���λ���βλ�� */
    U32 uiFlag;                                 /* ͼ���Ƿ���µı�־ */
}TASKSWITCHMSGSTR;


/*********************************** �������� *************************************/
extern TASKSWITCHMSGSTR gstrTsMsgStr;
extern U8 gaucFont[];
extern U8 gaucInvertFont[];


/*********************************** �������� *************************************/
extern void DEV_UartInit(void);
extern void DEV_PutChar(U8 ucChar);
extern void DEV_LcdInit(void);
extern void DEV_LcdGpioCfg(void);
extern void DEV_LcdFsmcCfg(void);
extern void DEV_LcdReset(void);
extern void DEV_LcdLightOn(void);
extern void DEV_LcdLightOff(void);
extern void DEV_LcdWriteCmd(U16 usCmd);
extern void DEV_LcdWriteData(U16 usData);
extern void DEV_LcdSetBlock(U16 usXStart, U16 usXEnd, U16 usYStart, U16 usYEnd);
extern void DEV_TaskSwitchDisplayInit(void);
extern void DEV_TaskSwitchMsgStrInit(void);
extern void DEV_PrintTaskSwitchTime(U32 uiLastTime);
extern void DEV_BufferInit(BUFPOOL* pstrBufPool);
extern U8 DEV_ReadHardwareVersion(void);


#endif

