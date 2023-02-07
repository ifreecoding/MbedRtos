
#include <stdlib.h>
#include "mindows_inner.h"


/***********************************************************************************
��������: ����һ���ź���.
��ڲ���: pstrSem: ��Ҫ�������ź�����ָ��, ��Ҫ��M_SEM����, ��ΪNULL���ɸú���������
                   ���ڴ洴��.
          uiSemOpt: �����ź������õ�ѡ��.
                    ���ȼ�:
                    SEMPRIO: �ź������в������ȼ�����, �ͷ��ź���ʱ�ȼ�����ź���
                             ���������ȼ���ߵ�����.
                    SEMFIFO: �ź������в����Ƚ��ȳ�����, �ͷ��ź���ʱ�ȼ������ȱ���
                             �ź�������������.
                    �ź�������:
                    SEMBIN: �����������ź���.
                    SEMCNT: ���������ź���.
                    SEMMUT: ���������ź���.
                    �������ȼ��̳�:
                    SEMPRIINH: ��������ʹ���������ȼ��̳й���. �ò���ֻ������
                               SEMMUT�ź���SEMPRIOģʽ��.
          uiInitVal: �ź����ĳ�ʼֵ.
                     SEMEMPTY: �ź���Ϊ��״̬, ���ɻ�ȡ. �ò�����������SEMMUT�ź���.
                     SEMFULL: �ź���Ϊ��״̬, �ɻ�ȡ.
                     ����: �ź�����ʼ�ļ���ֵ. �ò���ֻ������SEMCNT�ź���.
�� �� ֵ: NULL: �����ź���ʧ��.
          ����: �����ź����ɹ�, ����ֵΪ�������ź�����ָ��.
***********************************************************************************/
M_SEM* MDS_SemCreate(M_SEM* pstrSem, U32 uiSemOpt, U32 uiInitVal)
{
    U8* pucSemMemAddr;

    /* �ź���ѡ���� */
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

    /* �������ź�����ʼֵֻ���ǿջ����� */
    if(SEMBIN == (SEMTYPEMASK & uiSemOpt))
    {
        if((SEMEMPTY != uiInitVal) && (SEMFULL != uiInitVal))
        {
            return (M_SEM*)NULL;
        }
    }
    /* �����ź�����ʼֵֻ������ */
    else if(SEMMUT == (SEMTYPEMASK & uiSemOpt))
    {
        if(SEMFULL != uiInitVal)
        {
            return (M_SEM*)NULL;
        }
    }

    /* ����ָ��Ϊ��, ��Ҫ�Լ������ڴ� */
    if(NULL == pstrSem)
    {
        (void)MDS_IntLock();

        pucSemMemAddr = malloc(sizeof(M_SEM));
        if(NULL == pucSemMemAddr)
        {
            (void)MDS_IntUnlock();

            /* ���벻���ڴ�, ����ʧ�� */
            return (M_SEM*)NULL;
        }

        (void)MDS_IntUnlock();

        /* �ź���ָ��������ڴ� */
        pstrSem = (M_SEM*)pucSemMemAddr;
    }
    else /* ����ڲ��������ź���ʹ�õ��ڴ�, �����Լ����� */
    {
        /* ���ź����ڴ��ַ��Ϊ�� */
        pucSemMemAddr = (U8*)NULL;
    }

    /* ��ʼ���ź������ȱ� */
    MDS_TaskSchedTabInit(&pstrSem->strSemTab);

    /* ��ʼ���ź�����ʼֵ */
    pstrSem->uiCounter = uiInitVal;

    /* ��ʼ���ź������� */
    pstrSem->uiSemOpt = uiSemOpt;

    /* �����ź����ڴ����ʼ��ַ */
    pstrSem->pucSemMem = pucSemMemAddr;

    /* û�������ȡ�������ź��� */
    pstrSem->pstrSemTask = (M_TCB*)NULL;

    return pstrSem;
}

