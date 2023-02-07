
#include "mindows_inner.h"


U32 guiIntLockCounter;                  /* 锁中断计数值 */


/***********************************************************************************
函数功能: 初始化任务栈函数, 用来创建任务运行前的栈状态.
入口参数: pstrTcb: 需要初始化的任务TCB指针.
          vfFuncPointer: 创建任务所运行的函数.
          pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void MDS_TaskStackInit(M_TCB* pstrTcb, VFUNC vfFuncPointer, void* pvPara)
{
    STACKREG* pstrRegSp;
    U32* puiStack;

    pstrRegSp = &pstrTcb->strStackReg;          /* 寄存器组地址 */

    /* 对TCB中的寄存器组初始化 */
    pstrRegSp->uiR0 = (U32)pvPara;              /* R0 */
    pstrRegSp->uiR1 = 0;                        /* R1 */
    pstrRegSp->uiR2 = 0;                        /* R2 */
    pstrRegSp->uiR3 = 0;                        /* R3 */
    pstrRegSp->uiR4 = 0;                        /* R4 */
    pstrRegSp->uiR5 = 0;                        /* R5 */
    pstrRegSp->uiR6 = 0;                        /* R6 */
    pstrRegSp->uiR7 = 0;                        /* R7 */
    pstrRegSp->uiR8 = 0;                        /* R8 */
    pstrRegSp->uiR9 = 0;                        /* R9 */
    pstrRegSp->uiR10 = 0;                       /* R10 */
    pstrRegSp->uiR11 = 0;                       /* R11 */
    pstrRegSp->uiR12 = 0;                       /* R12 */
    pstrRegSp->uiR13 = (U32)pstrTcb - 32;       /* R13 */
    pstrRegSp->uiR14 = (U32)MDS_TaskSelfDelete; /* R14 */
    pstrRegSp->uiR15 = (U32)vfFuncPointer;      /* R15 */
    pstrRegSp->uiXpsr = MODE_USR;               /* XPSR */
    pstrRegSp->uiExc_Rtn = RTN_THREAD_MSP;      /* EXC_RETURN */

#ifdef MDS_DEBUGSTACKCHECK

    /* 向栈填充数据, 供检查栈时使用 */
    MDS_TaskStackCheckInit(pstrTcb);

#endif

    /* 构造任务初始运行时的栈, 该栈在任务运行时由硬件自动取出 */
    puiStack = (U32*)pstrTcb;
    *(--puiStack) = pstrRegSp->uiXpsr;
    *(--puiStack) = pstrRegSp->uiR15;
    *(--puiStack) = pstrRegSp->uiR14;
    *(--puiStack) = pstrRegSp->uiR12;
    *(--puiStack) = pstrRegSp->uiR3;
    *(--puiStack) = pstrRegSp->uiR2;
    *(--puiStack) = pstrRegSp->uiR1;
    *(--puiStack) = pstrRegSp->uiR0;
}

/***********************************************************************************
函数功能: 初始化用于操作系统任务调度使用的硬件.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_SystemHardwareInit(void)
{
    /* 初始化tick定时器 */
    MDS_TickTimerInit();

    /* 初始化PendSv中断 */
    MDS_PendSvIsrInit();

    /* 将处理器置为Thread用户级模式 */
    MDS_SetChipWorkMode(UNPRIVILEGED);
}

/***********************************************************************************
函数功能: 初始化tick定时器.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_TickTimerInit(void)
{
    /* 设置Tick中断定时周期 */
    (void)SysTick_Config(SystemCoreClock / (1000 / TICK));

    /* 设置Tick中断优先级为6 */
    NVIC_SetPriority(SysTick_IRQn, 6 << 1);
}

/***********************************************************************************
函数功能: 初始化PENDSV中断.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_PendSvIsrInit(void)
{
    /* 设置PendSV中断优先级为7 */
    NVIC_SetPriority(PendSV_IRQn, 7 << 1);
}

