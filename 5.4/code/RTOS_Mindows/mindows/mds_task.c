
#include <stdlib.h>
#include "mindows_inner.h"


#ifdef MDS_TASKROUNDROBIN
U32 guiTimeSlice;                       /* 轮转调度的时间周期值 */
U32 gauiSliceCnt[PRIORITYNUM];          /* 轮转调度的时间计数值 */
#endif


#ifdef MDS_INCLUDETASKHOOK
VFHCRT gvfTaskCreateHook;               /* 任务创建钩子变量 */
VFHSWT gvfTaskSwitchHook;               /* 任务切换钩子变量 */
VFHDLT gvfTaskDeleteHook;               /* 任务删除钩子变量 */
#endif


/***********************************************************************************
函数功能: 创建一个任务.
入口参数: pucTaskName: 指向任务名称的指针.
          vfFuncPointer: 创建任务所使用函数的指针.
          pvPara: 任务入口参数指针.
          pucTaskStack: 任务所使用栈的最低起始地址. 若为空, 则由任务自行申请.
          uiStackSize: 栈大小, 单位: 字节.
          ucTaskPrio: 任务优先级.
          pstrTaskOpt: 任务参数指针.
返 回 值: NULL: 创建任务失败.
          其它: 任务的TCB指针.
***********************************************************************************/
M_TCB* MDS_TaskCreate(U8* pucTaskName, VFUNC vfFuncPointer, void* pvPara,
           U8* pucTaskStack, U32 uiStackSize, U8 ucTaskPrio, M_TASKOPT* pstrTaskOpt)
{
    M_TCB* pstrTcb;

    /* pucTaskName和pstrTaskOpt参数并非必须, 不做检查 */

    /* 对创建任务所使用函数的指针合法性进行检查 */
    if(NULL == vfFuncPointer)
    {
        /* 指针为空, 返回失败 */
        return (M_TCB*)NULL;
    }

    /* 对任务栈合法性进行检查 */
    if(0 == uiStackSize)
    {
        /* 栈不合法, 返回失败 */
        return (M_TCB*)NULL;
    }

    /* 配置任务参数时对任务状态合法性进行检查 */
    if(NULL != pstrTaskOpt)
    {
        /* 不存在的任务状态返回失败 */
        if(!((TASKREADY == pstrTaskOpt->ucTaskSta)
             || (TASKDELAY == pstrTaskOpt->ucTaskSta)))
        {
            return (M_TCB*)NULL;
        }
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
    pstrTcb = MDS_TaskTcbInit(pucTaskName, vfFuncPointer, pvPara, pucTaskStack,
                              uiStackSize, ucTaskPrio, pstrTaskOpt);

    /* 初始化TCB失败, 则返回失败 */
    if(NULL == pstrTcb)
    {
        return NULL;
    }

    /* 在操作系统状态下创建任务后需要进行任务调度 */
    if(SYSTEMSCHEDULE == guiSystemStatus)
    {
#ifdef MDS_INCLUDETASKHOOK

        /* 如果任务创建钩子已经挂接函数则执行该函数 */
        if((VFHCRT)NULL != gvfTaskCreateHook)
        {
            gvfTaskCreateHook(pstrTcb);
        }

#endif

        /* 使用软中断调度任务 */
        MDS_TaskSwiSched();
    }

    return pstrTcb;
}

/***********************************************************************************
函数功能: 删除一个任务.
入口参数: pstrTcb: 需要删除的任务的TCB指针.
返 回 值: RTN_SUCD: 任务删除成功.
          RTN_FAIL: 任务删除失败.
***********************************************************************************/
U32 MDS_TaskDelete(M_TCB* pstrTcb)
{
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8 ucTaskPrio;
    U8 ucTaskSta;

    /* 入口参数检查 */
    if(NULL == pstrTcb)
    {
        return RTN_FAIL;
    }

    /* idle任务不能被删除 */
    if(pstrTcb == gpstrIdleTaskTcb)
    {
        return RTN_FAIL;
    }

    (void)MDS_IntLock();

#ifdef MDS_INCLUDETASKHOOK

    /* 如果任务删除钩子已经挂接函数则执行该函数 */
    if((VFHDLT)NULL != gvfTaskDeleteHook)
    {
        gvfTaskDeleteHook(pstrTcb);
    }

#endif

    /* 获取要删除任务的任务状态 */
    ucTaskSta = pstrTcb->strTaskOpt.ucTaskSta;

    /* 任务在ready表则从ready表拆除 */
    if(TASKREADY == (TASKREADY & ucTaskSta))
    {
        /* 获取该任务的相关调度参数 */
        ucTaskPrio = pstrTcb->ucTaskPrio;
        pstrList = &gstrReadyTab.astrList[ucTaskPrio];
        pstrPrioFlag = &gstrReadyTab.strFlag;

        /* 将该任务从ready表拆除 */
        (void)MDS_TaskDelFromSchedTab(pstrList, pstrPrioFlag, ucTaskPrio);
    }

    /* 任务在delay表则从delay表拆除 */
    if(DELAYQUEFLAG == (pstrTcb->uiTaskFlag & DELAYQUEFLAG))
    {
        /* 获取该任务TCB中挂接在delay调度表上的节点 */
        pstrNode = &pstrTcb->strTcbQue.strQueHead;

        /* 从delay表拆除该任务 */
        (void)MDS_DlistCurNodeDelete(&gstrDelayTab, pstrNode);
    }

    /* 任务拥有pend状态 */
    if(TASKPEND == (TASKPEND & ucTaskSta))
    {
        /* 从信号量调度表拆除任务 */
        (void)MDS_TaskDelFromSemTab(pstrTcb);
    }

    /* 删除的是当前任务 */
    if(pstrTcb == gpstrCurTcb)
    {
        /* 将当前寄存器组和栈信息保存到指定内存中 */
        MDS_SaveTaskContext();

        /* 将gpstrCurTcb置为NULL, 后面在任务上下文切换时不备份当前任务 */
        gpstrCurTcb = NULL;
    }

    /* 如果是任务自己申请的栈, 则需要释放 */
    if(TASKSTACKFLAG == (pstrTcb->uiTaskFlag & TASKSTACKFLAG))
    {
        free(pstrTcb->pucTaskStack);
    }

    (void)MDS_IntUnlock();

    /* 使用软中断调度任务 */
    MDS_TaskSwiSched();

    return RTN_SUCD;
}

/***********************************************************************************
函数功能: 删除任务自身.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_TaskSelfDelete(void)
{
    (void)MDS_TaskDelete(gpstrCurTcb);
}

/***********************************************************************************
函数功能: 初始化任务TCB, 用来创建任务运行前TCB的状态.
入口参数: pucTaskName: 指向任务名称的指针.
          vfFuncPionter: 创建任务所使用的函数指针.
          pvPara: 任务入口参数指针.
          pucTaskStack: 任务所使用的栈地址.
          uiStackSize: 栈大小, 单位: 字节.
          ucTaskPrio: 任务优先级.
          pstrTaskOpt: 任务参数指针.
返 回 值: NULL: 创建任务失败.
          其它: 创建任务成功, 返回值为任务的TCB指针.
***********************************************************************************/
M_TCB* MDS_TaskTcbInit(U8* pucTaskName, VFUNC vfFuncPointer, void* pvPara,
           U8* pucTaskStack, U32 uiStackSize, U8 ucTaskPrio, M_TASKOPT* pstrTaskOpt)
{
    M_TCB* pstrTcb;
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8* pucStackBy4;
    U32 uiTaskFlag;

    /* 锁中断, 防止其它任务影响 */
    (void)MDS_IntLock();

    /* 若传入栈为空, 则由任务自己申请内存 */
    if(NULL == pucTaskStack)
    {
        pucTaskStack = malloc(uiStackSize);
        if(NULL == pucTaskStack)
        {
            /* 返回前解锁中断 */
            (void)MDS_IntUnlock();

            /* 申请不到内存, 返回空指针 */
            return (M_TCB*)NULL;
        }

        /* 将任务标志置为栈申请状态 */
        uiTaskFlag = TASKSTACKFLAG;
    }
    else /* 是传入的栈, 先将任务标志置为空 */
    {
        uiTaskFlag = 0;
    }

    /* 栈满地址, 需要4字节对齐 */
    pucStackBy4 = (U8*)(((U32)pucTaskStack + uiStackSize) & ALIGN4MASK);

    /* TCB结构存放的地址, 需要8字节对齐 */
    pstrTcb = (M_TCB*)(((U32)pucStackBy4 - sizeof(M_TCB)) & STACKALIGNMASK);

    /* 保存任务栈起始地址 */
    pstrTcb->pucTaskStack = pucTaskStack;

    /* 初始化任务栈 */
    MDS_TaskStackInit(pstrTcb, vfFuncPointer, pvPara);

    /* 先将任务标志初始化为全空, 后面再为其增加具体的功能标志 */
    pstrTcb->uiTaskFlag = 0;

    /* 将任务标志置为申请栈的状态 */
    pstrTcb->uiTaskFlag |= uiTaskFlag;

    /* 初始化指向TCB的指针 */
    pstrTcb->strTcbQue.pstrTcb = pstrTcb;
    pstrTcb->strSemQue.pstrTcb = pstrTcb;

    /* 初始化指向任务名称的指针 */
    pstrTcb->pucTaskName = pucTaskName;

    /* 初始化任务优先级 */
    pstrTcb->ucTaskPrio = ucTaskPrio;

    /* 没有任务参数则将任务状态设置为ready态 */
    if(NULL == pstrTaskOpt)
    {
        pstrTcb->strTaskOpt.ucTaskSta = TASKREADY;
    }
    else /* 有任务参数则将参数复制到TCB中 */
    {
        pstrTcb->strTaskOpt.ucTaskSta = pstrTaskOpt->ucTaskSta;
        pstrTcb->strTaskOpt.uiDelayTick = pstrTaskOpt->uiDelayTick;
    }

    /* 建立的任务包含ready态, 将任务加入ready表 */
    if(TASKREADY == (TASKREADY & pstrTcb->strTaskOpt.ucTaskSta))
    {
        pstrList = &gstrReadyTab.astrList[ucTaskPrio];
        pstrNode = &pstrTcb->strTcbQue.strQueHead;
        pstrPrioFlag = &gstrReadyTab.strFlag;

        /* 将该任务添加到ready表中 */
        MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);
    }

    /* 建立的任务包含delay态, 将任务加入delay表 */
    if(TASKDELAY == (TASKDELAY & pstrTcb->strTaskOpt.ucTaskSta))
    {
        /* 非永久等待任务才挂入delay表 */
        if(DELAYWAITFEV != pstrTaskOpt->uiDelayTick)
        {
            /* 更新新建任务的延迟时间 */
            pstrTcb->uiStillTick = guiTick + pstrTaskOpt->uiDelayTick;

            /* 从任务参数里获取delay表节点并加入到delay表 */
            pstrNode = &pstrTcb->strTcbQue.strQueHead;
            MDS_TaskAddToDelayTab(pstrNode);

            /* 置任务在delay表标志 */
            pstrTcb->uiTaskFlag |= DELAYQUEFLAG;
        }
    }

    /* 挂入链表后解锁中断, 允许任务调度 */
    (void)MDS_IntUnlock();

    return pstrTcb;
}