/***********************************************************************************
��������: ��ȡһ���ź���. ���ź�����Ϊ��ʱ�ɻ�ȡ, ���ź���Ϊ��ʱ�޷���ȡ, ���ܻ�ʹ��
          ���Ϊpend״̬, ���������л�.
��ڲ���: pstrSem: Ҫ��ȡ���ź���ָ��.
          uiDelayTick: ��ȡ�ź����ȴ���ʱ��, ��λ: tick.
                       SEMNOWAIT: ���ȴ�, �����Ƿ��ȡ���ź���������ִ�и�����.
                       SEMWAITFEV: ���õȴ�, ����ȡ�����ź�����������л�Ϊpend״̬
                                   һֱ�ȴ�, �ڼ����ܻ�ȡ���ź�����ת��Ϊready̬.
                       ����: ���޷���ȡ���ź�����������л�Ϊpend״̬, ���ڸ�ʱ����
                             �ɻ�ȡ���ź���������������pend״̬, ����RTN_SUCD, ��
                             �ڸ�ʱ���ڻ�ȡ�����ź���������������pend״̬, ����
                             RTN_SMTKTO.
�� �� ֵ: RTN_SUCD: ���ӳ�ʱ���ڻ�ȡ���ź���.
          RTN_FAIL: ��ȡ�ź���ʧ��.
          RTN_SMTKTO: �ȴ��ź�����ʱ��ľ�, ��ʱ����.
          RTN_SMTKRT: ʹ�ò��ȴ�ʱ�����û�л�ȡ���ź���, �������̷���.
          RTN_SMTKDL: �ź�����ɾ��.
          RTN_SMTKOV: �����ź�������ֵ���.
***********************************************************************************/
U32 MDS_SemTake(M_SEM* pstrSem, U32 uiDelayTick)
{
    /* ��ڲ������ */
    if(NULL == pstrSem)
    {
        return RTN_FAIL;
    }

    /* ���ж���ʹ���ź��� */
    if(RTN_SUCD == MDS_RunInInt())
    {
        /* �ж��в���ʹ�û����ź��� */
        if(SEMMUT == (SEMTYPEMASK & pstrSem->uiSemOpt))
        {
            return RTN_FAIL;
        }
        else
        {
            /* �ж���ʹ�ö������ź����ͼ����ź���ʱ�����еȴ�ʱ�� */
            if(SEMNOWAIT != uiDelayTick)
            {
                return RTN_FAIL;
            }
        }
    }

    (void)MDS_IntLock();

    /* ��ȡ�ź���ʱ���ȴ�ʱ�� */
    if(SEMNOWAIT == uiDelayTick)
    {
        /* �������ź��� */
        if(SEMBIN == (SEMTYPEMASK & pstrSem->uiSemOpt))
        {
            /* �ź���Ϊ��, �ɻ�ȡ���ź��� */
            if(SEMFULL == pstrSem->uiCounter)
            {
                /* ��ȡ���ź������ź�����Ϊ�� */
                pstrSem->uiCounter = SEMEMPTY;

                (void)MDS_IntUnlock();

                return RTN_SUCD;
            }
            else /* �ź���Ϊ��, �޷���ȡ���ź��� */
            {
                (void)MDS_IntUnlock();

                return RTN_SMTKRT;
            }
        }
        /* �����ź��� */
        else if(SEMCNT == (SEMTYPEMASK & pstrSem->uiSemOpt))
        {
            /* �ź�����Ϊ��, �ɻ�ȡ���ź��� */
            if(SEMEMPTY != pstrSem->uiCounter)
            {
                /* ��ȡ���ź������ź�������ֵ-1 */
                pstrSem->uiCounter--;

                (void)MDS_IntUnlock();

                return RTN_SUCD;
            }
            else /* �ź���Ϊ��, �޷���ȡ���ź��� */
            {
                (void)MDS_IntUnlock();

                return RTN_SMTKRT;
            }
        }
        else /* �����ź��� */
        {
            /* �ź���Ϊ��, �ɻ�ȡ���ź��� */
            if(SEMFULL == pstrSem->uiCounter)
            {
                /* ��ȡ���ź������ź�������ֵ-1 */
                pstrSem->uiCounter--;

                /* �������ź������������ */
                pstrSem->pstrSemTask = gpstrCurTcb;

                (void)MDS_IntUnlock();

                return RTN_SUCD;
            }
            else /* �ź�����Ϊ��, ˵����һ�������ȡ�� */
            {
                /* �����ź����Ѿ����������ȡ����Լ�����ȡ */
                if(pstrSem->pstrSemTask == gpstrCurTcb)
                {
                    /* �ź�������δ�� */
                    if(SEMEMPTY != pstrSem->uiCounter)
                    {
                        /* �ź�������ֵ-1 */
                        pstrSem->uiCounter--;

                        (void)MDS_IntUnlock();

                        return RTN_SUCD;
                    }
                    else /* �ź��������ѿ�, ����ʧ�� */
                    {
                        (void)MDS_IntUnlock();

                        return RTN_SMTKOV;
                    }
                }
                else /* �����������ȡ�򷵻��޷���ȡ�ź��� */
                {
                    (void)MDS_IntUnlock();

                    return RTN_SMTKRT;
                }
            }
        }
    }
    else /* ��ȡ�ź���ʱ��Ҫ�ȴ�ʱ�� */
    {
        /* �������ź��� */
        if(SEMBIN == (SEMTYPEMASK & pstrSem->uiSemOpt))
        {
            /* �ź���Ϊ��, �ɻ�ȡ���ź��� */
            if(SEMFULL == pstrSem->uiCounter)
            {
                /* ��ȡ���ź������ź�����Ϊ�� */
                pstrSem->uiCounter = SEMEMPTY;

                (void)MDS_IntUnlock();

                return RTN_SUCD;
            }
            else /* �ź���Ϊ��, �޷���ȡ���ź���, ��Ҫ�л����� */
            {
                /* ��������Ϊpend״̬ */
                if(RTN_FAIL == MDS_TaskPend(pstrSem, uiDelayTick))
                {
                    (void)MDS_IntUnlock();

                    /* ����pendʧ�� */
                    return RTN_FAIL;
                }

                (void)MDS_IntUnlock();

                /* ʹ�����жϵ������� */
                MDS_TaskSwiSched();

                /* ����pend����ֵ, ��ֵ������pend״̬����ʱ��������uiDelayTick�� */
                return gpstrCurTcb->strTaskOpt.uiDelayTick;
            }
        }
        /* �����ź��� */
        else if(SEMCNT == (SEMTYPEMASK & pstrSem->uiSemOpt))
        {
            /* �ź�����Ϊ��, �ɻ�ȡ���ź��� */
            if(SEMEMPTY != pstrSem->uiCounter)
            {
                /* ��ȡ���ź������ź�������ֵ-1 */
                pstrSem->uiCounter--;

                (void)MDS_IntUnlock();

                return RTN_SUCD;
            }
            else /* �ź���Ϊ��, �޷���ȡ���ź���, ��Ҫ�л����� */
            {
                /* ��������Ϊpend״̬ */
                if(RTN_FAIL == MDS_TaskPend(pstrSem, uiDelayTick))
                {
                    (void)MDS_IntUnlock();

                    /* ����pendʧ�� */
                    return RTN_FAIL;
                }

                (void)MDS_IntUnlock();

                /* ʹ�����жϵ������� */
                MDS_TaskSwiSched();

                /* ����pend����ֵ, ��ֵ������pend״̬����ʱ��������uiDelayTick�� */
                return gpstrCurTcb->strTaskOpt.uiDelayTick;
            }
        }
        else /* �����ź��� */
        {
            /* �ź���Ϊ��, �ɻ�ȡ���ź��� */
            if(SEMFULL == pstrSem->uiCounter)
            {
                /* ��ȡ���ź������ź�������ֵ-1 */
                pstrSem->uiCounter--;

                /* �������ź������������ */
                pstrSem->pstrSemTask = gpstrCurTcb;

                (void)MDS_IntUnlock();

                return RTN_SUCD;
            }
            else /* �ź�����Ϊ��, ˵����һ�������ȡ�� */
            {
                /* �����ź����Ѿ����������ȡ����Լ�����ȡ */
                if(pstrSem->pstrSemTask == gpstrCurTcb)
                {
                    /* �ź�������δ�� */
                    if(SEMEMPTY != pstrSem->uiCounter)
                    {
                        /* �ź�������ֵ-1 */
                        pstrSem->uiCounter--;

                        (void)MDS_IntUnlock();

                        return RTN_SUCD;
                    }
                    else /* �ź��������ѿ�, ����ʧ�� */
                    {
                        (void)MDS_IntUnlock();

                        return RTN_SMTKOV;
                    }
                }
                else /* �����������ȡ, ��Ҫ�л����� */
                {
#ifdef MDS_TASKPRIOINHER

                    /* �ź����������������ȼ��̳й��� */
                    if(SEMPRIINH == (pstrSem->uiSemOpt & SEMPRIINH))
                    {
                        MDS_TaskPrioInheritance(pstrSem->pstrSemTask,
                                                gpstrCurTcb->ucTaskPrio);
                    }

#endif

                    /* ����ǰ������Ϊpend״̬ */
                    if(RTN_FAIL == MDS_TaskPend(pstrSem, uiDelayTick))
                    {
                        (void)MDS_IntUnlock();

                        /* ����pendʧ�� */
                        return RTN_FAIL;
                    }

                    (void)MDS_IntUnlock();

                    /* ʹ�����жϵ������� */
                    MDS_TaskSwiSched();

                    /* ����pend�ķ���ֵ, ��ֵ������pend״̬����ʱ��������uiDelayTick
                       �� */
                    return gpstrCurTcb->strTaskOpt.uiDelayTick;
                }
            }
        }
    }
}

