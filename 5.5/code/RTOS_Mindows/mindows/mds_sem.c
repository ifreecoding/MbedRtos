
#include <stdlib.h>
#include "mindows_inner.h"


/***********************************************************************************
函数功能: 创建一个信号量.
入口参数: pstrSem: 需要创建的信号量的指针, 需要是M_SEM类型, 若为NULL则由该函数自行申
                   请内存创建.
          uiSemOpt: 创建信号量所用的选项.
                    优先级:
                    SEMPRIO: 信号量队列采用优先级队列, 释放信号量时先激活被该信号量
                             阻塞的优先级最高的任务.
                    SEMFIFO: 信号量队列采用先进先出队列, 释放信号量时先激活最先被该
                             信号量阻塞的任务.
                    信号量类型:
                    SEMBIN: 创建二进制信号量.
                    SEMCNT: 创建计数信号量.
                    SEMMUT: 创建互斥信号量.
                    任务优先级继承:
                    SEMPRIINH: 任务被阻塞使用任务优先级继承功能. 该参数只可用在
                               SEMMUT信号量SEMPRIO模式下.
          uiInitVal: 信号量的初始值.
                     SEMEMPTY: 信号量为空状态, 不可获取. 该参数不可用在SEMMUT信号量.
                     SEMFULL: 信号量为满状态, 可获取.
                     其它: 信号量初始的计数值. 该参数只可用在SEMCNT信号量.
返 回 值: NULL: 创建信号量失败.
          其它: 创建信号量成功, 返回值为创建的信号量的指针.
***********************************************************************************/
M_SEM* MDS_SemCreate(M_SEM* pstrSem, U32 uiSemOpt, U32 uiInitVal)
{
    U8* pucSemMemAddr;

    /* 信号量选项检查 */
    if(((SEMBIN != (SEMTYPEMASK & uiSemOpt))
        && (SEMCNT != (SEMTYPEMASK & uiSemOpt))
        && (SEMMUT != (SEMTYPEMASK & uiSemOpt)))
       || ((SEMFIFO != (SEMSCHEDOPTMASK & uiSemOpt))
           && (SEMPRIO != (SEMSCHEDOPTMASK & uiSemOpt)))
       || (((SEMMUT != (SEMTYPEMASK & uiSemOpt))
            || (SEMPRIO != (SEMSCHEDOPTMASK & uiSemOpt)))
           && (SEMPRIINH == (SEMPRIINHMASK & uiSemOpt))))
    {
        return (M_SEM*)NULL;
    }

    /* 二进制信号量初始值只能是空或者满 */
    if(SEMBIN == (SEMTYPEMASK & uiSemOpt))
    {
        if((SEMEMPTY != uiInitVal) && (SEMFULL != uiInitVal))
        {
            return (M_SEM*)NULL;
        }
    }
    /* 互斥信号量初始值只能是满 */
    else if(SEMMUT == (SEMTYPEMASK & uiSemOpt))
    {
        if(SEMFULL != uiInitVal)
        {
            return (M_SEM*)NULL;
        }
    }

    /* 传入指针为空, 需要自己申请内存 */
    if(NULL == pstrSem)
    {
        (void)MDS_IntLock();

        pucSemMemAddr = malloc(sizeof(M_SEM));
        if(NULL == pucSemMemAddr)
        {
            (void)MDS_IntUnlock();

            /* 申请不到内存, 返回失败 */
            return (M_SEM*)NULL;
        }

        (void)MDS_IntUnlock();

        /* 信号量指向申请的内存 */
        pstrSem = (M_SEM*)pucSemMemAddr;
    }
    else /* 由入口参数传入信号量使用的内存, 无需自己申请 */
    {
        /* 将信号量内存地址置为空 */
        pucSemMemAddr = (U8*)NULL;
    }

    /* 初始化信号量调度表 */
    MDS_TaskSchedTabInit(&pstrSem->strSemTab);

    /* 初始化信号量初始值 */
    pstrSem->uiCounter = uiInitVal;

    /* 初始化信号量参数 */
    pstrSem->uiSemOpt = uiSemOpt;

    /* 保存信号量内存的起始地址 */
    pstrSem->pucSemMem = pucSemMemAddr;

    /* 没有任务获取到互斥信号量 */
    pstrSem->pstrSemTask = (M_TCB*)NULL;

    return pstrSem;
}