/***********************************************************************************
函数功能: 任务进入delay态, 延迟指定的时间. 调用此函数后发生任务切换.
入口参数: uiDelaytick: 任务需要延迟的时间, 单位: tick.
                       DELAYNOWAIT: 不等待, 仅发生任务切换.
                       DELAYWAITFEV: 任务一直等待.
                       其它: 任务需要延迟的tick数目.
返 回 值: RTN_SUCD: 任务延迟成功.
          RTN_FAIL: 任务延迟失败.
          RTN_TKDLTO: 任务延迟时间耗尽, 超时返回.
          RTN_TKDLBK: 任务延迟状态被中断, 任务返回.
***********************************************************************************/
U32 MDS_TaskDelay(U32 uiDelayTick)
{
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8 ucTaskPrio;

    /* 延迟时间不为0 tick则调度任务 */
    if(DELAYNOWAIT != uiDelayTick)
    {
        /* idle任务不能处于delay状态 */
        if(gpstrCurTcb == gpstrIdleTaskTcb)
        {
            return RTN_FAIL;
        }

        /* 获取当前任务的相关调度参数 */
        ucTaskPrio = gpstrCurTcb->ucTaskPrio;
        pstrList = &gstrReadyTab.astrList[ucTaskPrio];
        pstrPrioFlag = &gstrReadyTab.strFlag;

        (void)MDS_IntLock();

        /* 将当前任务从ready表拆除 */
        pstrNode = MDS_TaskDelFromSchedTab(pstrList, pstrPrioFlag, ucTaskPrio);

        /* 清除任务的ready状态 */
        gpstrCurTcb->strTaskOpt.ucTaskSta &= ~((U8)TASKREADY);

        /* 更新当前任务的延迟时间 */
        gpstrCurTcb->strTaskOpt.uiDelayTick = uiDelayTick;

        /* 非永久等待任务才挂入delay表 */
        if(DELAYWAITFEV != uiDelayTick)
        {
            /* 计算任务延迟结束的时间 */
            gpstrCurTcb->uiStillTick = guiTick + uiDelayTick;

            /* 将当前任务加入到delay表 */
            MDS_TaskAddToDelayTab(pstrNode);

            /* 置任务在delay表标志 */
            gpstrCurTcb->uiTaskFlag |= DELAYQUEFLAG;
        }

        /* 增加任务的delay状态 */
        gpstrCurTcb->strTaskOpt.ucTaskSta |= TASKDELAY;

#ifdef MDS_TASKROUNDROBIN

        /* 将当前任务优先级的轮转调度计数值清0 */
        gauiSliceCnt[gpstrCurTcb->ucTaskPrio] = 0;

#endif

        (void)MDS_IntUnlock();
    }
    else /* 任务不延迟, 仅发生任务切换 */
    {
        /* 借用uiDelayTick变量保存延迟任务的返回值 */
        gpstrCurTcb->strTaskOpt.uiDelayTick = RTN_SUCD;
    }

    /* 使用软中断调度任务 */
    MDS_TaskSwiSched();

    /* 返回延迟任务的返回值, 任务从delay状态返回时返回值被保存在uiDelayTick中 */
    return gpstrCurTcb->strTaskOpt.uiDelayTick;
}