/***********************************************************************************
��������: �ͷ�һ���ź���. �ź���Ϊ����ʱ, �������񱻸��ź������������һ���������.
��ڲ���: pstrSem: ��Ҫ�ͷŵ��ź���ָ��.
�� �� ֵ: RTN_SUCD: �ͷ��ź����ɹ�.
          RTN_FAIL: �ͷ��ź���ʧ��.
          RTN_SMGVOV: �ͷ��ź������.
***********************************************************************************/
U32 MDS_SemGive(M_SEM* pstrSem)
{
#define NOTCHECKPENDTASK                0   /* ������������ */
#define CHECKPENDTASK                   1   /* ���������� */
    M_TCB* pstrTcb;
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U32 uiRtn;
    U8 ucTaskPrio;
    U8 ucPendTaskFlag;

    /* ��ڲ������ */
    if(NULL == pstrSem)
    {
        return RTN_FAIL;
    }

    /* ���ж���ʹ���ź��� */
    if(RTN_SUCD == MDS_RunInInt())
    {
        /* �ж��в���ʹ�û����ź��� */
        if(SEMMUT == (SEMTYPEMASK & pstrSem->uiSemOpt))
        {
            return RTN_FAIL;
        }
    }

    uiRtn = RTN_SUCD;

    (void)MDS_IntLock();

    /* �������ź��� */
    if(SEMBIN == (SEMTYPEMASK & pstrSem->uiSemOpt))
    {
        /* �ź���Ϊ��, ��Ҫ��鱻���ź������������� */
        if(SEMEMPTY == pstrSem->uiCounter)
        {
            ucPendTaskFlag = CHECKPENDTASK;
        }
        else /* �ź����ǿ�, ����Ҫ��鱻���ź������������� */
        {
            ucPendTaskFlag = NOTCHECKPENDTASK;
        }
    }
    /* �����ź��� */
    else if(SEMCNT == (SEMTYPEMASK & pstrSem->uiSemOpt))
    {
        /* �ź���Ϊ��, ��Ҫ��鱻���ź������������� */
        if(SEMEMPTY == pstrSem->uiCounter)
        {
            ucPendTaskFlag = CHECKPENDTASK;
        }
        else /* �ź����ǿ�, ����Ҫ��鱻���ź������������� */
        {
            ucPendTaskFlag = NOTCHECKPENDTASK;

            /* �ź���δ�� */
            if(SEMFULL != pstrSem->uiCounter)
            {
                /* �ͷ��ź������ź���+1 */
                pstrSem->uiCounter++;
            }
            else /* �ź������� */
            {
                uiRtn = RTN_SMGVOV;
            }
        }
    }
    else /* �����ź��� */
    {
        /* ���ͷŻ����ź����������ǻ�ȡ�������ź���������, �򷵻�ʧ�� */
        if(pstrSem->pstrSemTask != gpstrCurTcb)
        {
            (void)MDS_IntUnlock();

            return RTN_FAIL;
        }

        /* �ͷ�һ�λ����ź���, �ź�������ֵ+1 */
        pstrSem->uiCounter++;

        /* ͬһ�����ȡ���ͷŻ����ź�����ƽ��, ����Ҫ��鱻���ź������������� */
        if(SEMFULL != pstrSem->uiCounter)
        {
            ucPendTaskFlag = NOTCHECKPENDTASK;
        }
        else /* ͬһ�����ȡ���ͷŻ����ź���ƽ��, ��Ҫ��鱻���ź������������� */
        {
            ucPendTaskFlag = CHECKPENDTASK;
        }
    }

    /* ��Ҫ��鱻���ź������������� */
    if(CHECKPENDTASK == ucPendTaskFlag)
    {
#ifdef MDS_TASKPRIOINHER

        /* �ָ���ǰ��������ȼ� */
        MDS_TaskPrioResume(gpstrCurTcb);

#endif

        /* �ڱ����ź��������������л�ȡ��Ҫ�ͷŵ����� */
        pstrTcb = MDS_SemGetActiveTask(pstrSem);

        /* ������������, �ͷ����� */
        if(NULL != pstrTcb)
        {
            /* ���ź������ȱ��������� */
            (void)MDS_TaskDelFromSemTab(pstrTcb);

            /* ������delay�����delay���� */
            if(DELAYQUEFLAG == (pstrTcb->uiTaskFlag & DELAYQUEFLAG))
            {
                pstrNode = &pstrTcb->strTcbQue.strQueHead;
                (void)MDS_DlistCurNodeDelete(&gstrDelayTab, pstrNode);

                /* ��������delay���־ */
                pstrTcb->uiTaskFlag &= (~((U32)DELAYQUEFLAG));
            }

            /* ��������pend״̬ */
            pstrTcb->strTaskOpt.ucTaskSta &= ~((U8)TASKPEND);

            /* ����uiDelayTick��������pend����ķ���ֵ */
            pstrTcb->strTaskOpt.uiDelayTick = RTN_SUCD;

            /* ����������ӵ�ready���� */
            pstrNode = &pstrTcb->strTcbQue.strQueHead;
            ucTaskPrio = pstrTcb->ucTaskPrio;
            pstrList = &gstrReadyTab.astrList[ucTaskPrio];
            pstrPrioFlag = &gstrReadyTab.strFlag;

            MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);

            /* ���������ready״̬ */
            pstrTcb->strTaskOpt.ucTaskSta |= TASKREADY;

            /* �����ȡ�������ź��������� */
            if(SEMMUT == (SEMTYPEMASK & pstrSem->uiSemOpt))
            {
                pstrSem->pstrSemTask = pstrTcb;     /* �»�ȡ�������ź��������� */
                pstrSem->uiCounter--;               /* �ź�������ȡһ�� */
            }

            (void)MDS_IntUnlock();

            /* ʹ�����жϵ������� */
            MDS_TaskSwiSched();

            return uiRtn;
        }
        else /* û������������ */
        {
            /* �������ź��� */
            if(SEMBIN == (SEMTYPEMASK & pstrSem->uiSemOpt))
            {
                /* �ͷ��ź������ź�����Ϊ�� */
                pstrSem->uiCounter = SEMFULL;
            }
            /* �����ź��� */
            else if(SEMCNT == (SEMTYPEMASK & pstrSem->uiSemOpt))
            {
                /* �ͷ��ź������ź���+1. �ߴ˷�֧�ź���+1������� */
                pstrSem->uiCounter++;
            }
            else /* �����ź��� */
            {
                /* û�������ȡ�������ź��� */
                pstrSem->pstrSemTask = (M_TCB*)NULL;
            }
        }
    }

    (void)MDS_IntUnlock();

    return uiRtn;
}

