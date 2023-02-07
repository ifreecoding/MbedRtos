
#ifndef  GLOBAL_H
#define  GLOBAL_H


#include "mindows.h"


/* 软件版本号 */
#define SOFTWARE_VER            "001.001.002.000"


#define BUFPOOLNUM              20      /* BUFPOOL数量 */
#define MSGBUFLEN               100     /* 每个BUF的长度, 单位: 字节 */


#define TASKSTACK               1024    /* 任务栈大小, 单位: 字节 */


/* 消息缓冲结构 */
typedef struct msgbuf
{
    M_DLIST strList;                    /* 缓冲链表 */
    U8 ucLength;                        /* 消息长度 */
    U8 aucBuf[MSGBUFLEN];               /* 消息缓冲 */
}MSGBUF;

/* 消息缓冲池结构 */
typedef struct bufpool
{
    M_DLIST strFreeList;                /* 缓冲池空闲链表 */
    MSGBUF astrBufPool[BUFPOOLNUM];     /* 缓冲池 */
}BUFPOOL;


/*********************************** 变量声明 *************************************/
extern BUFPOOL gstrBufPool;
extern M_QUE gstrSerialMsgQue;
extern U8 gaucTaskSrlStack[TASKSTACK];
extern U8 gaucTask1Stack[TASKSTACK];
extern U8 gaucTask2Stack[TASKSTACK];
extern U8 gaucTask3Stack[TASKSTACK];
extern U8 gaucTask4Stack[TASKSTACK];
extern M_TCB* gpstrTask4Tcb;


/*********************************** 函数声明 *************************************/
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

