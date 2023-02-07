
#include "mindows_inner.h"


/***********************************************************************************
函数功能: 初始化链表.
入口参数: pstrList: 链表根节点指针.
返 回 值: none.
***********************************************************************************/
void MDS_DlistInit(M_DLIST* pstrList)
{
    /* 空链表的头尾都指向空节点 */
    pstrList->pstrHead = (M_DLIST*)NULL;
    pstrList->pstrTail = (M_DLIST*)NULL;
}

/***********************************************************************************
函数功能: 向链表添加一个节点, 从链表尾部加入.
入口参数: pstrList: 链表根节点指针.
          pstrNode: 加入的节点指针.
返 回 值: none.
***********************************************************************************/
void MDS_DlistNodeAdd(M_DLIST* pstrList, M_DLIST* pstrNode)
{
    /* 链表非空 */
    if(NULL != pstrList->pstrTail)
    {
        /* 新节点的头指向原尾节点 */
        pstrNode->pstrHead = pstrList->pstrHead;

        /* 新节点的尾指向根节点 */
        pstrNode->pstrTail = pstrList;

        /* 原尾节点的尾指向新节点 */
        pstrList->pstrHead->pstrTail = pstrNode;

        /* 根节点的头指向新加入的节点 */
        pstrList->pstrHead = pstrNode;
    }
    else /* 链表为空 */
    {
        /* 新节点的头尾都指向根节点 */
        pstrNode->pstrHead = pstrList;
        pstrNode->pstrTail = pstrList;

        /* 根节点的头尾都指向新节点 */
        pstrList->pstrHead = pstrNode;
        pstrList->pstrTail = pstrNode;
    }
}

/***********************************************************************************
函数功能: 从链表删除一个节点, 从链表头部删除.
入口参数: pstrList: 链表根节点指针.
返 回 值: 删除的节点指针, 若链表为空则返回NULL.
***********************************************************************************/
M_DLIST* MDS_DlistNodeDelete(M_DLIST* pstrList)
{
    M_DLIST* pstrTempNode;

    /* 链表中的第一个节点 */
    pstrTempNode = pstrList->pstrTail;

    /* 链表非空 */
    if(NULL != pstrTempNode)
    {
        /* 链表中有多个节点 */
        if(pstrList->pstrHead != pstrList->pstrTail)
        {
            /* 根节点的尾指向第二个节点 */
            pstrList->pstrTail = pstrTempNode->pstrTail;

            /* 第二个节点的头指向根节点 */
            pstrTempNode->pstrTail->pstrHead = pstrList;
        }
        else /* 链表中只有一个节点 */
        {
            /* 取出节点后链表为空 */
            pstrList->pstrHead = (M_DLIST*)NULL;
            pstrList->pstrTail = (M_DLIST*)NULL;
        }

        /* 返回取出的节点指针 */
        return pstrTempNode;
    }
    else /* 链表为空返回NULL */
    {
        return (M_DLIST*)NULL;
    }
}

/***********************************************************************************
函数功能: 向链表指定的节点前插入一个节点.
入口参数: pstrList: 链表根节点指针.
          pstrNode: 基准节点指针, 将新节点插到该节点前面.
          pstrNewNode: 新插入节点的指针.
返 回 值: none.
***********************************************************************************/
void MDS_DlistCurNodeInsert(M_DLIST* pstrList, M_DLIST* pstrNode,
                            M_DLIST* pstrNewNode)
{
    /* 基准节点不是根节点 */
    if(pstrList != pstrNode)
    {
        /* 基准节点的上个节点的尾指向新节点 */
        pstrNode->pstrHead->pstrTail = pstrNewNode;

        /* 新节点的头指向基准节点的上个节点 */
        pstrNewNode->pstrHead = pstrNode->pstrHead;

        /* 新节点的尾指向基准节点 */
        pstrNewNode->pstrTail = pstrNode;

        /* 基准节点的头指向新节点 */
        pstrNode->pstrHead = pstrNewNode;
    }
    else /* 基准节点是根节点 */
    {
        MDS_DlistNodeAdd(pstrList, pstrNewNode);
    }
}

/***********************************************************************************
函数功能: 从链表删除指定的节点, 并返回下个节点的指针.
入口参数: pstrList: 链表根节点指针.
          pstrNode: 要删除的节点的指针.
返 回 值: 删除节点的下个节点指针, 若没有下个节点则返回NULL.
***********************************************************************************/
M_DLIST* MDS_DlistCurNodeDelete(M_DLIST* pstrList, M_DLIST* pstrNode)
{
    /* 要删除的节点不是根节点 */
    if(pstrList != pstrNode)
    {
        /* 链表中有多个节点 */
        if((pstrNode->pstrHead != pstrList) || (pstrNode->pstrTail != pstrList))
        {
            /* 要删除节点的上个节点的尾指向要删除节点的下个节点 */
            pstrNode->pstrHead->pstrTail = pstrNode->pstrTail;

            /* 要删除节点的下个节点的头指向要删除节点的上个节点 */
            pstrNode->pstrTail->pstrHead = pstrNode->pstrHead;

            /* 返回删除节点的下个节点指针 */
            return pstrNode->pstrTail;
        }
        else /* 链表中只有一个节点 */
        {
            (void)MDS_DlistNodeDelete(pstrList);

            /* 没有下个节点, 返回NULL */
            return (M_DLIST*)NULL;
        }
    }
    else /* 删除根节点直接返回NULL */
    {
        return (M_DLIST*)NULL;
    }
}

/***********************************************************************************
函数功能: 查询链表是否为空.
入口参数: pstrList: 链表根节点指针.
返 回 值: 若非空则返回第一个节点的指针, 若空则返回NULL.
***********************************************************************************/
M_DLIST* MDS_DlistEmpInq(M_DLIST* pstrList)
{
    return pstrList->pstrTail;
}

/***********************************************************************************
函数功能: 查询链表中指定节点的下一个节点是否为空.
入口参数: pstrList: 链表根节点指针.
          pstrNode: 基准节点指针, 查询该节点的下一个节点.
返 回 值: 若指定节点的下一个节点非空则返回下一个节点的指针, 若空则返回NULL.
***********************************************************************************/
M_DLIST* MDS_DlistNextNodeEmpInq(M_DLIST* pstrList, M_DLIST* pstrNode)
{
    /* 基准节点是最后一个节点则返回NULL */
    if(pstrNode->pstrTail == pstrList)
    {
        return (M_DLIST*)NULL;
    }
    else
    {
        return pstrNode->pstrTail;
    }
}

