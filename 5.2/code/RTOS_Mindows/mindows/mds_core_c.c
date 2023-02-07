
#include "mindows_inner.h"


U32 guiSystemStatus;                    /* 系统状态 */

M_TASKSCHEDTAB gstrReadyTab;            /* 任务就绪表 */
M_DLIST gstrDelayTab;                   /* 任务延迟表 */

U32 guiTick;                            /* 操作系统的tick变量 */
U8 gucTickSched;                        /* tick调度标志 */

STACKREG* gpstrCurTaskReg;              /* 当前运行的任务的寄存器组指针, 用于从C传递
                                           到汇编 */
STACKREG* gpstrNextTaskReg;             /* 将要运行的任务的寄存器组指针, 用于从C传递
                                           到汇编 */

M_TCB* gpstrCurTcb;                     /* 当前运行的任务的TCB指针 */
M_TCB* gpstrRootTaskTcb;                /* root任务TCB指针 */
M_TCB* gpstrIdleTaskTcb;                /* idle任务TCB指针 */

U32 guiUser;                            /* 操作系统所使用的用户 */

const U8 caucTaskPrioUnmapTab[256] =    /* 优先级反向查找表 */
{
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};


/***********************************************************************************
函数功能: 主函数, C程序从此函数开始运行. 该函数创建系统任务并从非任务状态切换到root
          任务运行.
入口参数: none.
返 回 值: 返回值为0, 但系统不会运行到该函数返回.
***********************************************************************************/
S32 main(void)
{
    /**************************************************************************/
    /*            更多资料请访问    www.ifreecoding.com                       */
    /*                              bbs.ifreecoding.com                       */
    /*                              blog.sina.com.cn/ifreecoding              */
    /**************************************************************************/

    /* 初始化系统变量, 建立操作系统启动所需要的环境 */
    MDS_SystemVarInit();

    /* 开始任务调度, 从前根任务开始执行 */
    MDS_TaskStart(gpstrRootTaskTcb);

    return 0;
}

/***********************************************************************************
函数功能: 初始化系统变量, 建立操作系统启动所需要的环境.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_SystemVarInit(void)
{
    /* 初始化为ROOT用户权限 */
    MDS_SetUser(USERROOT);

    /* 初始化为未进入操作系统状态 */
    guiSystemStatus = SYSTEMNOTSCHEDULE;

    /* 将当前运行任务, root任务和idle任务的TCB初始化为NULL */
    gpstrCurTcb = (M_TCB*)NULL;
    gpstrRootTaskTcb = (M_TCB*)NULL;
    gpstrIdleTaskTcb = (M_TCB*)NULL;

    /* 初始化tick计数从0开始 */
    guiTick = 0;

    /* 初始化为非tick中断调度状态 */
    gucTickSched = TICKSCHEDCLR;

    /* 初始化锁中断计数为0 */
    guiIntLockCounter = 0;

#ifdef MDS_INCLUDETASKHOOK

    /* 初始化钩子函数 */
    MDS_TaskHookInit();

#endif

#ifdef MDS_TASKROUNDROBIN

    /* 默认任务不发生轮转调度 */
    MDS_TaskTimeSlice(0);

#endif

    /* 初始化任务ready表 */
    MDS_TaskSchedTabInit(&gstrReadyTab);

    /* 初始化任务delay表 */
    MDS_DlistInit(&gstrDelayTab);

    /* 创建前根任务 */
    gpstrRootTaskTcb = MDS_TaskCreate(ROOTTASKNAME, MDS_BeforeRootTask, NULL, NULL,
                                      ROOTTASKSTACK, USERHIGHESTPRIO, NULL);

    /* 创建空闲任务 */
    gpstrIdleTaskTcb = MDS_TaskCreate(IDLETASKNAME, MDS_IdleTask, NULL, NULL,
                                      IDLETASKSTACK, LOWESTPRIO, NULL);
}

