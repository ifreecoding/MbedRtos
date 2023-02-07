
#include "wlx_core_c.h"


U32 guiCurTask;                         /* 当前运行的任务 */

W_TCB* gpstrTask1Tcb;                   /* 任务1的TCB指针 */
W_TCB* gpstrTask2Tcb;                   /* 任务2的TCB指针 */


/***********************************************************************************
函数功能: 创建任务运行前的状态.
入口参数: vfFuncPointer: 任务运行的函数指针.
          puiTaskStack: 任务使用的栈地址.
返 回 值: 任务的TCB指针.
***********************************************************************************/
W_TCB* WLX_TaskInit(VFUNC vfFuncPointer, U32* puiTaskStack)
{
    W_TCB* pstrTcb;
    STACKREG* pstrStackReg;

    /* 对于递减栈, TCB放在栈顶 */
    pstrTcb = (W_TCB*)((U32)puiTaskStack - sizeof(W_TCB));

    /* 寄存器组地址 */
    pstrStackReg = &pstrTcb->strStackReg;

    /* 对TCB中的寄存器组初始化 */
    pstrStackReg->uiR4 = 0;                     /* R4 */
    pstrStackReg->uiR5 = 0;                     /* R5 */
    pstrStackReg->uiR6 = 0;                     /* R6 */
    pstrStackReg->uiR7 = 0;                     /* R7 */
    pstrStackReg->uiR8 = 0;                     /* R8 */
    pstrStackReg->uiR9 = 0;                     /* R9 */
    pstrStackReg->uiR10 = 0;                    /* R10 */
    pstrStackReg->uiR11 = 0;                    /* R11 */
    pstrStackReg->uiR12 = 0;                    /* R12 */
    pstrStackReg->uiR13 = (U32)pstrTcb;         /* R13 */
    pstrStackReg->uiR14 = (U32)vfFuncPointer;   /* R14 */
    pstrStackReg->uiXpsr = MODE_USR;            /* XPSR */

    /* 返回任务的TCB指针 */
    return pstrTcb;
}

/***********************************************************************************
函数功能: 调用该函数将发生任务切换.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void WLX_TaskSwitch(void)
{
    STACKREG* pstrCurTaskStackRegAddr;
    STACKREG* pstrNextTaskStackRegAddr;

    if(1 == guiCurTask)
    {
        /* 当前任务寄存器组的地址 */
        pstrCurTaskStackRegAddr = &gpstrTask1Tcb->strStackReg;

        /* 即将运行任务寄存器组的地址 */
        pstrNextTaskStackRegAddr = &gpstrTask2Tcb->strStackReg;

        /* 更新下次运行的任务 */
        guiCurTask = 2;
    }
    else //if(2 == guiCurTask)
    {
        pstrCurTaskStackRegAddr = &gpstrTask2Tcb->strStackReg;
        pstrNextTaskStackRegAddr = &gpstrTask1Tcb->strStackReg;

        guiCurTask = 1;
    }

    /* 切换任务 */
    WLX_ContextSwitch(pstrCurTaskStackRegAddr, pstrNextTaskStackRegAddr);
}

/***********************************************************************************
函数功能: 开始进入任务调度.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void WLX_TaskStart(void)
{
    STACKREG* pstrNextTaskStackRegAddr;

    /* 即将运行任务寄存器组的地址 */
    pstrNextTaskStackRegAddr = &gpstrTask1Tcb->strStackReg;

    /* 更新下次调度的任务 */
    guiCurTask = 1;

    /* 切换到任务状态 */
    WLX_SwitchToTask(pstrNextTaskStackRegAddr);
}

