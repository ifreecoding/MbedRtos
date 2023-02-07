
#ifndef  MDS_MDSDEF_H
#define  MDS_MDSDEF_H


/* 定义变量类型 */
typedef char                        U8;     /* 本编译器char型为无符号型 */
typedef unsigned short              U16;
typedef unsigned int                U32;
typedef signed char                 S8;
typedef short                       S16;
typedef int                         S32;
typedef void                        (*VFUNC)(void*);

#ifndef NULL
 #define NULL                       ((void*)0)
#endif


/* 返回值 */
#define RTN_SUCD                    0       /* 成功 */
#define RTN_FAIL                    1       /* 失败 */


/* THUMB指令集USR工作模式掩码 */
#define MODE_USR                    0x01000000


/* 字节对齐掩码 */
#define ALIGN4MASK                  0xFFFFFFFC  /* 4字节对齐 */
#define ALIGN8MASK                  0xFFFFFFF8  /* 8字节对齐 */


/* 任务优先级数目 */
#define PRIORITY256                 256
#define PRIORITY128                 128
#define PRIORITY64                  64
#define PRIORITY32                  32
#define PRIORITY16                  16
#define PRIORITY8                   8


/* 调度表宏定义 */
#if PRIORITY256 == PRIORITYNUM
 #define PRIOFLAGGRP1               32
 #define PRIOFLAGGRP2               4
#elif PRIORITY128 == PRIORITYNUM
 #define PRIOFLAGGRP1               16
 #define PRIOFLAGGRP2               2
#elif PRIORITY64 == PRIORITYNUM
 #define PRIOFLAGGRP1               8
 #define PRIOFLAGGRP2               1
#elif PRIORITY32 == PRIORITYNUM
 #define PRIOFLAGGRP1               4
 #define PRIOFLAGGRP2               1
#elif PRIORITY16 == PRIORITYNUM
 #define PRIOFLAGGRP1               2
 #define PRIOFLAGGRP2               1
#elif PRIORITY8 == PRIORITYNUM
 #define PRIOFLAGGRP1               1
 #define PRIOFLAGGRP2               1
#else
 #error The priotity must be defined between 8 and 256, and must be the power of 2.
#endif


/* TCB中备份寄存器组的结构体, 用来临时保存任务前换的寄存器 */
typedef struct stackreg
{
    U32 uiR0;
    U32 uiR1;
    U32 uiR2;
    U32 uiR3;
    U32 uiR4;
    U32 uiR5;
    U32 uiR6;
    U32 uiR7;
    U32 uiR8;
    U32 uiR9;
    U32 uiR10;
    U32 uiR11;
    U32 uiR12;
    U32 uiR13;
    U32 uiR14;
    U32 uiR15;
    U32 uiXpsr;
    U32 uiExc_Rtn;
}STACKREG;

typedef struct m_dlist              /* 链表结构 */
{
    struct m_dlist* pstrHead;       /* 头指针 */
    struct m_dlist* pstrTail;       /* 尾指针 */
}M_DLIST;

typedef struct m_tcbque             /* TCB队列结构 */
{
    M_DLIST strQueHead;             /* 连接队列的链表 */
    struct m_tcb* pstrTcb;          /* TCB指针 */
}M_TCBQUE;

typedef struct m_prioflag               /* 优先级标志表 */
{
#if PRIORITYNUM >= PRIORITY128
    U8 aucPrioFlagGrp1[PRIOFLAGGRP1];
    U8 aucPrioFlagGrp2[PRIOFLAGGRP2];
    U8 ucPrioFlagGrp3;
#elif PRIORITYNUM >= PRIORITY16
    U8 aucPrioFlagGrp1[PRIOFLAGGRP1];
    U8 ucPrioFlagGrp2;
#else
    U8 ucPrioFlagGrp1;
#endif
}M_PRIOFLAG;

typedef struct m_taskschedtab           /* 任务调度表 */
{
    M_DLIST astrList[PRIORITYNUM];      /* 各个优先级根节点 */
    M_PRIOFLAG strFlag;                 /* 优先级标志 */
}M_TASKSCHEDTAB;

/* TCB结构体 */
typedef struct m_tcb
{
    STACKREG strStackReg;           /* 备份寄存器组 */
    M_TCBQUE strTcbQue;             /* TCB结构队列 */
    U8 ucTaskPrio;                  /* 任务优先级 */
}M_TCB;


#endif