/***********************************************************************************
函数功能: 唤醒且只能唤醒任务的delay状态.
入口参数: pstrTcb: 被唤醒任务的TCB指针.
返 回 值: RTN_SUCD: 任务唤醒操作成功.
          RTN_FAIL: 任务唤醒操作失败.
***********************************************************************************/
U32 MDS_TaskWake(M_TCB* pstrTcb)
{
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8 ucTaskPrio;

    /* 入口参数检查 */
    if(NULL == pstrTcb)
    {
        return RTN_FAIL;
    }

    (void)MDS_IntLock();

    /* 仅可以唤醒任务的delay状态 */
    if(TASKDELAY != (TASKDELAY & pstrTcb->strTaskOpt.ucTaskSta))
    {
        (void)MDS_IntUnlock();

        return RTN_FAIL;
    }

    pstrNode = &pstrTcb->strTcbQue.strQueHead;

    /* 非永久等待任务才从delay表拆除 */
    if(DELAYWAITFEV != pstrTcb->strTaskOpt.uiDelayTick)
    {
        /* 从delay表拆除该任务 */
        (void)MDS_DlistCurNodeDelete(&gstrDelayTab, pstrNode);

        /* 置任务不在delay表标志 */
        pstrTcb->uiTaskFlag &= (~((U32)DELAYQUEFLAG));
    }

    /* 清除任务的delay状态 */
    pstrTcb->strTaskOpt.ucTaskSta &= ~((U8)TASKDELAY);

    /* 借用uiDelayTick变量保存延迟任务的返回值 */
    pstrTcb->strTaskOpt.uiDelayTick = RTN_TKDLBK;

    /* 获取该任务的相关参数 */
    ucTaskPrio = pstrTcb->ucTaskPrio;
    pstrList = &gstrReadyTab.astrList[ucTaskPrio];
    pstrPrioFlag = &gstrReadyTab.strFlag;

    /* 将该任务添加到ready表中 */
    MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);

    /* 增加任务的ready状态 */
    pstrTcb->strTaskOpt.ucTaskSta |= TASKREADY;

    (void)MDS_IntUnlock();

    /* 使用软中断调度任务 */
    MDS_TaskSwiSched();

    return RTN_SUCD;
}