/***********************************************************************************
函数功能: 前根任务, 在根任务运行前运行, 用于在根任务运行前初始化操作系统用于调度使用
          的硬件, 然后再调用根任务进入用户代码.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void MDS_BeforeRootTask(void* pvPara)
{
    /* 初始化用于操作系统任务调度使用的硬件 */
    MDS_SystemHardwareInit();

    /* 已进入操作系统状态 */
    guiSystemStatus = SYSTEMSCHEDULE;

    /* 即将进入到用户程序, 设置为GUEST用户权限 */
    MDS_SetUser(USERGUEST);

    /* 调用根任务, 进入用户代码 */
    MDS_RootTask();
}

/***********************************************************************************
函数功能: tick中断调度任务函数. tick中断会执行该函数, 由该函数触发PendSv中断进行任务
          调度.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_TaskTick(void)
{
    /* 每个tick中断tick计数加1 */
    guiTick++;

    /* 由tick中断触发的调度, 置为tick中断调度状态 */
    gucTickSched = TICKSCHEDSET;

#ifdef MDS_TASKROUNDROBIN

    /* 如果设置的轮转调度周期值不为0, 则增加当前任务优先级的轮转调度计数值 */
    if((0 != guiTimeSlice) && (NULL != gpstrCurTcb))
    {
        gauiSliceCnt[gpstrCurTcb->ucTaskPrio]++;
    }

#endif

    /* 触发PendSv中断, 在该中断中调度任务 */
    MDS_IntPendSvSet();
}

/***********************************************************************************
函数功能: 为发生的任务切换做准备, 更新任务切换过程中汇编函数所使用的变量.
入口参数: pstrTcb: 即将运行的任务的TCB指针.
返 回 值: none.
***********************************************************************************/
void MDS_TaskSwitch(M_TCB* pstrTcb)
{
    /* 存在当前任务 */
    if(NULL != gpstrCurTcb)
    {
        /* 当前任务的寄存器组地址, 汇编语言通过这个变量备份寄存器 */
        gpstrCurTaskReg = &gpstrCurTcb->strStackReg;
    }
    else /* 当前任务被删除 */
    {
        gpstrCurTaskReg = NULL;
    }

    /* 即将运行任务的寄存器组地址, 汇编语言通过这个变量恢复寄存器 */
    gpstrNextTaskReg = &pstrTcb->strStackReg;

    /* 即将运行任务的TCB指针 */
    gpstrCurTcb = pstrTcb;
}

/***********************************************************************************
函数功能: 实现从非操作系统状态切换到操作系统状态, 执行第一个任务调度.
入口参数: pstrTcb: 即将运行的任务的TCB指针.
返 回 值: none.
***********************************************************************************/
void MDS_TaskStart(M_TCB* pstrTcb)
{
    /* 即将运行任务的寄存器组地址, 汇编语言通过这个变量恢复寄存器 */
    gpstrNextTaskReg = &pstrTcb->strStackReg;

    /* 即将运行任务的TCB指针 */
    gpstrCurTcb = pstrTcb;

    /* 切换到操作系统状态 */
    MDS_SwitchToTask();
}

/***********************************************************************************
函数功能: 初始化任务调度表.
入口参数: pstrSchedTab: 任务调度表指针.
返 回 值: none.
***********************************************************************************/
void MDS_TaskSchedTabInit(M_TASKSCHEDTAB* pstrSchedTab)
{
    U32 i;

    /* 初始化每个优先级任务的ready表 */
    for(i = 0; i < PRIORITYNUM; i++)
    {
        MDS_DlistInit(&pstrSchedTab->astrList[i]);
    }

    /* 初始化ready表优先级标志 */
#if PRIORITYNUM >= PRIORITY128

    for(i = 0; i < PRIOFLAGGRP1; i++)
    {
        pstrSchedTab->strFlag.aucPrioFlagGrp1[i] = 0;
    }

    for(i = 0; i < PRIOFLAGGRP2; i++)
    {
        pstrSchedTab->strFlag.aucPrioFlagGrp2[i] = 0;
    }

    pstrSchedTab->strFlag.ucPrioFlagGrp3 = 0;

#elif PRIORITYNUM >= PRIORITY16

    for(i = 0; i< PRIOFLAGGRP1; i++)
    {
        pstrSchedTab->strFlag.aucPrioFlagGrp1[i] = 0;
    }

    pstrSchedTab->strFlag.ucPrioFlagGrp2 = 0;

#else

    pstrSchedTab->strFlag.ucPrioFlagGrp1 = 0;

#endif
}

