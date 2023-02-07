
#include <stdlib.h>
#include "mindows_inner.h"


/***********************************************************************************
��������: ����һ������.
��ڲ���: pstrQue: ��Ҫ�����Ķ��е�ָ��, ��Ҫ��M_QUE����, ��ΪNULL���ɸú�����������
                   �ڴ洴��.
          uiQueOpt: �����������õ�ѡ��.
                    ���ȼ�:
                    QUEPRIO: �������ȼ�����, ������Ϣʱ�ȼ�������б����������ȼ���
                             �ߵ�����.
                    QUEFIFO: �����Ƚ��ȳ�����, ������Ϣʱ�ȼ������ȱ���������������.
�� �� ֵ: NULL: ��������ʧ��.
          ����: �������гɹ�, ����ֵΪ�����Ķ��е�ָ��.
***********************************************************************************/
M_QUE* MDS_QueCreate(M_QUE* pstrQue, U32 uiQueOpt)
{
    U8* pucQueMemAddr;

    /* ����ѡ���� */
    if((QUEFIFO != (QUESCHEDOPTMASK & uiQueOpt))
       && (QUEPRIO != (QUESCHEDOPTMASK & uiQueOpt)))
    {
        return (M_QUE*)NULL;
    }

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

    /* ��������ʹ�õļ����ź��� */
    if(NULL != MDS_SemCreate(&pstrQue->strSem, SEMCNT | uiQueOpt, SEMEMPTY))
    {
        return pstrQue;
    }
    else
    {
        /* �����ź���ʧ��, �ͷ�������ڴ� */
        if(NULL != pucQueMemAddr)
        {
            free(pucQueMemAddr);
        }

        return (M_QUE*)NULL;
    }
}

/***********************************************************************************
��������: ��һ���ڵ�������. �������񱻸ö�������������һ���������.
��ڲ���: pstrQue: ����ָ��.
          pstrQueNode: Ҫ����Ķ��нڵ�ָ��.
�� �� ֵ: RTN_SUCD: ���ڵ������гɹ�.
          RTN_FAIL: ���ڵ�������ʧ��.
          RTN_QUPTOV: ���ڵ������е��¶����ź������.
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

    return MDS_SemGive(&pstrQue->strSem);
}

/***********************************************************************************
��������: �Ӷ�����ȡ��һ�����нڵ�. �����в�Ϊ��ʱ�ɻ�ȡ, ������Ϊ��ʱ�޷���ȡ, ����
          ��ʹ�����Ϊpend״̬, ���������л�.
��ڲ���: pstrQue: ����ָ��.
          ppstrQueNode: ��Ŷ��нڵ�ָ���ָ��.
          uiDelayTick: ��ȡ���еȴ���ʱ��, ��λ: tick.
                       QUENOWAIT: ���۴Ӷ������ܷ�ȡ���ڵ㶼���ȴ�ʱ��, ���ظú���.
                       QUEWAITFEV: ���õȴ�, ���Ӷ����޷�ȡ���ڵ���������л�Ϊpend
                                   ״̬һֱ�ȴ�, �ڼ����ܻ�ȡ���ڵ���ת��Ϊready̬.
                       ����: ���Ӷ������޷�ȡ���ڵ���������л�Ϊpend״̬, ���ڸ�ʱ
                             ���ڿɻ�ȡ�ڵ�������������pend״̬, ����RTN_SUCD, ��
                             �ڸ�ʱ���ڻ�ȡ�����ڵ�������������pend״̬, ����
                             RTN_SMTKTO.
�� �� ֵ: RTN_SUCD: �Ӷ���ȡ���ڵ�ɹ�.
          RTN_FAIL: �Ӷ���ȡ���ڵ�ʧ��.
          RTN_QUGTTO: �ȴ�������Ϣ��ʱ��ľ�, ��ʱ����.
          RTN_QUGTRT: ʹ�ò��ȴ�ʱ�����û�л�ȡ��������Ϣ, ֱ�ӷ���.
          RTN_QUGTDL: ���б�ɾ��.
***********************************************************************************/
U32 MDS_QueGet(M_QUE* pstrQue, M_DLIST** ppstrQueNode, U32 uiDelayTick)
{
    M_DLIST* pstrQueNode;
    U32 uiRtn;

    /* ��ڲ������ */
    if((NULL == pstrQue) || (NULL == ppstrQueNode))
    {
        return RTN_FAIL;
    }

    /* û�л�ȡ��������Ҫ���ź���, ����ʧ�� */
    uiRtn = MDS_SemTake(&pstrQue->strSem, uiDelayTick);
    if(RTN_SUCD != uiRtn)
    {
        return uiRtn;
    }

    (void)MDS_IntLock();

    /* �Ӷ���ȡ���ڵ� */
    pstrQueNode = MDS_DlistNodeDelete(&pstrQue->strList);

    (void)MDS_IntUnlock();

    /* ȡ���ڵ� */
    *ppstrQueNode = pstrQueNode;

    return RTN_SUCD;
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

    /* �ͷŶ����ź�������������������, �����ź�����ɾ�� */
    if(RTN_SUCD != MDS_SemFlushValue(&pstrQue->strSem, RTN_SMTKDL))
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