/***********************************************************************************
函数功能: 任务进入pend态, 延迟指定的时间. 调用此函数后发生任务切换.
入口参数: pstrSem: 阻塞任务的信号量指针.
          uiDelaytick: 任务需要延迟的时间, 单位: tick.
                       DELAYNOWAIT: 不等待, 仅发生任务切换.
                       DELAYWAITFEV: 任务一直等待.
                       其它: 任务需要延迟的tick数目.
返 回 值: RTN_SUCD: 任务阻塞成功.
          RTN_FAIL: 任务阻塞失败.
***********************************************************************************/
U32 MDS_TaskPend(M_SEM* pstrSem, U32 uiDelayTick)
{
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8 ucTaskPrio;

    /* idle任务不能处于pend状态 */
    if(gpstrCurTcb == gpstrIdleTaskTcb)
    {
        return RTN_FAIL;
    }

    /* 获取当前任务的相关调度参数 */
    ucTaskPrio = gpstrCurTcb->ucTaskPrio;
    pstrList = &gstrReadyTab.astrList[ucTaskPrio];
    pstrPrioFlag = &gstrReadyTab.strFlag;

    /* 将当前任务从ready表拆除 */
    pstrNode = MDS_TaskDelFromSchedTab(pstrList, pstrPrioFlag, ucTaskPrio);

    /* 清除任务的ready状态 */
    gpstrCurTcb->strTaskOpt.ucTaskSta &= ~((U8)TASKREADY);

    /* 更新当前任务的延迟时间 */
    gpstrCurTcb->strTaskOpt.uiDelayTick = uiDelayTick;

    /* 非永久等待任务才挂入delay表 */
    if(SEMWAITFEV != uiDelayTick)
    {
        gpstrCurTcb->uiStillTick = guiTick + uiDelayTick;

        /* 将当前任务加入到delay表 */
        MDS_TaskAddToDelayTab(pstrNode);

        /* 置任务在delay表标志 */
        gpstrCurTcb->uiTaskFlag |= DELAYQUEFLAG;
    }

    /* 将该任务添加到信号量调度表中 */
    MDS_TaskAddToSemTab(gpstrCurTcb, pstrSem);

    /* 增加任务的pend状态 */
    gpstrCurTcb->strTaskOpt.ucTaskSta |= TASKPEND;

#ifdef MDS_TASKROUNDROBIN

    /* 将当前任务优先级的轮转调度计数值清0 */
    gauiSliceCnt[gpstrCurTcb->ucTaskPrio] = 0;

#endif

    return RTN_SUCD;
}