/***********************************************************************************
函数功能: 将任务按照优先级添加到任务调度表.
入口参数: pstrList: 调度表指针, 将任务添加到该调度表.
          pstrNode: 需要添加到调度表的任务节点指针.
          pstrPrioFlag: 该调度表对应的优先级标志表指针.
          ucTaskPrio: 需要添加的任务的优先级.
返 回 值: none.
***********************************************************************************/
void MDS_TaskAddToSchedTab(M_DLIST* pstrList, M_DLIST* pstrNode,
                           M_PRIOFLAG* pstrPrioFlag, U8 ucTaskPrio)
{
    /* 将该任务节点添加到调度表中 */
    MDS_DlistNodeAdd(pstrList, pstrNode);

    /* 设置该任务对应调度表的优先级标志表 */
    MDS_TaskSetPrioFlag(pstrPrioFlag, ucTaskPrio);
}

/***********************************************************************************
函数功能: 将任务添加到delay表, 按照任务延迟时间从少到多添加到delay表中.
入口参数: pstrNode: 需要添加的任务节点指针.
返 回 值: none.
***********************************************************************************/
void MDS_TaskAddToDelayTab(M_DLIST* pstrNode)
{
    M_DLIST* pstrTempNode;
    M_DLIST* pstrNextNode;
    M_TCBQUE* pstrTcbQue;
    U32 uiStillTick;
    U32 uiTempStillTick;

    /* 获取delay表中的第一个任务节点指针 */
    pstrTempNode = MDS_DlistEmpInq(&gstrDelayTab);

    /* delay表非空 */
    if(NULL != pstrTempNode)
    {
        /* 获取要加入delay表的任务的延迟时间 */
        pstrTcbQue = (M_TCBQUE*)pstrNode;
        uiStillTick = pstrTcbQue->pstrTcb->uiStillTick;

        /* 寻找要加入delay表的任务在delay表中的位置 */
        while(1)
        {
            /* 获取delay表中任务的延迟时间 */
            pstrTcbQue = (M_TCBQUE*)pstrTempNode;
            uiTempStillTick = pstrTcbQue->pstrTcb->uiStillTick;

            /* 在以下各条件中判断要加入delay表的任务应该插入的位置 */
            if(uiStillTick < uiTempStillTick)
            {
                if(uiStillTick > guiTick)
                {
                    MDS_DlistCurNodeInsert(&gstrDelayTab, pstrTempNode, pstrNode);

                    return;
                }
                else if(uiStillTick < guiTick)
                {
                    if(guiTick > uiTempStillTick)
                    {
                        MDS_DlistCurNodeInsert(&gstrDelayTab, pstrTempNode,
                                               pstrNode);

                        return;
                    }
                }
            }
            else if(uiStillTick > uiTempStillTick)
            {
                if(uiStillTick > guiTick)
                {
                    if(guiTick > uiTempStillTick)
                    {
                        MDS_DlistCurNodeInsert(&gstrDelayTab, pstrTempNode,
                                               pstrNode);

                        return;
                    }
                }
            }

            /* delay表中取出的任务延迟时间小于需要加入delay表的任务延迟时间, 取出下
               个任务节点继续判断 */
            pstrNextNode = MDS_DlistNextNodeEmpInq(&gstrDelayTab, pstrTempNode);

            /* 搜索到delay表的最后一个节点, 将需要加入delay表的任务加入到表尾 */
            if(NULL == pstrNextNode)
            {
                MDS_DlistNodeAdd(&gstrDelayTab, pstrNode);

                return;
            }
            else /* 更新delay表下个任务节点继续判断 */
            {
                pstrTempNode = pstrNextNode;
            }
        }
    }
    else /* delay表为空则直接将需要加入delay表的任务加入到delay表尾 */
    {
        MDS_DlistNodeAdd(&gstrDelayTab, pstrNode);

        return;
    }
}

