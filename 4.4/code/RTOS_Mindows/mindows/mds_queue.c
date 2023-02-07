
#include "mindows_inner.h"


/***********************************************************************************
��������: ����һ������.
��ڲ���: pstrQue: ��Ҫ�����Ķ��е�ָ��.
�� �� ֵ: RTN_SUCD: �������гɹ�.
          RTN_FAIL: ��������ʧ��.
***********************************************************************************/
U32 MDS_QueCreate(M_QUE* pstrQue)
{
    /* ��ڲ������ */
    if(NULL == pstrQue)
    {
        return RTN_FAIL;
    }

    /* ��ʼ�����и��ڵ� */
    MDS_DlistInit(&pstrQue->strList);

    return RTN_SUCD;
}

/***********************************************************************************
��������: ��һ���ڵ�������.
��ڲ���: pstrQue: ����ָ��.
          pstrQueNode: Ҫ����Ķ��нڵ�ָ��.
�� �� ֵ: RTN_SUCD: ���ڵ������гɹ�.
          RTN_FAIL: ���ڵ�������ʧ��.
***********************************************************************************/
U32 MDS_QuePut(M_QUE* pstrQue, M_DLIST* pstrQueNode)
{
    /* ��ڲ������ */
    if((NULL == pstrQue) || (NULL == pstrQueNode))
    {
        return RTN_FAIL;
    }

    (void)MDS_IntLock();

    /* ���ڵ������� */
    MDS_DlistNodeAdd(&pstrQue->strList, pstrQueNode);

    (void)MDS_IntUnlock();

    return RTN_SUCD;
}

/***********************************************************************************
��������: �Ӷ�����ȡ��һ�����нڵ�.
��ڲ���: pstrQue: ����ָ��.
          ppstrQueNode: ��Ŷ��нڵ�ָ���ָ��.
�� �� ֵ: RTN_SUCD: �Ӷ���ȡ���ڵ�ɹ�.
          RTN_FAIL: �Ӷ���ȡ���ڵ�ʧ��.
          RTN_NULL: ����Ϊ��.
***********************************************************************************/
U32 MDS_QueGet(M_QUE* pstrQue, M_DLIST** ppstrQueNode)
{
    M_DLIST* pstrQueNode;

    /* ��ڲ������ */
    if((NULL == pstrQue) || (NULL == ppstrQueNode))
    {
        return RTN_FAIL;
    }

    (void)MDS_IntLock();

    /* �Ӷ���ȡ���ڵ� */
    pstrQueNode = MDS_DlistNodeDelete(&pstrQue->strList);

    (void)MDS_IntUnlock();

    /* ���в�Ϊ��, ����ȡ���ڵ� */
    if(NULL != pstrQueNode)
    {
        *ppstrQueNode = pstrQueNode;

        return RTN_SUCD;
    }
    else /* ����Ϊ��, �޷�ȡ���ڵ� */
    {
        return RTN_NULL;
    }
}

