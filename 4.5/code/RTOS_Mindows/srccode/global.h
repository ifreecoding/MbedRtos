
#ifndef  GLOBAL_H
#define  GLOBAL_H


#include "mindows.h"


/* 软件版本号 */
#define SOFTWARE_VER            "001.001.002.000"


#define BUFPOOLNUM              20      /* BUFPOOL数量 */
#define MSGBUFLEN               100     /* 每个BUF的长度, 单位: 字节 */


#define TASKSTACK               1024    /* 任务栈大小, 单位: 字节 */
#define LCDTASKSTACK            3072    /* LCD打印任务栈大小, 单位: 字节 */


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


/* LCD颜色 */
#define LCDCOLORBLACK       0x0000
#define LCDCOLORWHITE       0xFFFF
#define LCDCOLORRED         0xF800
#define LCDCOLORGREEN       0x07E0
#define LCDCOLORBLUE        0x001F


/* 存储任务切换信息的结构 */
typedef struct lcdtaskswitchmsg
{
    U32 uiTask;
    U32 uiTime;
}LCDTASKSWITCHMSG;


/*********************************** 变量声明 *************************************/
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


/* 设置LCD画笔颜色, usColor: 画笔颜色, 格式为: R(5)G(6)B(5), R在数据高端 */
#define LCDSETPENCOLOR(usColor)         (gusPenColor = usColor)

/* 获取LCD画笔颜色, 返回值为画笔颜色, 格式为: R(5)G(6)B(5), R在数据高端 */
#define LCDGETPENCOLOR                  gusPenColor

/* 设置LCD背景色, usColor: 背景色, 格式为: R(5)G(6)B(5), R在数据高端 */
#define LCDSETBACKGROUNDCOLOR(usColor)  (gusBackGroundColor = usColor)

/* 获取LCD背景色, 返回值为背景色, 格式为: R(5)G(6)B(5), R在数据高端 */
#define LCDGETBACKGROUNDCOLOR           gusBackGroundColor


/*********************************** 函数声明 *************************************/
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