/***********************************************************************************
函数功能: 从任务调度表删除任务.
入口参数: pstrList: 调度表指针, 从该链表删除任务.
          pstrPrioFlag: 调度表对应的优先级标志表指针.
          ucTaskPrio: 要删除任务的优先级.
返 回 值: 被删除的任务的节点指针.
***********************************************************************************/
M_DLIST* MDS_TaskDelFromSchedTab(M_DLIST* pstrList, M_PRIOFLAG* pstrPrioFlag,
                                 U8 ucTaskPrio)
{
    M_DLIST* pstrDelNode;

    /* 将该任务节点从调度表中删除 */
    pstrDelNode = MDS_DlistNodeDelete(pstrList);

    /* 如果调度表中该优先级为空则清除优先级标志 */
    if(NULL == MDS_DlistEmpInq(pstrList))
    {
        MDS_TaskClrPrioFlag(pstrPrioFlag, ucTaskPrio);
    }

    /* 返回被删除任务的节点指针 */
    return pstrDelNode;
}

/***********************************************************************************
函数功能: 将任务添加到sem调度表.
入口参数: pstrTcb: 任务的TCB.
          pstrSem: 操作的信号量指针.
返 回 值: none.
***********************************************************************************/
void MDS_TaskAddToSemTab(M_TCB* pstrTcb, M_SEM* pstrSem)
{
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8 ucTaskPrio;

    /* 信号量是采用优先级调度算法 */
    if(SEMPRIO == (SEMSCHEDOPTMASK & pstrSem->uiSemOpt))
    {
        /* 获取任务的相关参数 */
        ucTaskPrio = pstrTcb->ucTaskPrio;
        pstrList = &pstrSem->strSemTab.astrList[ucTaskPrio];
        pstrNode = &pstrTcb->strSemQue.strQueHead;
        pstrPrioFlag = &pstrSem->strSemTab.strFlag;

        /* 添加到sem调度表 */
        MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);
    }
    else /* 信号量采用先进先出调度算法 */
    {
        /* 获取任务的相关参数, 使用0优先级链表作为先进现出链表 */
        pstrList = &pstrSem->strSemTab.astrList[LOWESTPRIO];
        pstrNode = &pstrTcb->strSemQue.strQueHead;

        /* 添加到sem调度表 */
        MDS_DlistNodeAdd(pstrList, pstrNode);
    }

    /* 保存阻塞任务的信号量 */
    pstrTcb->pstrSem = pstrSem;
}

/***********************************************************************************
函数功能: 将任务从sem调度表删除.
入口参数: pstrTcb: 任务的TCB.
返 回 值: 被删除任务的节点指针.
***********************************************************************************/
M_DLIST* MDS_TaskDelFromSemTab(M_TCB* pstrTcb)
{
    M_SEM* pstrSem;
    M_DLIST* pstrList;
    M_PRIOFLAG* pstrPrioFlag;
    U8 ucTaskPrio;

    /* 获取阻塞任务的信号量 */
    pstrSem = pstrTcb->pstrSem;

    /* 信号量是采用优先级调度算法 */
    if(SEMPRIO == (SEMSCHEDOPTMASK & pstrSem->uiSemOpt))
    {
        /* 获取任务的相关参数 */
        ucTaskPrio = pstrTcb->ucTaskPrio;
        pstrList = &pstrSem->strSemTab.astrList[ucTaskPrio];
        pstrPrioFlag = &pstrSem->strSemTab.strFlag;

        /* 从sem调度表删除该任务 */
        return MDS_TaskDelFromSchedTab(pstrList, pstrPrioFlag, ucTaskPrio);
    }
    else /* 信号量采用先进先出调度算法 */
    {
        /* 获取任务的相关参数, 使用0优先级链表作为先进现出链表 */
        pstrList = &pstrSem->strSemTab.astrList[LOWESTPRIO];

        /* 从sem调度表删除该任务 */
        return MDS_DlistNodeDelete(pstrList);
    }
}