#ifdef MDS_TASKPRIOINHER

/***********************************************************************************
函数功能: 继承任务的优先级.
入口参数: pstrTcb: 需要继承优先级的任务的TCB.
          ucTaskPrio: 需要继承的优先级.
返 回 值: none.
***********************************************************************************/
void MDS_TaskPrioInheritance(M_TCB* pstrTcb, U8 ucTaskPrio)
{
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8 ucTaskPrioTemp;

    /* 入口参数检查 */
    if(NULL == pstrTcb)
    {
        return;
    }

    /* 如果任务的优先级高于或等于要继承的优先级, 则直接返回, 不继承优先级 */
    if(pstrTcb->ucTaskPrio <= ucTaskPrio)
    {
        return;
    }

    /* 规避编译告警 */
    pstrNode = (M_DLIST*)NULL;
    pstrPrioFlag = (M_PRIOFLAG*)NULL;

    /* 如果任务在ready表中则需要更新ready表, 先从ready表删除 */
    if(TASKREADY == (TASKREADY & pstrTcb->strTaskOpt.ucTaskSta))
    {
        ucTaskPrioTemp = pstrTcb->ucTaskPrio;
        pstrList = &gstrReadyTab.astrList[ucTaskPrioTemp];
        pstrPrioFlag = &gstrReadyTab.strFlag;
        pstrNode = MDS_TaskDelFromSchedTab(pstrList, pstrPrioFlag, ucTaskPrioTemp);
    }

    /* 任务在优先级未继承状态下才备份任务的原始优先级 */
    if(TASKPRIINHFLAG != (pstrTcb->uiTaskFlag & TASKPRIINHFLAG))
    {
        pstrTcb->ucTaskPrioBackup = pstrTcb->ucTaskPrio;
    }

    /* 继承任务的优先级 */
    pstrTcb->ucTaskPrio = ucTaskPrio;

    /* 置继承优先级的任务为优先级继承状态 */
    pstrTcb->uiTaskFlag |= TASKPRIINHFLAG;

    /* 如果该任务在ready表中则需要将更新过优先级的该任务重新加入ready表 */
    if(TASKREADY == (TASKREADY & pstrTcb->strTaskOpt.ucTaskSta))
    {
        pstrList = &gstrReadyTab.astrList[ucTaskPrio];
        MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);
    }
}

