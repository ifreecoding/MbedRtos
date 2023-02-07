
#ifndef  GLOBAL_H
#define  GLOBAL_H


#include "mindows.h"


/* ����汾�� */
#define SOFTWARE_VER            "001.001.002.000"


#define BUFPOOLNUM              20      /* BUFPOOL���� */
#define MSGBUFLEN               100     /* ÿ��BUF�ĳ���, ��λ: �ֽ� */


#define TASKSTACK               1024    /* ����ջ��С, ��λ: �ֽ� */


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


/*********************************** �������� *************************************/
extern BUFPOOL gstrBufPool;
extern M_QUE gstrSerialMsgQue;
extern U8 gaucTaskSrlStack[TASKSTACK];
extern U8 gaucTask1Stack[TASKSTACK];
extern U8 gaucTask2Stack[TASKSTACK];
extern U8 gaucTask3Stack[TASKSTACK];
extern U8 gaucTask4Stack[TASKSTACK];
extern M_TCB* gpstrTask4Tcb;


/*********************************** �������� *************************************/
extern void DEV_SoftwareInit(void);
extern void DEV_HardwareInit(void);
extern void DEV_PutStrToMem(U8* pvStringPt, ...);
extern void DEV_PrintMsg(U8* pucBuf, U32 uiLen);
extern void DEV_PrintTaskSwitchMsgToLcd(void);
extern MSGBUF* DEV_BufferAlloc(BUFPOOL* pstrBufPool);
extern void DEV_BufferFree(BUFPOOL* pstrBufPool, M_DLIST* pstrQueNode);
extern void DEV_DelayMs(U32 uiMs);
extern void TEST_TestTask1(void* pvPara);
extern void TEST_TestTask2(void* pvPara);
extern void TEST_TestTask3(void* pvPara);
extern void TEST_TestTask4(void* pvPara);
extern void TEST_SerialPrintTask(void* pvPara);


#endif

