
#include "mindows_inner.h"


/***********************************************************************************
函数功能: 创建一个任务.
入口参数: vfFuncPointer: 创建任务所使用函数的指针.
          pvPara: 任务入口参数指针.
          pucTaskStack: 任务所使用栈的最低起始地址.
          uiStackSize: 栈大小, 单位: 字节.
          ucTaskPrio: 任务优先级.
返 回 值: NULL: 创建任务失败.
          其它: 任务的TCB指针.
***********************************************************************************/
M_TCB* MDS_TaskCreate(VFUNC vfFuncPointer, void* pvPara, U8* pucTaskStack,
                      U32 uiStackSize, U8 ucTaskPrio)
{
    M_TCB* pstrTcb;

    /* 对创建任务所使用函数的指针合法性进行检查 */
    if(NULL == vfFuncPointer)
    {
        /* 指针为空, 返回失败 */
        return (M_TCB*)NULL;
    }

    /* 对任务栈合法性进行检查 */
    if((NULL == pucTaskStack) || (0 == uiStackSize))
    {
        /* 栈不合法, 返回失败 */
        return (M_TCB*)NULL;
    }

    /* 对任务优先级检查 */
    if(USERROOT == MDS_GetUser())
    {
        /* 对于ROOT用户, 任务优先级不能低于最低优先级 */
        if(ucTaskPrio > LOWESTPRIO)
        {
            return (M_TCB*)NULL;
        }
    }
    else //if(USERGUEST == MDS_GetUser())
    {
        /* 对于GUEST用户, 任务不能高于用户最高优先级, 不能低于用户最低优先级 */
        if((ucTaskPrio < USERHIGHESTPRIO) || (ucTaskPrio > USERLOWESTPRIO))
        {
            return (M_TCB*)NULL;
        }
    }

    /* 初始化TCB */
    pstrTcb = MDS_TaskTcbInit(vfFuncPointer, pvPara, pucTaskStack, uiStackSize,
                              ucTaskPrio);

    return pstrTcb;
}

/***********************************************************************************
函数功能: 初始化任务TCB, 用来创建任务运行前TCB的状态.
入口参数: vfFuncPionter: 创建任务所使用的函数指针.
          pvPara: 任务入口参数指针.
          pucTaskStack: 任务所使用的栈地址.
          uiStackSize: 栈大小, 单位: 字节.
          ucTaskPrio: 任务优先级.
返 回 值: NULL: 创建任务失败.
          其它: 创建任务成功, 返回值为任务的TCB指针.
***********************************************************************************/
M_TCB* MDS_TaskTcbInit(VFUNC vfFuncPointer, void* pvPara, U8* pucTaskStack,
                       U32 uiStackSize, U8 ucTaskPrio)
{
    M_TCB* pstrTcb;
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8* pucStackBy4;

    /* 栈满地址, 需要4字节对齐 */
    pucStackBy4 = (U8*)(((U32)pucTaskStack + uiStackSize) & ALIGN4MASK);

    /* TCB结构存放的地址, 需要8字节对齐 */
    pstrTcb = (M_TCB*)(((U32)pucStackBy4 - sizeof(M_TCB)) & STACKALIGNMASK);

    /* 初始化任务栈 */
    MDS_TaskStackInit(pstrTcb, vfFuncPointer, pvPara);

    /* 初始化指向TCB的指针 */
    pstrTcb->strTcbQue.pstrTcb = pstrTcb;

    /* 初始化任务优先级 */
    pstrTcb->ucTaskPrio = ucTaskPrio;

    pstrList = &gstrReadyTab.astrList[ucTaskPrio];
    pstrNode = &pstrTcb->strTcbQue.strQueHead;
    pstrPrioFlag = &gstrReadyTab.strFlag;

    /* 锁中断, 防止其它任务影响 */
    (void)MDS_IntLock();

    /* 将该任务添加到ready表中 */
    MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);

    /* 挂入链表后解锁中断, 允许任务调度 */
    (void)MDS_IntUnlock();

    return pstrTcb;
}