/***********************************************************************************
函数功能: 设置处理器工作模式.
入口参数: uiMode: PRIVILEGED: 特权级模式.
                  UNPRIVILEGED: 用户级模式.
返 回 值: none.
***********************************************************************************/
void MDS_SetChipWorkMode(U32 uiMode)
{
    U32 uiCtrl;

    /* 读取control寄存器数值 */
    uiCtrl = __get_CONTROL();

    /* 修改control寄存器数值 */
    uiCtrl &= 0xFFFFFFFE;
    uiCtrl |= uiMode;

    /* 写入control寄存器数值 */
    __set_CONTROL(uiCtrl);
}

/***********************************************************************************
函数功能: 触发pendsv软中断调度任务.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_TaskSwiSched(void)
{
    /* 在中断中调用该函数 */
    if(RTN_SUCD != MDS_RunInInt())
    {
        /* 触发SWI软中断, 由软中断触发PendSv中断进行任务调度 */
        MDS_TaskOccurSwi(SWI_TASKSCHED);
    }
    else /* 没在中断中调用该函数 */
    {
        /* 直接触发PendSv中断进行调度任务 */
        MDS_IntPendSvSet();
    }
}

/***********************************************************************************
函数功能: 触发pendsv软中断的函数.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_IntPendSvSet(void)
{
#define HWREG(x)                (*((volatile unsigned long *)(x)))
#define NVIC_INT_CTRL           0xE000ED04  // Interrupt Control and State
#define NVIC_INT_CTRL_PEND_SV   0x10000000  // PendSV Set Pending

    /* 触发PendSv中断, 在该中断中调度任务 */
    HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;
}

/***********************************************************************************
函数功能: 判断当前程序是否是在中断中运行.
入口参数: none.
返 回 值: RTN_SUCD: 在中断中运行.
          RTN_FAIL: 不在中断中运行.
***********************************************************************************/
U32 MDS_RunInInt(void)
{
    /* 当前程序在中断中运行 */
    if(0 != (MDS_GetXpsr() & XPSR_EXTMASK))
    {
        return RTN_SUCD;
    }
    else
    {
        return RTN_FAIL;
    }
}

/***********************************************************************************
函数功能: 通过设置处理器总中断标志位禁止所有中断产生.
入口参数: none.
返 回 值: RTN_SUCD: 锁中断成功.
          RTN_FAIL: 锁中断失败.
***********************************************************************************/
U32 MDS_IntLock(void)
{
    /* 如果在中断中运行该函数则直接返回 */
    if(RTN_SUCD == MDS_RunInInt())
    {
        return RTN_FAIL;
    }

    /* 第一次调用该函数才做实际的锁中断操作 */
    if(0 == guiIntLockCounter)
    {
        __disable_irq();

        guiIntLockCounter++;

        return RTN_SUCD;
    }
    /* 非第一次调用该函数并且小于最大次数则直接返回成功 */
    else if(guiIntLockCounter < 0xFFFFFFFF)
    {
        guiIntLockCounter++;

        return RTN_SUCD;
    }
    else /* 超出最大次数则直接返回失败 */
    {
        return RTN_FAIL;
    }
}

/***********************************************************************************
函数功能: 使能处理器总中断标志位, 允许中断产生.
入口参数: none.
返 回 值: RTN_SUCD: 解锁中断成功.
          RTN_FAIL: 解锁中断失败.
***********************************************************************************/
U32 MDS_IntUnlock(void)
{
    /* 如果在中断中运行该函数则直接返回 */
    if(RTN_SUCD == MDS_RunInInt())
    {
        return RTN_FAIL;
    }

    /* 非第一次调用该函数直接返回成功 */
    if(guiIntLockCounter > 1)
    {
        guiIntLockCounter--;

        return RTN_SUCD;
    }
    /* 最后一次调用该函数才做实际的解锁中断操作 */
    else if(1 == guiIntLockCounter)
    {
        guiIntLockCounter--;

        __enable_irq();

        return RTN_SUCD;
    }
    else /* 等于0次则直接返回失败 */
    {
        return RTN_FAIL;
    }
}