/***********************************************************************************
函数功能: 在被信号量阻塞的任务中获取一个需要被最先释放的任务.
入口参数: pstrSem: 信号量指针.
返 回 值: 需要释放的任务的TCB指针, 若没有需要释放的任务则返回NULL.
***********************************************************************************/
M_TCB* MDS_SemGetActiveTask(M_SEM* pstrSem)
{
    M_DLIST* pstrNode;
    M_TCBQUE* pstrTaskQue;
    U8 ucTaskPrio;

    /* 信号量采用优先级调度算法 */
    if(SEMPRIO == (SEMSCHEDOPTMASK & pstrSem->uiSemOpt))
    {
        /* 获取信号量表中优先级最高的任务TCB */
        ucTaskPrio = MDS_TaskGetHighestPrio(&pstrSem->strSemTab.strFlag);
    }
    else /* 信号量采用先进先出调度算法 */
    {
        /* 采用0优先级的链表 */
        ucTaskPrio = LOWESTPRIO;
    }

    /* 查询sem表是否为空 */
    pstrNode = MDS_DlistEmpInq(&pstrSem->strSemTab.astrList[ucTaskPrio]);

    /* 信号量中没有被阻塞的任务, 不需要释放任务 */
    if(NULL == pstrNode)
    {
        return (M_TCB*)NULL;
    }
    else /* 信号量中有被阻塞的任务, 返回需要被释放的任务的TCB指针 */
    {
        pstrTaskQue = (M_TCBQUE*)pstrNode;

        return pstrTaskQue->pstrTcb;
    }
}

/***********************************************************************************
函数功能: 任务调度函数, 在此函数实现任务各种状态转换的调度.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_TaskSched(void)
{
    M_TCB* pstrTcb;

    /* 由tick中断触发的调度 */
    if(TICKSCHEDSET == gucTickSched)
    {
        /* 清tick调度状态 */
        gucTickSched = TICKSCHEDCLR;

        /* 调度delay表任务 */
        MDS_TaskDelayTabSched();
    }

    /* 调度ready表任务 */
    pstrTcb = MDS_TaskReadyTabSched();

#ifdef MDS_INCLUDETASKHOOK

    /* 如果任务切换钩子已经挂接函数则执行该函数 */
    if((VFHSWT)NULL != gvfTaskSwitchHook)
    {
        /* 执行任务切换钩子函数 */
        gvfTaskSwitchHook(gpstrCurTcb, pstrTcb);
    }

#endif

    /* 任务切换 */
    MDS_TaskSwitch(pstrTcb);
}

/***********************************************************************************
函数功能: 对ready表进行调度.
入口参数: none.
返 回 值: 即将运行的任务的TCB指针.
***********************************************************************************/
M_TCB* MDS_TaskReadyTabSched(void)
{
    M_TCB* pstrTcb;
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
#ifdef MDS_TASKROUNDROBIN
    M_DLIST* pstrNextNode;
#endif
    M_TCBQUE* pstrTaskQue;
    U8 ucTaskPrio;

    /* 获取ready表中优先级最高的任务的TCB */
    ucTaskPrio = MDS_TaskGetHighestPrio(&gstrReadyTab.strFlag);
    pstrList = &gstrReadyTab.astrList[ucTaskPrio];
    pstrNode = MDS_DlistEmpInq(pstrList);
    pstrTaskQue = (M_TCBQUE*)pstrNode;
    pstrTcb = pstrTaskQue->pstrTcb;

#ifdef MDS_TASKROUNDROBIN

    /* 需要执行任务轮转调度 */
    if(0 != guiTimeSlice)
    {
        /* 下个调度任务仍为当前任务 */
        if(gpstrCurTcb == pstrTcb)
        {
            /* 同一任务已经达到轮转调度的周期值 */
            if(gauiSliceCnt[gpstrCurTcb->ucTaskPrio] >= guiTimeSlice)
            {
                /* 将当前任务优先级的轮转调度计数值清0 */
                gauiSliceCnt[gpstrCurTcb->ucTaskPrio] = 0;

                /* 取出与当前任务同等优先级的任务 */
                pstrNextNode = MDS_DlistNextNodeEmpInq(pstrList, pstrNode);

                /* 若有, 则需要发生任务轮转调度 */
                if(NULL != pstrNextNode)
                {
                    /* 将当前任务放到ready表中当前最高优先级任务链表的最后 */
                    /* 从链表删除当前任务节点 */
                    (void)MDS_DlistNodeDelete(pstrList);

                    /* 将当前任务节点加入链表尾 */
                    MDS_DlistNodeAdd(pstrList, pstrNode);

                    /* 更新下个任务 */
                    pstrTaskQue = (M_TCBQUE*)pstrNextNode;
                    pstrTcb = pstrTaskQue->pstrTcb;
                }
            }
        }
    }

#endif

    return pstrTcb;
}