/***********************************************************************************
��������: �ͷű����ź�������������������, ȥ����Щ�����pend״̬, ��ָ�������������
          ����ֵ.
��ڲ���: pstrSem: Ҫ�ͷŵ��ź���ָ��.
          uiRtnValue: ָ������������ķ���ֵ
�� �� ֵ: RTN_SUCD: �ͷ��ź����ɹ�.
          RTN_FAIL: �ͷ��ź���ʧ��.
***********************************************************************************/
U32 MDS_SemFlushValue(M_SEM* pstrSem, U32 uiRtnValue)
{
    M_TCB* pstrTcb;
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8 ucTaskPrio;

    /* ��ڲ������ */
    if(NULL == pstrSem)
    {
        return RTN_FAIL;
    }

    /* �����ź������ܱ������ͷ� */
    if(SEMMUT == (SEMTYPEMASK & pstrSem->uiSemOpt))
    {
        return RTN_FAIL;
    }

    (void)MDS_IntLock();

    /* �ڱ����ź��������������л�ȡ��Ҫ�ͷŵ����� */
    while(1)
    {
        pstrTcb = MDS_SemGetActiveTask(pstrSem);

        /* ������������, �ͷ����� */
        if(NULL != pstrTcb)
        {
            /* ���ź������ȱ��������� */
            (void)MDS_TaskDelFromSemTab(pstrTcb);

            /* ������delay�����delay���� */
            if(DELAYQUEFLAG == (pstrTcb->uiTaskFlag & DELAYQUEFLAG))
            {
                pstrNode = &pstrTcb->strTcbQue.strQueHead;
                (void)MDS_DlistCurNodeDelete(&gstrDelayTab, pstrNode);

                /* ��������delay���־ */
                pstrTcb->uiTaskFlag &= (~((U32)DELAYQUEFLAG));
            }

            /* ��������pend״̬ */
            pstrTcb->strTaskOpt.ucTaskSta &= ~((U8)TASKPEND);

            /* ����uiDelayTick��������pend����ķ���ֵ */
            pstrTcb->strTaskOpt.uiDelayTick = uiRtnValue;

            /* ����������ӵ�ready���� */
            pstrNode = &pstrTcb->strTcbQue.strQueHead;
            ucTaskPrio = pstrTcb->ucTaskPrio;
            pstrList = &gstrReadyTab.astrList[ucTaskPrio];
            pstrPrioFlag = &gstrReadyTab.strFlag;

            MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);

            /* ���������ready״̬ */
            pstrTcb->strTaskOpt.ucTaskSta |= TASKREADY;
        }
        else /* û������������, ����ѭ������ */
        {
            break;
        }
    }

    /* ���ź�����Ϊ�� */
    pstrSem->uiCounter = SEMEMPTY;

    (void)MDS_IntUnlock();

    /* ʹ�����жϵ������� */
    MDS_TaskSwiSched();

    return RTN_SUCD;
}

