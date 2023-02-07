
#include <stdlib.h>
#include "mindows_inner.h"


/***********************************************************************************
��������: ����һ������.
��ڲ���: pstrQue: ��Ҫ�����Ķ��е�ָ��, ��Ҫ��M_QUE����, ��ΪNULL���ɸú�����������
                   �ڴ洴��.
�� �� ֵ: NULL: ��������ʧ��.
          ����: �������гɹ�, ����ֵΪ�����Ķ��е�ָ��.
***********************************************************************************/
M_QUE* MDS_QueCreate(M_QUE* pstrQue)
{
    U8* pucQueMemAddr;

    /* ����ָ��Ϊ��, ��Ҫ�Լ������ڴ� */
    if(NULL == pstrQue)
    {
        (void)MDS_IntLock();

        pucQueMemAddr = malloc(sizeof(M_QUE));
        if(NULL == pucQueMemAddr)
        {
            (void)MDS_IntUnlock();

            /* ���벻���ڴ�, ����ʧ�� */
            return (M_QUE*)NULL;
        }

        (void)MDS_IntUnlock();

        /* ����ָ��������ڴ� */
        pstrQue = (M_QUE*)pucQueMemAddr;
    }
    else /* ����ڲ����������ʹ�õ��ڴ�, �����Լ����� */
    {
        /* �������ڴ��ַ��Ϊ�� */
        pucQueMemAddr = (U8*)NULL;
    }

    /* ��ʼ�����и��ڵ� */
    MDS_DlistInit(&pstrQue->strList);

    /* ��������ڴ����ʼ��ַ */
    pstrQue->pucQueMem = pucQueMemAddr;

    return pstrQue;
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

/***********************************************************************************
��������: ɾ��һ������.
��ڲ���: pstrQue: Ҫɾ���Ķ���ָ��.
�� �� ֵ: RTN_SUCD: ɾ�����гɹ�.
          RTN_FAIL: ɾ������ʧ��.
***********************************************************************************/
U32 MDS_QueDelete(M_QUE* pstrQue)
{
    /* ��ڲ������ */
    if(NULL == pstrQue)
    {
        return RTN_FAIL;
    }

    /* ����Ǵ������к�������Ķ����ڴ�, ����Ҫ�ͷ� */
    if(NULL != pstrQue->pucQueMem)
    {
        (void)MDS_IntLock();

        free(pstrQue->pucQueMem);

        (void)MDS_IntUnlock();
    }

    return RTN_SUCD;
}