/***********************************************************************************
函数功能: 获取一个信号量. 当信号量不为空时可获取, 当信号量为空时无法获取, 可能会使任
          务变为pend状态, 发生任务切换.
入口参数: pstrSem: 要获取的信号量指针.
          uiDelayTick: 获取信号量等待的时间, 单位: tick.
                       SEMNOWAIT: 不等待, 无论是否获取到信号量都继续执行该任务.
                       SEMWAITFEV: 永久等待, 若获取不到信号量则该任务切换为pend状态
                                   一直等待, 期间若能获取到信号量则转换为ready态.
                       其它: 若无法获取到信号量则该任务切换为pend状态, 若在该时间内
                             可获取到信号量则结束该任务的pend状态, 返回RTN_SUCD, 若
                             在该时间内获取不到信号量则结束该任务的pend状态, 返回
                             RTN_SMTKTO.
返 回 值: RTN_SUCD: 在延迟时间内获取到信号量.
          RTN_FAIL: 获取信号量失败.
          RTN_SMTKTO: 等待信号量的时间耗尽, 超时返回.
          RTN_SMTKRT: 使用不等待时间参数没有获取到信号量, 函数立刻返回.
          RTN_SMTKDL: 信号量被删除.
          RTN_SMTKOV: 互斥信号量计数值溢出.
***********************************************************************************/
U32 MDS_SemTake(M_SEM* pstrSem, U32 uiDelayTick)
{
    /* 入口参数检查 */
    if(NULL == pstrSem)
    {
        return RTN_FAIL;
    }

    /* 在中断中使用信号量 */
    if(RTN_SUCD == MDS_RunInInt())
    {
        /* 中断中不能使用互斥信号量 */
        if(SEMMUT == (SEMTYPEMASK & pstrSem->uiSemOpt))
        {
            return RTN_FAIL;
        }
        else
        {
            /* 中断中使用二进制信号量和计数信号量时不能有等待时间 */
            if(SEMNOWAIT != uiDelayTick)
            {
                return RTN_FAIL;
            }
        }
    }

    (void)MDS_IntLock();

    /* 获取信号量时不等待时间 */
    if(SEMNOWAIT == uiDelayTick)
    {
        /* 二进制信号量 */
        if(SEMBIN == (SEMTYPEMASK & pstrSem->uiSemOpt))
        {
            /* 信号量为满, 可获取到信号量 */
            if(SEMFULL == pstrSem->uiCounter)
            {
                /* 获取到信号量后将信号量置为空 */
                pstrSem->uiCounter = SEMEMPTY;

                (void)MDS_IntUnlock();

                return RTN_SUCD;
            }
            else /* 信号量为空, 无法获取到信号量 */
            {
                (void)MDS_IntUnlock();

                return RTN_SMTKRT;
            }
        }
        /* 计数信号量 */
        else if(SEMCNT == (SEMTYPEMASK & pstrSem->uiSemOpt))
        {
            /* 信号量不为空, 可获取到信号量 */
            if(SEMEMPTY != pstrSem->uiCounter)
            {
                /* 获取到信号量后将信号量计数值-1 */
                pstrSem->uiCounter--;

                (void)MDS_IntUnlock();

                return RTN_SUCD;
            }
            else /* 信号量为空, 无法获取到信号量 */
            {
                (void)MDS_IntUnlock();

                return RTN_SMTKRT;
            }
        }
        else /* 互斥信号量 */
        {
            /* 信号量为满, 可获取到信号量 */
            if(SEMFULL == pstrSem->uiCounter)
            {
                /* 获取到信号量后将信号量计数值-1 */
                pstrSem->uiCounter--;

                /* 将互斥信号量与任务关联 */
                pstrSem->pstrSemTask = gpstrCurTcb;

                (void)MDS_IntUnlock();

                return RTN_SUCD;
            }
            else /* 信号量不为满, 说明被一个任务获取过 */
            {
                /* 若该信号量已经被本任务获取则可以继续获取 */
                if(pstrSem->pstrSemTask == gpstrCurTcb)
                {
                    /* 信号量计数未空 */
                    if(SEMEMPTY != pstrSem->uiCounter)
                    {
                        /* 信号量计数值-1 */
                        pstrSem->uiCounter--;

                        (void)MDS_IntUnlock();

                        return RTN_SUCD;
                    }
                    else /* 信号量计数已空, 返回失败 */
                    {
                        (void)MDS_IntUnlock();

                        return RTN_SMTKOV;
                    }
                }
                else /* 被其它任务获取则返回无法获取信号量 */
                {
                    (void)MDS_IntUnlock();

                    return RTN_SMTKRT;
                }
            }
        }
    }
    else /* 获取信号量时需要等待时间 */
    {
        /* 二进制信号量 */
        if(SEMBIN == (SEMTYPEMASK & pstrSem->uiSemOpt))
        {
            /* 信号量为满, 可获取到信号量 */
            if(SEMFULL == pstrSem->uiCounter)
            {
                /* 获取到信号量后将信号量置为空 */
                pstrSem->uiCounter = SEMEMPTY;

                (void)MDS_IntUnlock();

                return RTN_SUCD;
            }
            else /* 信号量为空, 无法获取到信号量, 需要切换任务 */
            {
                /* 将任务置为pend状态 */
                if(RTN_FAIL == MDS_TaskPend(pstrSem, uiDelayTick))
                {
                    (void)MDS_IntUnlock();

                    /* 任务pend失败 */
                    return RTN_FAIL;
                }

                (void)MDS_IntUnlock();

                /* 使用软中断调度任务 */
                MDS_TaskSwiSched();

                /* 任务pend返回值, 该值在任务pend状态结束时被保存在uiDelayTick中 */
                return gpstrCurTcb->strTaskOpt.uiDelayTick;
            }
        }
        /* 计数信号量 */
        else if(SEMCNT == (SEMTYPEMASK & pstrSem->uiSemOpt))
        {
            /* 信号量不为空, 可获取到信号量 */
            if(SEMEMPTY != pstrSem->uiCounter)
            {
                /* 获取到信号量后将信号量计数值-1 */
                pstrSem->uiCounter--;

                (void)MDS_IntUnlock();

                return RTN_SUCD;
            }
            else /* 信号量为空, 无法获取到信号量, 需要切换任务 */
            {
                /* 将任务置为pend状态 */
                if(RTN_FAIL == MDS_TaskPend(pstrSem, uiDelayTick))
                {
                    (void)MDS_IntUnlock();

                    /* 任务pend失败 */
                    return RTN_FAIL;
                }

                (void)MDS_IntUnlock();

                /* 使用软中断调度任务 */
                MDS_TaskSwiSched();

                /* 任务pend返回值, 该值在任务pend状态结束时被保存在uiDelayTick中 */
                return gpstrCurTcb->strTaskOpt.uiDelayTick;
            }
        }
        else /* 互斥信号量 */
        {
            /* 信号量为满, 可获取到信号量 */
            if(SEMFULL == pstrSem->uiCounter)
            {
                /* 获取到信号量后将信号量计数值-1 */
                pstrSem->uiCounter--;

                /* 将互斥信号量与任务关联 */
                pstrSem->pstrSemTask = gpstrCurTcb;

                (void)MDS_IntUnlock();

                return RTN_SUCD;
            }
            else /* 信号量不为满, 说明被一个任务获取过 */
            {
                /* 若该信号量已经被本任务获取则可以继续获取 */
                if(pstrSem->pstrSemTask == gpstrCurTcb)
                {
                    /* 信号量计数未空 */
                    if(SEMEMPTY != pstrSem->uiCounter)
                    {
                        /* 信号量计数值-1 */
                        pstrSem->uiCounter--;

                        (void)MDS_IntUnlock();

                        return RTN_SUCD;
                    }
                    else /* 信号量计数已空, 返回失败 */
                    {
                        (void)MDS_IntUnlock();

                        return RTN_SMTKOV;
                    }
                }
                else /* 被其它任务获取, 需要切换任务 */
                {
#ifdef MDS_TASKPRIOINHER

                    /* 信号量开启了任务优先级继承功能 */
                    if(SEMPRIINH == (pstrSem->uiSemOpt & SEMPRIINH))
                    {
                        MDS_TaskPrioInheritance(pstrSem->pstrSemTask,
                                                gpstrCurTcb->ucTaskPrio);
                    }

#endif

                    /* 将当前任务置为pend状态 */
                    if(RTN_FAIL == MDS_TaskPend(pstrSem, uiDelayTick))
                    {
                        (void)MDS_IntUnlock();

                        /* 任务pend失败 */
                        return RTN_FAIL;
                    }

                    (void)MDS_IntUnlock();

                    /* 使用软中断调度任务 */
                    MDS_TaskSwiSched();

                    /* 任务pend的返回值, 该值在任务pend状态结束时被保存在uiDelayTick
                       中 */
                    return gpstrCurTcb->strTaskOpt.uiDelayTick;
                }
            }
        }
    }
}

