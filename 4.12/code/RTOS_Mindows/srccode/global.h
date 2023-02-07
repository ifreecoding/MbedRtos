
#ifndef  GLOBAL_H
#define  GLOBAL_H


#include "mindows.h"


/* 软件版本号 */
#define SOFTWARE_VER            "001.001.002.000"


#define BUFPOOLNUM              50      /* BUFPOOL数量 */
#define MSGBUFLEN               100     /* 每个BUF的长度, 单位: 字节 */


/* 按键定义 */
#define KEYUP                   0x1     /* 向上按键 */
#define KEYDOWN                 0x2     /* 向下按键 */
#define KEYLEFT                 0x4     /* 向左按键 */
#define KEYRIGHT                0x8     /* 向右按键 */
#define KEYRESTART              0x10    /* 重新开始按键 */

#define KEYNULL                 0       /* 没有按键按下 */
#define KEYNUM                  5       /* 按键总数 */

/* 按键IO定义 */
#define IOKEYUP                 GPIO_Pin_11 /* 向上按键 */
#define IOKEYDOWN               GPIO_Pin_12 /* 向下按键 */
#define IOKEYLEFT               GPIO_Pin_10 /* 向左按键 */
#define IOKEYRIGHT              GPIO_Pin_13 /* 向右按键 */
#define IOKEYRESTART            GPIO_Pin_15 /* 重新开始按键 */

/* 按键管脚在GPIO端口的偏移值 */
#define KEYUPOFFSET             11      /* 向上按键 */
#define KEYDOWNOFFSET           12      /* 向下按键 */
#define KEYLEFTOFFSET           10      /* 向左按键 */
#define KEYRIGHTOFFSET          13      /* 向右按键 */
#define KEYRESTARTOFFSET        15      /* 重新开始按键 */


/* 游戏状态 */
#define GAMESTA_STOP            0       /* 游戏处于停止状态 */
#define GAMESTA_DROP            1       /* 图形在主窗口区中正在下降 */
#define GAMESTA_DELLINE         2       /* 图形填满一行, 正在删除一行图形 */


/* 图形编号定义 */
typedef enum figno
{
    BOX_FIG = 0,        /* 方块 */
    I_FIG,              /* I型 */
    MOUNTAIN_FIG,       /* 山型 */
    Z_FIG,              /* Z型 */
    Z_REVERSE_FIG,      /* 反序Z型 */
    L_FIG,              /* L型 */
    L_REVERSE_FIG,      /* 反序L型 */
    FIGNUM              /* 图形总数 */
}FIGNO;


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
    M_SEM strSem;                       /* 缓冲池信号量 */
    MSGBUF astrBufPool[BUFPOOLNUM];     /* 缓冲池 */
}BUFPOOL;


/* LCD颜色 */
#define LCDCOLORBLACK       0x0000
#define LCDCOLORWHITE       0xFFFF
#define LCDCOLORRED         0xF800
#define LCDCOLORGREEN       0x07E0
#define LCDCOLORBLUE        0x001F
#define LCDCOLORBLUE2       0x051F
#define LCDCOLORGREY        0xF7DE
#define LCDCOLORMAGENTA     0xF81F
#define LCDCOLORCYAN        0x7FFF
#define LCDCOLORYELLOW      0xFFE0


/*********************************** 变量声明 *************************************/
extern BUFPOOL gstrBufPool;
extern U16 gusPenColor;
extern U16 gusBackGroundColor;
extern M_QUE* gpstrSerialMsgQue;
extern M_TCB* gpstrSerialTaskTcb;
extern M_QUE* gpstrKeyMsgQue;
extern M_SEM* gpstrFigSem;
extern U8 gucGameSta;


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
extern void DEV_LcdDrawTxt(U16 usXStart, U16 usYStart, U16 usXLen, U16 usYLen, U8* pucPic);
extern U8 DEV_ReadKey(void);
extern void DEV_BufferInit(BUFPOOL* pstrBufPool);
extern MSGBUF* DEV_BufferAlloc(BUFPOOL* pstrBufPool);
extern void DEV_BufferFree(BUFPOOL* pstrBufPool, M_DLIST* pstrQueNode);
extern U32 DEV_GetRandomFig(void);
extern void DEV_HardwareVersionInit(void);
extern void DEV_DelayMs(U32 uiMs);
extern void TEST_FlushScnTask(void* pvPara);
extern void TEST_KeyTask(void* pvPara);
extern void TEST_ProcessTask(void* pvPara);
extern void TEST_SerialPrintTask(void* pvPara);
extern void TEST_TaskCreatePrint(M_TCB* pstrTcb);
extern void TEST_TaskSwitchPrint(M_TCB* pstrOldTcb, M_TCB* pstrNewTcb);
extern void TEST_TaskDeletePrint(M_TCB* pstrTcb);


#endif

