
#include <stdlib.h>
#include "mindows_inner.h"


/***********************************************************************************
函数功能: 创建一个队列.
入口参数: pstrQue: 需要创建的队列的指针, 需要是M_QUE类型, 若为NULL则由该函数自行申请
                   内存创建.
          uiQueOpt: 创建队列所用的选项.
                    优先级:
                    QUEPRIO: 采用优先级队列, 发送消息时先激活队列中被阻塞的优先级最
                             高的任务.
                    QUEFIFO: 采用先进先出队列, 发送消息时先激活最先被队列阻塞的任务.
返 回 值: NULL: 创建队列失败.
          其它: 创建队列成功, 返回值为创建的队列的指针.
***********************************************************************************/
M_QUE* MDS_QueCreate(M_QUE* pstrQue, U32 uiQueOpt)
{
    U8* pucQueMemAddr;

    /* 队列选项检查 */
    if((QUEFIFO != (QUESCHEDOPTMASK & uiQueOpt))
       && (QUEPRIO != (QUESCHEDOPTMASK & uiQueOpt)))
    {
        return (M_QUE*)NULL;
    }

    /* 传入指针为空, 需要自己申请内存 */
    if(NULL == pstrQue)
    {
        (void)MDS_IntLock();

        pucQueMemAddr = malloc(sizeof(M_QUE));
        if(NULL == pucQueMemAddr)
        {
            (void)MDS_IntUnlock();

            /* 申请不到内存, 返回失败 */
            return (M_QUE*)NULL;
        }

        (void)MDS_IntUnlock();

        /* 队列指向申请的内存 */
        pstrQue = (M_QUE*)pucQueMemAddr;
    }
    else /* 由入口参数传入队列使用的内存, 无需自己申请 */
    {
        /* 将队列内存地址置为空 */
        pucQueMemAddr = (U8*)NULL;
    }

    /* 初始化队列根节点 */
    MDS_DlistInit(&pstrQue->strList);

    /* 保存队列内存的起始地址 */
    pstrQue->pucQueMem = pucQueMemAddr;

    /* 创建队列使用的计数信号量 */
    if(NULL != MDS_SemCreate(&pstrQue->strSem, SEMCNT | uiQueOpt, SEMEMPTY))
    {
        return pstrQue;
    }
    else
    {
        /* 创建信号量失败, 释放申请的内存 */
        if(NULL != pucQueMemAddr)
        {
            free(pucQueMemAddr);
        }

        return (M_QUE*)NULL;
    }
}

/***********************************************************************************
函数功能: 将一个节点加入队列. 若有任务被该队列阻塞则会产生一次任务调度.
入口参数: pstrQue: 队列指针.
          pstrQueNode: 要加入的队列节点指针.
返 回 值: RTN_SUCD: 将节点加入队列成功.
          RTN_FAIL: 将节点加入队列失败.
          RTN_QUPTOV: 将节点加入队列导致队列信号量溢出.
***********************************************************************************/
U32 MDS_QuePut(M_QUE* pstrQue, M_DLIST* pstrQueNode)
{
    /* 入口参数检查 */
    if((NULL == pstrQue) || (NULL == pstrQueNode))
    {
        return RTN_FAIL;
    }

    (void)MDS_IntLock();

    /* 将节点加入队列 */
    MDS_DlistNodeAdd(&pstrQue->strList, pstrQueNode);

    (void)MDS_IntUnlock();

    return MDS_SemGive(&pstrQue->strSem);
}

/***********************************************************************************
函数功能: 从队列中取出一个队列节点. 当队列不为空时可获取, 当队列为空时无法获取, 可能
          会使任务变为pend状态, 发生任务切换.
入口参数: pstrQue: 队列指针.
          ppstrQueNode: 存放队列节点指针的指针.
          uiDelayTick: 获取队列等待的时间, 单位: tick.
                       QUENOWAIT: 无论从队列中能否取出节点都不等待时间, 返回该函数.
                       QUEWAITFEV: 永久等待, 若从队列无法取出节点则该任务切换为pend
                                   状态一直等待, 期间若能获取到节点则转换为ready态.
                       其它: 若从队列中无法取出节点则该任务切换为pend状态, 若在该时
                             间内可获取节点则结束该任务的pend状态, 返回RTN_SUCD, 若
                             在该时间内获取不到节点则结束该任务的pend状态, 返回
                             RTN_SMTKTO.
返 回 值: RTN_SUCD: 从队列取出节点成功.
          RTN_FAIL: 从队列取出节点失败.
          RTN_QUGTTO: 等待队列消息的时间耗尽, 超时返回.
          RTN_QUGTRT: 使用不等待时间参数没有获取到队列消息, 直接返回.
          RTN_QUGTDL: 队列被删除.
***********************************************************************************/
U32 MDS_QueGet(M_QUE* pstrQue, M_DLIST** ppstrQueNode, U32 uiDelayTick)
{
    M_DLIST* pstrQueNode;
    U32 uiRtn;

    /* 入口参数检查 */
    if((NULL == pstrQue) || (NULL == ppstrQueNode))
    {
        return RTN_FAIL;
    }

    /* 没有获取到队列需要的信号量, 返回失败 */
    uiRtn = MDS_SemTake(&pstrQue->strSem, uiDelayTick);
    if(RTN_SUCD != uiRtn)
    {
        return uiRtn;
    }

    (void)MDS_IntLock();

    /* 从队列取出节点 */
    pstrQueNode = MDS_DlistNodeDelete(&pstrQue->strList);

    (void)MDS_IntUnlock();

    /* 取出节点 */
    *ppstrQueNode = pstrQueNode;

    return RTN_SUCD;
}

/***********************************************************************************
函数功能: 删除一个队列.
入口参数: pstrQue: 要删除的队列指针.
返 回 值: RTN_SUCD: 删除队列成功.
          RTN_FAIL: 删除队列失败.
***********************************************************************************/
U32 MDS_QueDelete(M_QUE* pstrQue)
{
    /* 入口参数检查 */
    if(NULL == pstrQue)
    {
        return RTN_FAIL;
    }

    /* 释放队列信号量所阻塞的所有任务, 返回信号量被删除 */
    if(RTN_SUCD != MDS_SemFlushValue(&pstrQue->strSem, RTN_SMTKDL))
    {
        return RTN_FAIL;
    }

    /* 如果是创建队列函数申请的队列内存, 则需要释放 */
    if(NULL != pstrQue->pucQueMem)
    {
        (void)MDS_IntLock();

        free(pstrQue->pucQueMem);

        (void)MDS_IntUnlock();
    }

    return RTN_SUCD;
}

