
#include "mindows_inner.h"


/***********************************************************************************
��������: ��ʼ������.
��ڲ���: pstrList: ������ڵ�ָ��.
�� �� ֵ: none.
***********************************************************************************/
void MDS_DlistInit(M_DLIST* pstrList)
{
    /* �������ͷβ��ָ��սڵ� */
    pstrList->pstrHead = (M_DLIST*)NULL;
    pstrList->pstrTail = (M_DLIST*)NULL;
}

/***********************************************************************************
��������: ���������һ���ڵ�, ������β������.
��ڲ���: pstrList: ������ڵ�ָ��.
          pstrNode: ����Ľڵ�ָ��.
�� �� ֵ: none.
***********************************************************************************/
void MDS_DlistNodeAdd(M_DLIST* pstrList, M_DLIST* pstrNode)
{
    /* ����ǿ� */
    if(NULL != pstrList->pstrTail)
    {
        /* �½ڵ��ͷָ��ԭβ�ڵ� */
        pstrNode->pstrHead = pstrList->pstrHead;

        /* �½ڵ��βָ����ڵ� */
        pstrNode->pstrTail = pstrList;

        /* ԭβ�ڵ��βָ���½ڵ� */
        pstrList->pstrHead->pstrTail = pstrNode;

        /* ���ڵ��ͷָ���¼���Ľڵ� */
        pstrList->pstrHead = pstrNode;
    }
    else /* ����Ϊ�� */
    {
        /* �½ڵ��ͷβ��ָ����ڵ� */
        pstrNode->pstrHead = pstrList;
        pstrNode->pstrTail = pstrList;

        /* ���ڵ��ͷβ��ָ���½ڵ� */
        pstrList->pstrHead = pstrNode;
        pstrList->pstrTail = pstrNode;
    }
}

/***********************************************************************************
��������: ������ɾ��һ���ڵ�, ������ͷ��ɾ��.
��ڲ���: pstrList: ������ڵ�ָ��.
�� �� ֵ: ɾ���Ľڵ�ָ��, ������Ϊ���򷵻�NULL.
***********************************************************************************/
M_DLIST* MDS_DlistNodeDelete(M_DLIST* pstrList)
{
    M_DLIST* pstrTempNode;

    /* �����еĵ�һ���ڵ� */
    pstrTempNode = pstrList->pstrTail;

    /* ����ǿ� */
    if(NULL != pstrTempNode)
    {
        /* �������ж���ڵ� */
        if(pstrList->pstrHead != pstrList->pstrTail)
        {
            /* ���ڵ��βָ��ڶ����ڵ� */
            pstrList->pstrTail = pstrTempNode->pstrTail;

            /* �ڶ����ڵ��ͷָ����ڵ� */
            pstrTempNode->pstrTail->pstrHead = pstrList;
        }
        else /* ������ֻ��һ���ڵ� */
        {
            /* ȡ���ڵ������Ϊ�� */
            pstrList->pstrHead = (M_DLIST*)NULL;
            pstrList->pstrTail = (M_DLIST*)NULL;
        }

        /* ����ȡ���Ľڵ�ָ�� */
        return pstrTempNode;
    }
    else /* ����Ϊ�շ���NULL */
    {
        return (M_DLIST*)NULL;
    }
}

/***********************************************************************************
��������: ������ָ���Ľڵ�ǰ����һ���ڵ�.
��ڲ���: pstrList: ������ڵ�ָ��.
          pstrNode: ��׼�ڵ�ָ��, ���½ڵ�嵽�ýڵ�ǰ��.
          pstrNewNode: �²���ڵ��ָ��.
�� �� ֵ: none.
***********************************************************************************/
void MDS_DlistCurNodeInsert(M_DLIST* pstrList, M_DLIST* pstrNode,
                            M_DLIST* pstrNewNode)
{
    /* ��׼�ڵ㲻�Ǹ��ڵ� */
    if(pstrList != pstrNode)
    {
        /* ��׼�ڵ���ϸ��ڵ��βָ���½ڵ� */
        pstrNode->pstrHead->pstrTail = pstrNewNode;

        /* �½ڵ��ͷָ���׼�ڵ���ϸ��ڵ� */
        pstrNewNode->pstrHead = pstrNode->pstrHead;

        /* �½ڵ��βָ���׼�ڵ� */
        pstrNewNode->pstrTail = pstrNode;

        /* ��׼�ڵ��ͷָ���½ڵ� */
        pstrNode->pstrHead = pstrNewNode;
    }
    else /* ��׼�ڵ��Ǹ��ڵ� */
    {
        MDS_DlistNodeAdd(pstrList, pstrNewNode);
    }
}

/***********************************************************************************
��������: ������ɾ��ָ���Ľڵ�, �������¸��ڵ��ָ��.
��ڲ���: pstrList: ������ڵ�ָ��.
          pstrNode: Ҫɾ���Ľڵ��ָ��.
�� �� ֵ: ɾ���ڵ���¸��ڵ�ָ��, ��û���¸��ڵ��򷵻�NULL.
***********************************************************************************/
M_DLIST* MDS_DlistCurNodeDelete(M_DLIST* pstrList, M_DLIST* pstrNode)
{
    /* Ҫɾ���Ľڵ㲻�Ǹ��ڵ� */
    if(pstrList != pstrNode)
    {
        /* �������ж���ڵ� */
        if((pstrNode->pstrHead != pstrList) || (pstrNode->pstrTail != pstrList))
        {
            /* Ҫɾ���ڵ���ϸ��ڵ��βָ��Ҫɾ���ڵ���¸��ڵ� */
            pstrNode->pstrHead->pstrTail = pstrNode->pstrTail;

            /* Ҫɾ���ڵ���¸��ڵ��ͷָ��Ҫɾ���ڵ���ϸ��ڵ� */
            pstrNode->pstrTail->pstrHead = pstrNode->pstrHead;

            /* ����ɾ���ڵ���¸��ڵ�ָ�� */
            return pstrNode->pstrTail;
        }
        else /* ������ֻ��һ���ڵ� */
        {
            (void)MDS_DlistNodeDelete(pstrList);

            /* û���¸��ڵ�, ����NULL */
            return (M_DLIST*)NULL;
        }
    }
    else /* ɾ�����ڵ�ֱ�ӷ���NULL */
    {
        return (M_DLIST*)NULL;
    }
}

/***********************************************************************************
��������: ��ѯ�����Ƿ�Ϊ��.
��ڲ���: pstrList: ������ڵ�ָ��.
�� �� ֵ: ���ǿ��򷵻ص�һ���ڵ��ָ��, �����򷵻�NULL.
***********************************************************************************/
M_DLIST* MDS_DlistEmpInq(M_DLIST* pstrList)
{
    return pstrList->pstrTail;
}

/***********************************************************************************
��������: ��ѯ������ָ���ڵ����һ���ڵ��Ƿ�Ϊ��.
��ڲ���: pstrList: ������ڵ�ָ��.
          pstrNode: ��׼�ڵ�ָ��, ��ѯ�ýڵ����һ���ڵ�.
�� �� ֵ: ��ָ���ڵ����һ���ڵ�ǿ��򷵻���һ���ڵ��ָ��, �����򷵻�NULL.
***********************************************************************************/
M_DLIST* MDS_DlistNextNodeEmpInq(M_DLIST* pstrList, M_DLIST* pstrNode)
{
    /* ��׼�ڵ������һ���ڵ��򷵻�NULL */
    if(pstrNode->pstrTail == pstrList)
    {
        return (M_DLIST*)NULL;
    }
    else
    {
        return pstrNode->pstrTail;
    }
}

