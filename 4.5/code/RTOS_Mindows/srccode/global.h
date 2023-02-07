
#ifndef  GLOBAL_H
#define  GLOBAL_H


#include "mindows.h"


/* ����汾�� */
#define SOFTWARE_VER            "001.001.002.000"


#define BUFPOOLNUM              20      /* BUFPOOL���� */
#define MSGBUFLEN               100     /* ÿ��BUF�ĳ���, ��λ: �ֽ� */


#define TASKSTACK               1024    /* ����ջ��С, ��λ: �ֽ� */
#define LCDTASKSTACK            3072    /* LCD��ӡ����ջ��С, ��λ: �ֽ� */


/* ��Ϣ����ṹ */
typedef struct msgbuf
{
    M_DLIST strList;                    /* �������� */
    U8 ucLength;                        /* ��Ϣ���� */
    U8 aucBuf[MSGBUFLEN];               /* ��Ϣ���� */
}MSGBUF;

/* ��Ϣ����ؽṹ */
typedef struct bufpool
{
    M_DLIST strFreeList;                /* ����ؿ������� */
    MSGBUF astrBufPool[BUFPOOLNUM];     /* ����� */
}BUFPOOL;


/* LCD��ɫ */
#define LCDCOLORBLACK       0x0000
#define LCDCOLORWHITE       0xFFFF
#define LCDCOLORRED         0xF800
#define LCDCOLORGREEN       0x07E0
#define LCDCOLORBLUE        0x001F


/* �洢�����л���Ϣ�Ľṹ */
typedef struct lcdtaskswitchmsg
{
    U32 uiTask;
    U32 uiTime;
}LCDTASKSWITCHMSG;


/*********************************** �������� *************************************/
extern BUFPOOL gstrBufPool;
extern U16 gusPenColor;
extern U16 gusBackGroundColor;
extern M_QUE gstrSerialMsgQue;
extern M_TCB* gpstrSerialTaskTcb;
extern M_TCB* gpstrLcdTaskTcb;
extern U8 gaucTaskSrlStack[TASKSTACK];
extern U8 gaucTaskLcdStack[LCDTASKSTACK];
extern U8 gaucTask1Stack[TASKSTACK];
extern U8 gaucTask2Stack[TASKSTACK];
extern U8 gaucTask3Stack[TASKSTACK];


/* ����LCD������ɫ, usColor: ������ɫ, ��ʽΪ: R(5)G(6)B(5), R�����ݸ߶� */
#define LCDSETPENCOLOR(usColor)         (gusPenColor = usColor)

/* ��ȡLCD������ɫ, ����ֵΪ������ɫ, ��ʽΪ: R(5)G(6)B(5), R�����ݸ߶� */
#define LCDGETPENCOLOR                  gusPenColor

/* ����LCD����ɫ, usColor: ����ɫ, ��ʽΪ: R(5)G(6)B(5), R�����ݸ߶� */
#define LCDSETBACKGROUNDCOLOR(usColor)  (gusBackGroundColor = usColor)

/* ��ȡLCD����ɫ, ����ֵΪ����ɫ, ��ʽΪ: R(5)G(6)B(5), R�����ݸ߶� */
#define LCDGETBACKGROUNDCOLOR           gusBackGroundColor


/*********************************** �������� *************************************/
extern void DEV_SoftwareInit(void);
extern void DEV_HardwareInit(void);
extern void DEV_PutStrToMem(U8* pvStringPt, ...);
extern void DEV_PrintMsg(U8* pucBuf, U32 uiLen);
extern void DEV_LcdDrawBox(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen);
extern void DEV_LcdDrawXLine(U16 usXStart, U16 usYStart, U16 usLen, U16 usWid);
extern void DEV_LcdDrawYLine(U16 usXStart, U16 usYStart, U16 usLen, U16 usWid);
extern void DEV_LcdDrawPic(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen, U8* pucPic);
extern U8* DEV_CalcFontAddr(U8 ucAscii);
extern U8* DEV_CalcInvertFontAddr(U8 ucAscii);
extern void DEV_SaveTaskSwitchMsg(U8* pucTaskName);
extern void DEV_PrintTaskSwitchMsgToLcd(void);
extern MSGBUF* DEV_BufferAlloc(BUFPOOL* pstrBufPool);
extern void DEV_BufferFree(BUFPOOL* pstrBufPool, M_DLIST* pstrQueNode);
extern void DEV_HardwareVersionInit(void);
extern void DEV_DelayMs(U32 uiMs);
extern void TEST_TestTask1(void* pvPara);
extern void TEST_TestTask2(void* pvPara);
extern void TEST_TestTask3(void* pvPara);
extern void TEST_SerialPrintTask(void* pvPara);
extern void TEST_LcdPrintTask(void* pvPara);
extern void TEST_TaskCreatePrint(M_TCB* pstrTcb);
extern void TEST_TaskSwitchPrint(M_TCB* pstrOldTcb, M_TCB* pstrNewTcb);
extern void TEST_TaskDeletePrint(M_TCB* pstrTcb);


#endif

