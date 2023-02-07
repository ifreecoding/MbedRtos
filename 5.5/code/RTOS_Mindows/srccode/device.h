
#ifndef  DEVICE_H
#define  DEVICE_H


#include "global.h"


#define FSMCCMDADDR         ((U32)0x60000000)           /* FSMC �����ַ */
#define FSMCDATAADDR        ((U32)0x60020000)           /* FSMC ���ݵ�ַ */


/* LCD��ʾ�궨��, �������Ͻ�Ϊ����ԭ��, X�᷽������, Y�᷽������ */
#define LCDXLEN             240                         /* LCD X�᳤��, ��λ: ���� */
#define LCDYLEN             400                         /* LCD Y�᳤��, ��λ: ���� */


/* CPUռ����LCD��ʾ�궨�� */
#define CSDSPXSTART         130                         /* CPUռ������ʾ����X����ʼ��, ��λ: ���� */
#define CSDSPYSTART         30                          /* CPUռ������ʾ����Y����ʼ��, ��λ: ���� */
#define CSDSPXLEN           (LCDXLEN - CSDSPXSTART - 10)/* CPUռ������ʾ����X�᳤��, ��λ: ���� */
#define CSDSPYLEN           (LCDYLEN - CSDSPYSTART - 10)/* CPUռ������ʾ����Y�᳤��, ��λ: ���� */
#define CSDSPXWID           3                           /* CPUռ������ʾ����X����, ��λ: ���� */
#define CSDSPYWID           3                           /* CPUռ������ʾ����Y����, ��λ: ���� */

#define CSDSPWID            (CSDSPXLEN - 20)            /* CPUռ������ʾ������, X�᷽��, ��λ: ���� */
#define CSDSPLEN            (CSDSPYLEN - 10)            /* CPUռ������ʾ���򳤶�, Y�᷽��, ��λ: ���� */
#define CSDSPSWTITV         1                           /* CPUռ������ʾ����ͳ�Ƶ���, ��λ: ���� */
#define CSDSPSWTTICKNUM     10                          /* CPUռ������ʾ����ͳ�Ƶ�֮���TICK�� */
#define CSDSPBTNUM          (500 / CSDSPSWTTICKNUM)     /* CPUռ������ʾ����2��ʱ��̶���֮��������л����� */
#define CSDSPTIMEITV        (CSDSPSWTITV * CSDSPBTNUM)  /* CPUռ������ʾ����ʱ��̶���ʱ����, ��λ: ���� */
#define CSDSPTIMENUM        (CSDSPLEN / CSDSPTIMEITV)   /* ʱ��̶��ߵ����� */
#define CSDSPTIMELEN        5                           /* ʱ��̶��ߵĳ���, X�᷽��, ��λ: ���� */
#define CSDSPTIMEWID        3                           /* ʱ��̶��ߵĿ��, Y�᷽��, ��λ: ���� */
#define CSDSPLINEWID        3                           /* CPUռ�����߿�, ��λ: ���� */
#define CSDSPSWTNUM         (CSDSPLEN / CSDSPSWTITV / CSDSPSWTTICKNUM)   /* CPUռ���ʵ������ */

#define CPUSHAREMSGLEN      (CSDSPSWTNUM + 1)           /* ���CPUռ������Ϣ���λ���ĳ��� */

#define CSTASKXSTART        100                         /* CPUռ����������ʾ����X����ʼ��, ��λ: ���� */
#define CSTASKYSTART        30                          /* CPUռ����������ʾ����Y����ʼ��, ��λ: ���� */
#define CSVALYSTART         100                         /* CPUռ������ֵ��ʾ����Y����ʼ��, ��λ: ���� */


/* �洢CPUռ������Ϣ�Ľṹ */
typedef struct cpusharemsgstr
{
    U32 astrMsg[CPUSHAREMSGLEN];    /* ���CPUռ������Ϣ�Ļ��λ��� */
    U32 uiHead;                     /* ���λ���ͷλ�� */
    U32 uiTail;                     /* ���λ���βλ�� */
}CPUSHAREMSGSTR;


/*********************************** �������� *************************************/
extern CPUSHAREMSGSTR gstrCsMsgStr;
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
extern void DEV_CpuShareDisplayInit(void);
extern void DEV_CpuShareMsgStrInit(void);
extern void DEV_BufferInit(BUFPOOL* pstrBufPool);
extern U8 DEV_ReadHardwareVersion(void);


#endif

