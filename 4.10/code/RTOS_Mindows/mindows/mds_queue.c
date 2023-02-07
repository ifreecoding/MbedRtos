
#include <stdlib.h>
#include "mindows_inner.h"


/***********************************************************************************
函数功能: 创建一个队列.
入口参数: pstrQue: 需要创建的队列的指针, 需要是M_QUE类型, 若为NULL则由该函数自行申请
                   内存创建.
返 回 值: NULL: 创建队列失败.
          其它: 创建队列成功, 返回值为创建的队列的指针.
***********************************************************************************/
M_QUE* MDS_QueCreate(M_QUE* pstrQue)
{
    U8* pucQueMemAddr;

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

    return pstrQue;
}

/***********************************************************************************
函数功能: 将一个节点加入队列.
入口参数: pstrQue: 队列指针.
          pstrQueNode: 要加入的队列节点指针.
返 回 值: RTN_SUCD: 将节点加入队列成功.
          RTN_FAIL: 将节点加入队列失败.
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

    return RTN_SUCD;
}

/***********************************************************************************
函数功能: 从队列中取出一个队列节点.
入口参数: pstrQue: 队列指针.
          ppstrQueNode: 存放队列节点指针的指针.
返 回 值: RTN_SUCD: 从队列取出节点成功.
          RTN_FAIL: 从队列取出节点失败.
          RTN_NULL: 队列为空.
***********************************************************************************/
U32 MDS_QueGet(M_QUE* pstrQue, M_DLIST** ppstrQueNode)
{
    M_DLIST* pstrQueNode;

    /* 入口参数检查 */
    if((NULL == pstrQue) || (NULL == ppstrQueNode))
    {
        return RTN_FAIL;
    }

    (void)MDS_IntLock();

    /* 从队列取出节点 */
    pstrQueNode = MDS_DlistNodeDelete(&pstrQue->strList);

    (void)MDS_IntUnlock();

    /* 队列不为空, 可以取出节点 */
    if(NULL != pstrQueNode)
    {
        *ppstrQueNode = pstrQueNode;

        return RTN_SUCD;
    }
    else /* 队列为空, 无法取出节点 */
    {
        return RTN_NULL;
    }
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

    /* 如果是创建队列函数申请的队列内存, 则需要释放 */
    if(NULL != pstrQue->pucQueMem)
    {
        (void)MDS_IntLock();

        free(pstrQue->pucQueMem);

        (void)MDS_IntUnlock();
    }

    return RTN_SUCD;
}

