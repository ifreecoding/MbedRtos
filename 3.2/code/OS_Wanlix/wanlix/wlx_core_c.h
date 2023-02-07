
#ifndef  WLX_CORE_C_H
#define  WLX_CORE_C_H


#include "wanlix.h"


/* THUMB指令集USR工作模式掩码 */
#define MODE_USR            0x01000000


/*********************************** 变量声明 *************************************/
extern U32 guiCurTask;


/*********************************** 函数声明 *************************************/
extern void WLX_ContextSwitch(STACKREG* pstrCurTaskStackRegAddr,
                              STACKREG* pstrNextTaskStackRegAddr);
extern void WLX_SwitchToTask(STACKREG* pstrNextTaskStackRegAddr);


#endif

