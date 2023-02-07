
#ifndef  WLX_CORE_C_H
#define  WLX_CORE_C_H


#include "wanlix.h"


/* THUMB指令集USR工作模式掩码 */
#define MODE_USR            0x01000000


/* 字节对齐掩码 */
#define ALIGN4MASK          0xFFFFFFFC  /* 4字节对齐 */
#define ALIGN8MASK          0xFFFFFFF8  /* 8字节对齐 */


/*********************************** 变量声明 *************************************/
extern U8 gaucRootTaskStack[ROOTTASKSTACK];
extern W_TCB* gpstrCurTcb;
extern W_TCB* gpstrRootTaskTcb;


/*********************************** 函数声明 *************************************/
extern W_TCB* WLX_TaskTcbInit(VFUNC vfFuncPointer, void* pvPara, U8* pucTaskStack,
                              U32 uiStackSize);
extern void WLX_TaskStackInit(W_TCB* pstrTcb, VFUNC vfFuncPointer, void* pvPara);
extern void WLX_ContextSwitch(STACKREG* pstrCurTaskStackRegAddr,
                              STACKREG* pstrNextTaskStackRegAddr);
extern void WLX_SwitchToTask(STACKREG* pstrNextTaskStackRegAddr);
extern void WLX_TaskStart(W_TCB* pstrTcb);
extern void WLX_RootTask(void* pvPara);


#endif