/***********************************************************************************
函数功能: 释放一个信号量. 信号量为非满时, 若有任务被该信号量阻塞则产生一次任务调度.
入口参数: pstrSem: 需要释放的信号量指针.
返 回 值: RTN_SUCD: 释放信号量成功.
          RTN_FAIL: 释放信号量失败.
          RTN_SMGVOV: 释放信号量溢出.
***********************************************************************************/
U32 MDS_SemGive(M_SEM* pstrSem)
{
#define NOTCHECKPENDTASK                0   /* 不检查挂起任务 */
#define CHECKPENDTASK                   1   /* 检查挂起任务 */
    M_TCB* pstrTcb;
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U32 uiRtn;
    U8 ucTaskPrio;
    U8 ucPendTaskFlag;

    /* 入口参数检查 */
    if(NULL == pstrSem)
    {
        return RTN_FAIL;
    }

    /* 在中断中使用信号量 */
    if(RTN_SUCD == MDS_RunInInt())
    {
        /* 中断中不能使用互斥信号量 */
        if(SEMMUT == (SEMTYPEMASK & pstrSem->uiSemOpt))
        {
            return RTN_FAIL;
        }
    }

    uiRtn = RTN_SUCD;

    (void)MDS_IntLock();

    /* 二进制信号量 */
    if(SEMBIN == (SEMTYPEMASK & pstrSem->uiSemOpt))
    {
        /* 信号量为空, 需要检查被该信号量阻塞的任务 */
        if(SEMEMPTY == pstrSem->uiCounter)
        {
            ucPendTaskFlag = CHECKPENDTASK;
        }
        else /* 信号量非空, 不需要检查被该信号量阻塞的任务 */
        {
            ucPendTaskFlag = NOTCHECKPENDTASK;
        }
    }
    /* 计数信号量 */
    else if(SEMCNT == (SEMTYPEMASK & pstrSem->uiSemOpt))
    {
        /* 信号量为空, 需要检查被该信号量阻塞的任务 */
        if(SEMEMPTY == pstrSem->uiCounter)
        {
            ucPendTaskFlag = CHECKPENDTASK;
        }
        else /* 信号量非空, 不需要检查被该信号量阻塞的任务 */
        {
            ucPendTaskFlag = NOTCHECKPENDTASK;

            /* 信号量未满 */
            if(SEMFULL != pstrSem->uiCounter)
            {
                /* 释放信号量后将信号量+1 */
                pstrSem->uiCounter++;
            }
            else /* 信号量已满 */
            {
                uiRtn = RTN_SMGVOV;
            }
        }
    }
    else /* 互斥信号量 */
    {
        /* 若释放互斥信号量的任务不是获取到互斥信号量的任务, 则返回失败 */
        if(pstrSem->pstrSemTask != gpstrCurTcb)
        {
            (void)MDS_IntUnlock();

            return RTN_FAIL;
        }

        /* 释放一次互斥信号量, 信号量计数值+1 */
        pstrSem->uiCounter++;

        /* 同一任务获取和释放互斥信号量不平衡, 不需要检查被该信号量阻塞的任务 */
        if(SEMFULL != pstrSem->uiCounter)
        {
            ucPendTaskFlag = NOTCHECKPENDTASK;
        }
        else /* 同一任务获取和释放互斥信号量平衡, 需要检查被该信号量阻塞的任务 */
        {
            ucPendTaskFlag = CHECKPENDTASK;
        }
    }

    /* 需要检查被该信号量阻塞的任务 */
    if(CHECKPENDTASK == ucPendTaskFlag)
    {
#ifdef MDS_TASKPRIOINHER

        /* 恢复当前任务的优先级 */
        MDS_TaskPrioResume(gpstrCurTcb);

#endif

        /* 在被该信号量阻塞的任务中获取需要释放的任务 */
        pstrTcb = MDS_SemGetActiveTask(pstrSem);

        /* 有阻塞的任务, 释放任务 */
        if(NULL != pstrTcb)
        {
            /* 从信号量调度表拆除该任务 */
            (void)MDS_TaskDelFromSemTab(pstrTcb);

            /* 任务在delay表则从delay表拆除 */
            if(DELAYQUEFLAG == (pstrTcb->uiTaskFlag & DELAYQUEFLAG))
            {
                pstrNode = &pstrTcb->strTcbQue.strQueHead;
                (void)MDS_DlistCurNodeDelete(&gstrDelayTab, pstrNode);

                /* 置任务不在delay表标志 */
                pstrTcb->uiTaskFlag &= (~((U32)DELAYQUEFLAG));
            }

            /* 清除任务的pend状态 */
            pstrTcb->strTaskOpt.ucTaskSta &= ~((U8)TASKPEND);

            /* 借用uiDelayTick变量保存pend任务的返回值 */
            pstrTcb->strTaskOpt.uiDelayTick = RTN_SUCD;

            /* 将该任务添加到ready表中 */
            pstrNode = &pstrTcb->strTcbQue.strQueHead;
            ucTaskPrio = pstrTcb->ucTaskPrio;
            pstrList = &gstrReadyTab.astrList[ucTaskPrio];
            pstrPrioFlag = &gstrReadyTab.strFlag;

            MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);

            /* 增加任务的ready状态 */
            pstrTcb->strTaskOpt.ucTaskSta |= TASKREADY;

            /* 保存获取到互斥信号量的任务 */
            if(SEMMUT == (SEMTYPEMASK & pstrSem->uiSemOpt))
            {
                pstrSem->pstrSemTask = pstrTcb;     /* 新获取到互斥信号量的任务 */
                pstrSem->uiCounter--;               /* 信号量被获取一次 */
            }

            (void)MDS_IntUnlock();

            /* 使用软中断调度任务 */
            MDS_TaskSwiSched();

            return uiRtn;
        }
        else /* 没有阻塞的任务 */
        {
            /* 二进制信号量 */
            if(SEMBIN == (SEMTYPEMASK & pstrSem->uiSemOpt))
            {
                /* 释放信号量后将信号量置为满 */
                pstrSem->uiCounter = SEMFULL;
            }
            /* 计数信号量 */
            else if(SEMCNT == (SEMTYPEMASK & pstrSem->uiSemOpt))
            {
                /* 释放信号量后将信号量+1. 走此分支信号量+1不会溢出 */
                pstrSem->uiCounter++;
            }
            else /* 互斥信号量 */
            {
                /* 没有任务获取到互斥信号量 */
                pstrSem->pstrSemTask = (M_TCB*)NULL;
            }
        }
    }

    (void)MDS_IntUnlock();

    return uiRtn;
}

