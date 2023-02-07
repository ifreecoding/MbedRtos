
#ifndef MDS_QUEUE_H
#define MDS_QUEUE_H


#define RTN_NULL            2       /* 队列为空 */


typedef struct m_que    /* 队列结构 */
{
    M_DLIST strList;    /* 队列链表 */
    U8* pucQueMem;      /* 创建队列时的内存地址 */
}M_QUE;


/*********************************** 函数声明 *************************************/
extern M_QUE* MDS_QueCreate(M_QUE* pstrQue);
extern U32 MDS_QuePut(M_QUE* pstrQue, M_DLIST* pstrQueNode);
extern U32 MDS_QueGet(M_QUE* pstrQue, M_DLIST** ppstrQueNode);
extern U32 MDS_QueDelete(M_QUE* pstrQue);


#endif