/***********************************************************************************
函数功能: 恢复任务继承的优先级.
入口参数: pstrTcb: 需要恢复优先级的任务的TCB.
返 回 值: none.
***********************************************************************************/
void MDS_TaskPrioResume(M_TCB* pstrTcb)
{
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8 ucTaskPrioTemp;

    /* 入口参数检查 */
    if(NULL == pstrTcb)
    {
        return;
    }

    /* 如果任务没有处于优先级继承状态, 则直接返回, 不需要恢复原优先级 */
    if(TASKPRIINHFLAG != (pstrTcb->uiTaskFlag & TASKPRIINHFLAG))
    {
        return;
    }

    /* 规避编译告警 */
    pstrNode = (M_DLIST*)NULL;
    pstrPrioFlag = (M_PRIOFLAG*)NULL;

    /* 需要更新ready表, 先从ready表删除 */
    ucTaskPrioTemp = pstrTcb->ucTaskPrio;
    pstrList = &gstrReadyTab.astrList[ucTaskPrioTemp];
    pstrPrioFlag = &gstrReadyTab.strFlag;
    pstrNode = MDS_TaskDelFromSchedTab(pstrList, pstrPrioFlag, ucTaskPrioTemp);

    /* 将任务优先级恢复为原优先级, 并将任务状态恢复为未继承状态 */
    pstrTcb->ucTaskPrio = pstrTcb->ucTaskPrioBackup;
    pstrTcb->uiTaskFlag &= (~((U32)TASKPRIINHFLAG));

    /* 将更新过优先级的该任务重新加入ready表 */
    pstrList = &gstrReadyTab.astrList[pstrTcb->ucTaskPrio];
    MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, pstrTcb->ucTaskPrio);
}

#endif

#ifdef MDS_TASKROUNDROBIN

/***********************************************************************************
函数功能: 设置同等优先级轮转调度周期值. 若ready表中有与当前任务同等优先级的任务, 并
          且当前任务运行的计数值已经达到周期值, 则将当前任务挂入ready表, 运行ready表
          中下个同等优先级的任务.
入口参数: uiTimeSlice: 同等优先级任务轮转调度时间周期值, 单位: Tick. 若为0则代表不执
          行同等优先级任务的轮转调度.
返 回 值: none.
***********************************************************************************/
void MDS_TaskTimeSlice(U32 uiTimeSlice)
{
    U32 i;

    (void)MDS_IntLock();

    /* 设置轮转调度时间周期值 */
    guiTimeSlice = uiTimeSlice;

    /* 轮转调度的时间计数值清0 */
    for(i = 0; i < PRIORITYNUM; i++)
    {
        gauiSliceCnt[i] = 0;
    }

    (void)MDS_IntUnlock();
}

#endif

#ifdef MDS_INCLUDETASKHOOK

/***********************************************************************************
函数功能: 初始化钩子函数.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_TaskHookInit(void)
{
    /* 初始化钩子变量 */
    gvfTaskCreateHook = (VFHCRT)NULL;
    gvfTaskSwitchHook = (VFHSWT)NULL;
    gvfTaskDeleteHook = (VFHDLT)NULL;
}

/***********************************************************************************
函数功能: 添加任务创建钩子函数.
入口参数: vfFuncPointer: 添加的钩子函数.
返 回 值: none.
***********************************************************************************/
void MDS_TaskCreateHookAdd(VFHCRT vfFuncPointer)
{
    gvfTaskCreateHook = vfFuncPointer;
}

/***********************************************************************************
函数功能: 删除任务创建钩子函数.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_TaskCreateHookDel(void)
{
    gvfTaskCreateHook = (VFHCRT)NULL;
}

/***********************************************************************************
函数功能: 添加任务切换钩子函数.
入口参数: vfFuncPointer: 添加的钩子函数.
返 回 值: none.
***********************************************************************************/
void MDS_TaskSwitchHookAdd(VFHSWT vfFuncPointer)
{
    gvfTaskSwitchHook = vfFuncPointer;
}

/***********************************************************************************
函数功能: 删除任务切换钩子函数.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_TaskSwitchHookDel(void)
{
    gvfTaskSwitchHook = (VFHSWT)NULL;
}

/***********************************************************************************
函数功能: 添加任务删除钩子函数.
入口参数: vfFuncPointer: 添加的钩子函数.
返 回 值: none.
***********************************************************************************/
void MDS_TaskDeleteHookAdd(VFHDLT vfFuncPointer)
{
    gvfTaskDeleteHook = vfFuncPointer;
}

/***********************************************************************************
函数功能: 删除任务删除钩子函数.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_TaskDeleteHookDel(void)
{
    gvfTaskDeleteHook = (VFHDLT)NULL;
}

#endif