/***********************************************************************************
函数功能: 释放被该信号量所阻塞的所有任务, 去掉这些任务的pend状态, 并指定被阻塞任务的
          返回值.
入口参数: pstrSem: 要释放的信号量指针.
          uiRtnValue: 指定被阻塞任务的返回值
返 回 值: RTN_SUCD: 释放信号量成功.
          RTN_FAIL: 释放信号量失败.
***********************************************************************************/
U32 MDS_SemFlushValue(M_SEM* pstrSem, U32 uiRtnValue)
{
    M_TCB* pstrTcb;
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8 ucTaskPrio;

    /* 入口参数检查 */
    if(NULL == pstrSem)
    {
        return RTN_FAIL;
    }

    /* 互斥信号量不能被批量释放 */
    if(SEMMUT == (SEMTYPEMASK & pstrSem->uiSemOpt))
    {
        return RTN_FAIL;
    }

    (void)MDS_IntLock();

    /* 在被该信号量阻塞的任务中获取需要释放的任务 */
    while(1)
    {
        pstrTcb = MDS_SemGetActiveTask(pstrSem);

        /* 有阻塞的任务, 释放任务 */
        if(NULL != pstrTcb)
        {
            /* 从信号量调度表拆除该任务 */
            (void)MDS_TaskDelFromSemTab(pstrTcb);

            /* 任务在delay表则从delay表拆除 */
            if(DELAYQUEFLAG == (pstrTcb->uiTaskFlag & DELAYQUEFLAG))
            {
                pstrNode = &pstrTcb->strTcbQue.strQueHead;
                (void)MDS_DlistCurNodeDelete(&gstrDelayTab, pstrNode);

                /* 置任务不在delay表标志 */
                pstrTcb->uiTaskFlag &= (~((U32)DELAYQUEFLAG));
            }

            /* 清除任务的pend状态 */
            pstrTcb->strTaskOpt.ucTaskSta &= ~((U8)TASKPEND);

            /* 借用uiDelayTick变量保存pend任务的返回值 */
            pstrTcb->strTaskOpt.uiDelayTick = uiRtnValue;

            /* 将该任务添加到ready表中 */
            pstrNode = &pstrTcb->strTcbQue.strQueHead;
            ucTaskPrio = pstrTcb->ucTaskPrio;
            pstrList = &gstrReadyTab.astrList[ucTaskPrio];
            pstrPrioFlag = &gstrReadyTab.strFlag;

            MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);

            /* 增加任务的ready状态 */
            pstrTcb->strTaskOpt.ucTaskSta |= TASKREADY;
        }
        else /* 没有阻塞的任务, 跳出循环操作 */
        {
            break;
        }
    }

    /* 将信号量置为空 */
    pstrSem->uiCounter = SEMEMPTY;

    (void)MDS_IntUnlock();

    /* 使用软中断调度任务 */
    MDS_TaskSwiSched();

    return RTN_SUCD;
}