/***********************************************************************************
��������: �ͷű����ź�������������������, ȥ����Щ�����pend״̬.
��ڲ���: pstrSem: ��Ҫ�ͷŵ��ź���ָ��.
�� �� ֵ: RTN_SUCD: �ͷ��ź����ɹ�.
          RTN_FAIL: �ͷ��ź���ʧ��.
***********************************************************************************/
U32 MDS_SemFlush(M_SEM* pstrSem)
{
    /* �ͷ��ź������������������� */
    return MDS_SemFlushValue(pstrSem, RTN_SUCD);
}

/***********************************************************************************
��������: ɾ��һ���ź���.
��ڲ���: pstrSem: ��Ҫɾ�����ź���ָ��.
�� �� ֵ: RTN_SUCD: ɾ���ź����ɹ�.
          RTN_FAIL: ɾ���ź���ʧ��.
***********************************************************************************/
U32 MDS_SemDelete(M_SEM* pstrSem)
{
    /* ��ڲ������ */
    if(NULL == pstrSem)
    {
        return RTN_FAIL;
    }

    /* �ǻ����ź������ͷű��������������� */
    if(SEMMUT != (SEMTYPEMASK & pstrSem->uiSemOpt))
    {
        /* �ͷ��ź������������������� */
        if(RTN_SUCD != MDS_SemFlushValue(pstrSem, RTN_SMTKDL))
        {
            return RTN_FAIL;
        }
    }

    /* ����Ǵ����ź�������������ź����ڴ�, ����Ҫ�ͷ� */
    if(NULL != pstrSem->pucSemMem)
    {
        (void)MDS_IntLock();

        free(pstrSem->pucSemMem);

        (void)MDS_IntUnlock();
    }

    return RTN_SUCD;
}