/***********************************************************************************
函数功能: 对delay表进行调度.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_TaskDelayTabSched(void)
{
    M_TCB* pstrTcb;
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_DLIST* pstrDelayNode;
    M_DLIST* pstrNextNode;
    M_PRIOFLAG* pstrPrioFlag;
    M_TCBQUE* pstrTcbQue;
    U32 uiTick;
    U8 ucTaskPrio;

    /* 获取delay表中的任务节点 */
    pstrDelayNode = MDS_DlistEmpInq(&gstrDelayTab);

    /* delay表中有任务, 调度delay表中的任务 */
    if(NULL != pstrDelayNode)
    {
        /* 判断delay表中任务的延迟时间是否结束 */
        while(1)
        {
            /* 获取delay表中任务的延迟时间 */
            pstrTcbQue = (M_TCBQUE*)pstrDelayNode;
            pstrTcb = pstrTcbQue->pstrTcb;
            uiTick = pstrTcb->uiStillTick;

            /* 该任务延迟时间结束, 从delay表中删除并加入到调度表中 */
            if(uiTick == guiTick)
            {
                /* 从delay表拆除该任务 */
                pstrNextNode = MDS_DlistCurNodeDelete(&gstrDelayTab, pstrDelayNode);

                /* 置任务不在delay表标志 */
                pstrTcb->uiTaskFlag &= (~((U32)DELAYQUEFLAG));

                /* 更新该任务的delay和pend相关参数 */
                /* 如果任务拥有delay状态则从delay状态恢复 */
                if(TASKDELAY == (TASKDELAY & pstrTcb->strTaskOpt.ucTaskSta))
                {
                    /* 清除任务的delay状态 */
                    pstrTcb->strTaskOpt.ucTaskSta &= ~((U8)TASKDELAY);

                    /* 借用uiDelayTick变量保存delay任务的返回值 */
                    pstrTcb->strTaskOpt.uiDelayTick = RTN_TKDLTO;
                }
                /* 如果任务拥有pend状态则从pend状态恢复 */
                else if(TASKPEND == (TASKPEND & pstrTcb->strTaskOpt.ucTaskSta))
                {
                    /* 从sem调度表拆除任务 */
                    (void)MDS_TaskDelFromSemTab(pstrTcb);

                    /* 清除任务的pend状态 */
                    pstrTcb->strTaskOpt.ucTaskSta &= ~((U8)TASKPEND);

                    /* 借用uiDelayTick变量保存pend任务的返回值 */
                    pstrTcb->strTaskOpt.uiDelayTick = RTN_SMTKTO;
                }

                /* 获取该任务的相关参数 */
                pstrNode = &pstrTcb->strTcbQue.strQueHead;
                ucTaskPrio = pstrTcb->ucTaskPrio;
                pstrList = &gstrReadyTab.astrList[ucTaskPrio];
                pstrPrioFlag = &gstrReadyTab.strFlag;

                /* 将该任务添加到ready表中 */
                MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);

                /* 增加任务的ready状态 */
                pstrTcb->strTaskOpt.ucTaskSta |= TASKREADY;

                /* delay表已经调度完毕, 结束对delay表的调度 */
                if(NULL == pstrNextNode)
                {
                    break;
                }
                else /* delay表没调度完, 更新下个节点继续判断 */
                {
                    pstrDelayNode = pstrNextNode;
                }
            }
            else /* 所有任务的延迟时间没有结束, 结束对delay表的调度 */
            {
                break;
            }
        }
    }
}