/***********************************************************************************
函数功能: 释放被该信号量所阻塞的所有任务, 去掉这些任务的pend状态.
入口参数: pstrSem: 需要释放的信号量指针.
返 回 值: RTN_SUCD: 释放信号量成功.
          RTN_FAIL: 释放信号量失败.
***********************************************************************************/
U32 MDS_SemFlush(M_SEM* pstrSem)
{
    /* 释放信号量所阻塞的所有任务 */
    return MDS_SemFlushValue(pstrSem, RTN_SUCD);
}

/***********************************************************************************
函数功能: 删除一个信号量.
入口参数: pstrSem: 需要删除的信号量指针.
返 回 值: RTN_SUCD: 删除信号量成功.
          RTN_FAIL: 删除信号量失败.
***********************************************************************************/
U32 MDS_SemDelete(M_SEM* pstrSem)
{
    /* 入口参数检查 */
    if(NULL == pstrSem)
    {
        return RTN_FAIL;
    }

    /* 非互斥信号量则释放被阻塞的所有任务 */
    if(SEMMUT != (SEMTYPEMASK & pstrSem->uiSemOpt))
    {
        /* 释放信号量所阻塞的所有任务 */
        if(RTN_SUCD != MDS_SemFlushValue(pstrSem, RTN_SMTKDL))
        {
            return RTN_FAIL;
        }
    }

    /* 如果是创建信号量函数申请的信号量内存, 则需要释放 */
    if(NULL != pstrSem->pucSemMem)
    {
        (void)MDS_IntLock();

        free(pstrSem->pucSemMem);

        (void)MDS_IntUnlock();
    }

    return RTN_SUCD;
}