/***********************************************************************************
函数功能: 将任务添加到任务调度表对应的优先级标志表中.
入口参数: pstrPrioFlag: 调度表对应的优先级标志表指针.
          ucTaskPrio: 任务的优先级.
返 回 值: none.
***********************************************************************************/
void MDS_TaskSetPrioFlag(M_PRIOFLAG* pstrPrioFlag, U8 ucTaskPrio)
{
#if PRIORITYNUM >= PRIORITY128
    U8 ucPrioFlagGrp1;
    U8 ucPrioFlagGrp2;
    U8 ucPosInGrp1;
    U8 ucPosInGrp2;
    U8 ucPosInGrp3;
#elif PRIORITYNUM >= PRIORITY16
    U8 ucPrioFlagGrp1;
    U8 ucPosInGrp1;
    U8 ucPosInGrp2;
#endif

    /* 设置调度表对应的优先级标志表 */
#if PRIORITYNUM >= PRIORITY128

    /* 获取优先级标志在第一组和第二组中的组号 */
    ucPrioFlagGrp1 = ucTaskPrio / 8;
    ucPrioFlagGrp2 = ucPrioFlagGrp1 / 8;

    /* 获取优先级标志在每一组中的位置 */
    ucPosInGrp1 = ucTaskPrio % 8;
    ucPosInGrp2 = ucPrioFlagGrp1 % 8;
    ucPosInGrp3 = ucPrioFlagGrp2;

    /* 在每一组中设置优先级标志 */
    pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1] |= (U8)(1 << ucPosInGrp1);
    pstrPrioFlag->aucPrioFlagGrp2[ucPrioFlagGrp2] |= (U8)(1 << ucPosInGrp2);
    pstrPrioFlag->ucPrioFlagGrp3 |= (U8)(1 << ucPosInGrp3);

#elif PRIORITYNUM >= PRIORITY16

    ucPrioFlagGrp1 = ucTaskPrio / 8;

    ucPosInGrp1 = ucTaskPrio % 8;
    ucPosInGrp2 = ucPrioFlagGrp1;

    pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1] |= (U8)(1 << ucPosInGrp1);
    pstrPrioFlag->ucPrioFlagGrp2 |= (U8)(1 << ucPosInGrp2);

#else

    pstrPrioFlag->ucPrioFlagGrp1 |= (U8)(1 << ucTaskPrio);

#endif
}

/***********************************************************************************
函数功能: 将该任务从任务调度表对应的优先级标志表中清除.
入口参数: pstrPrioFlag: 调度表对应的优先级标志表指针.
          ucTaskPrio: 任务的优先级.
返 回 值: none.
***********************************************************************************/
void MDS_TaskClrPrioFlag(M_PRIOFLAG* pstrPrioFlag, U8 ucTaskPrio)
{
#if PRIORITYNUM >= PRIORITY128
    U8 ucPrioFlagGrp1;
    U8 ucPrioFlagGrp2;
    U8 ucPosInGrp1;
    U8 ucPosInGrp2;
    U8 ucPosInGrp3;
#elif PRIORITYNUM >= PRIORITY16
    U8 ucPrioFlagGrp1;
    U8 ucPosInGrp1;
    U8 ucPosInGrp2;
#endif

    /* 清除调度表对应的优先级标志表 */
#if PRIORITYNUM >= PRIORITY128

    /* 获取优先级标志在第一组和第二组中的组号 */
    ucPrioFlagGrp1 = ucTaskPrio / 8;
    ucPrioFlagGrp2 = ucPrioFlagGrp1 / 8;

    /* 获取优先级标志在每一组中的位置 */
    ucPosInGrp1 = ucTaskPrio % 8;
    ucPosInGrp2 = ucPrioFlagGrp1 % 8;
    ucPosInGrp3 = ucPrioFlagGrp2;

    /* 在每一组中清除优先级标志 */
    pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1] &= ~((U8)(1 << ucPosInGrp1));
    if(0 == pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1])
    {
        pstrPrioFlag->aucPrioFlagGrp2[ucPrioFlagGrp2] &= ~((U8)(1 << ucPosInGrp2));
        if(0 == pstrPrioFlag->aucPrioFlagGrp2[ucPrioFlagGrp2])
        {
            pstrPrioFlag->ucPrioFlagGrp3 &= ~((U8)(1 << ucPosInGrp3));
        }
    }

#elif PRIORITYNUM >= PRIORITY16

    ucPrioFlagGrp1 = ucTaskPrio / 8;

    ucPosInGrp1 = ucTaskPrio % 8;
    ucPosInGrp2 = ucPrioFlagGrp1;

    pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1] &= ~((U8)(1 << ucPosInGrp1));
    if(0 == pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1])
    {
        pstrPrioFlag->ucPrioFlagGrp2 &= ~((U8)(1 << ucPosInGrp2));
    }

#else

    pstrPrioFlag->ucPrioFlagGrp1 &= ~((U8)(1 << ucTaskPrio));

#endif
}

/***********************************************************************************
函数功能: 获取任务调度表中任务的最高优先级.
入口参数: pstrPrioFlag: 调度表的优先级标志表指针.
返 回 值: 任务调度表中的最高优先级.
***********************************************************************************/
U8 MDS_TaskGetHighestPrio(M_PRIOFLAG* pstrPrioFlag)
{
#if PRIORITYNUM >= PRIORITY128
    U8 ucPrioFlagGrp1;
    U8 ucPrioFlagGrp2;
    U8 ucHighestFlagInGrp1;
#elif PRIORITYNUM >= PRIORITY16
    U8 ucPrioFlagGrp1;
    U8 ucHighestFlagInGrp1;
#endif

    /* 获取任务调度表中的最高优先级 */
#if PRIORITYNUM >= PRIORITY128

    ucPrioFlagGrp2 = caucTaskPrioUnmapTab[pstrPrioFlag->ucPrioFlagGrp3];

    ucPrioFlagGrp1 =
                caucTaskPrioUnmapTab[pstrPrioFlag->aucPrioFlagGrp2[ucPrioFlagGrp2]];

    ucHighestFlagInGrp1 = caucTaskPrioUnmapTab[pstrPrioFlag->aucPrioFlagGrp1
                                             [ucPrioFlagGrp2 * 8 + ucPrioFlagGrp1]];

    return (U8)((ucPrioFlagGrp2 * 8 + ucPrioFlagGrp1) * 8 + ucHighestFlagInGrp1);

#elif PRIORITYNUM >= PRIORITY16

    ucPrioFlagGrp1 = caucTaskPrioUnmapTab[pstrPrioFlag->ucPrioFlagGrp2];

    ucHighestFlagInGrp1 =
                caucTaskPrioUnmapTab[pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1]];

    return (U8)(ucPrioFlagGrp1 * 8 + ucHighestFlagInGrp1);

#else

    return caucTaskPrioUnmapTab[pstrPrioFlag->ucPrioFlagGrp1];

#endif
}

/***********************************************************************************
函数功能: 获取系统当前的tick.
入口参数: none.
返 回 值: 系统当前的tick.
***********************************************************************************/
U32 MDS_GetSystemTick(void)
{
    return guiTick;
}

/***********************************************************************************
函数功能: 获取系统当前任务的TCB指针.
入口参数: none.
返 回 值: 系统当前的TCB指针.
***********************************************************************************/
M_TCB* MDS_GetCurrentTcb(void)
{
    return gpstrCurTcb;
}

/***********************************************************************************
函数功能: 获取根任务的TCB指针.
入口参数: none.
返 回 值: 根任务的TCB指针.
***********************************************************************************/
M_TCB* MDS_GetRootTcb(void)
{
    return gpstrRootTaskTcb;
}

/***********************************************************************************
函数功能: 获取空闲任务的TCB指针.
入口参数: none.
返 回 值: 空闲任务的TCB指针.
***********************************************************************************/
M_TCB* MDS_GetIdleTcb(void)
{
    return gpstrIdleTaskTcb;
}

/***********************************************************************************
函数功能: 设置用户.
入口参数: uiUser: 需要设置的用户.
                  USERROOT: ROOT用户, 具有所有权限.
                  USERGUEST: GUEST用户, 具有部分权限.
返 回 值: none.
***********************************************************************************/
void MDS_SetUser(U32 uiUser)
{
    guiUser = uiUser;
}

/***********************************************************************************
函数功能: 获取用户.
入口参数: none.
返 回 值: 获取到的用户.
          USERROOT: ROOT用户, 具有所有权限.
          USERGUEST: GUEST用户, 具有部分权限.
***********************************************************************************/
U32 MDS_GetUser(void)
{
    return guiUser;
}

