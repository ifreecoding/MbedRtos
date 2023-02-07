
#include "mindows_inner.h"


/***********************************************************************************
��������: ����һ������.
��ڲ���: vfFuncPointer: ����������ʹ�ú�����ָ��.
          pvPara: ������ڲ���ָ��.
          pucTaskStack: ������ʹ��ջ�������ʼ��ַ.
          uiStackSize: ջ��С, ��λ: �ֽ�.
          ucTaskPrio: �������ȼ�.
          pstrTaskOpt: �������ָ��.
�� �� ֵ: NULL: ��������ʧ��.
          ����: �����TCBָ��.
***********************************************************************************/
M_TCB* MDS_TaskCreate(VFUNC vfFuncPointer, void* pvPara, U8* pucTaskStack,
                      U32 uiStackSize, U8 ucTaskPrio, M_TASKOPT* pstrTaskOpt)
{
    M_TCB* pstrTcb;

    /* pstrTaskOpt�������Ǳ���, ������� */

    /* �Դ���������ʹ�ú�����ָ��Ϸ��Խ��м�� */
    if(NULL == vfFuncPointer)
    {
        /* ָ��Ϊ��, ����ʧ�� */
        return (M_TCB*)NULL;
    }

    /* ������ջ�Ϸ��Խ��м�� */
    if((NULL == pucTaskStack) || (0 == uiStackSize))
    {
        /* ջ���Ϸ�, ����ʧ�� */
        return (M_TCB*)NULL;
    }

    /* �����������ʱ������״̬�Ϸ��Խ��м�� */
    if(NULL != pstrTaskOpt)
    {
        /* �����ڵ�����״̬����ʧ�� */
        if(!((TASKREADY == pstrTaskOpt->ucTaskSta)
             || (TASKDELAY == pstrTaskOpt->ucTaskSta)))
        {
            return (M_TCB*)NULL;
        }
    }

    /* ���������ȼ���� */
    if(USERROOT == MDS_GetUser())
    {
        /* ����ROOT�û�, �������ȼ����ܵ���������ȼ� */
        if(ucTaskPrio > LOWESTPRIO)
        {
            return (M_TCB*)NULL;
        }
    }
    else //if(USERGUEST == MDS_GetUser())
    {
        /* ����GUEST�û�, �����ܸ����û�������ȼ�, ���ܵ����û�������ȼ� */
        if((ucTaskPrio < USERHIGHESTPRIO) || (ucTaskPrio > USERLOWESTPRIO))
        {
            return (M_TCB*)NULL;
        }
    }

    /* ��ʼ��TCB */
    pstrTcb = MDS_TaskTcbInit(vfFuncPointer, pvPara, pucTaskStack, uiStackSize,
                              ucTaskPrio, pstrTaskOpt);

    /* ��ʼ��TCBʧ��, �򷵻�ʧ�� */
    if(NULL == pstrTcb)
    {
        return NULL;
    }

    /* �ڲ���ϵͳ״̬�´����������Ҫ����������� */
    if(SYSTEMSCHEDULE == guiSystemStatus)
    {
        /* ʹ�����жϵ������� */
        MDS_TaskSwiSched();
    }

    return pstrTcb;
}

/***********************************************************************************
��������: ��ʼ������TCB, ����������������ǰTCB��״̬.
��ڲ���: vfFuncPionter: ����������ʹ�õĺ���ָ��.
          pvPara: ������ڲ���ָ��.
          pucTaskStack: ������ʹ�õ�ջ��ַ.
          uiStackSize: ջ��С, ��λ: �ֽ�.
          ucTaskPrio: �������ȼ�.
          pstrTaskOpt: �������ָ��.
�� �� ֵ: NULL: ��������ʧ��.
          ����: ��������ɹ�, ����ֵΪ�����TCBָ��.
***********************************************************************************/
M_TCB* MDS_TaskTcbInit(VFUNC vfFuncPointer, void* pvPara, U8* pucTaskStack,
                       U32 uiStackSize, U8 ucTaskPrio, M_TASKOPT* pstrTaskOpt)
{
    M_TCB* pstrTcb;
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8* pucStackBy4;

    /* ջ����ַ, ��Ҫ4�ֽڶ��� */
    pucStackBy4 = (U8*)(((U32)pucTaskStack + uiStackSize) & ALIGN4MASK);

    /* TCB�ṹ��ŵĵ�ַ, ��Ҫ8�ֽڶ��� */
    pstrTcb = (M_TCB*)(((U32)pucStackBy4 - sizeof(M_TCB)) & STACKALIGNMASK);

    /* ��ʼ������ջ */
    MDS_TaskStackInit(pstrTcb, vfFuncPointer, pvPara);

    /* �Ƚ������־��ʼ��Ϊȫ��, ������Ϊ�����Ӿ���Ĺ��ܱ�־ */
    pstrTcb->uiTaskFlag = 0;

    /* ��ʼ��ָ��TCB��ָ�� */
    pstrTcb->strTcbQue.pstrTcb = pstrTcb;

    /* ��ʼ���������ȼ� */
    pstrTcb->ucTaskPrio = ucTaskPrio;

    /* û���������������״̬����Ϊready̬ */
    if(NULL == pstrTaskOpt)
    {
        pstrTcb->strTaskOpt.ucTaskSta = TASKREADY;
    }
    else /* ����������򽫲������Ƶ�TCB�� */
    {
        pstrTcb->strTaskOpt.ucTaskSta = pstrTaskOpt->ucTaskSta;
        pstrTcb->strTaskOpt.uiDelayTick = pstrTaskOpt->uiDelayTick;
    }

    /* ���ж�, ��ֹ��������Ӱ�� */
    (void)MDS_IntLock();

    /* �������������ready̬, ���������ready�� */
    if(TASKREADY == (TASKREADY & pstrTcb->strTaskOpt.ucTaskSta))
    {
        pstrList = &gstrReadyTab.astrList[ucTaskPrio];
        pstrNode = &pstrTcb->strTcbQue.strQueHead;
        pstrPrioFlag = &gstrReadyTab.strFlag;

        /* �����������ӵ�ready���� */
        MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);
    }

    /* �������������delay̬, ���������delay�� */
    if(TASKDELAY == (TASKDELAY & pstrTcb->strTaskOpt.ucTaskSta))
    {
        /* �����õȴ�����Ź���delay�� */
        if(DELAYWAITFEV != pstrTaskOpt->uiDelayTick)
        {
            /* �����½�������ӳ�ʱ�� */
            pstrTcb->uiStillTick = guiTick + pstrTaskOpt->uiDelayTick;

            /* ������������ȡdelay���ڵ㲢���뵽delay�� */
            pstrNode = &pstrTcb->strTcbQue.strQueHead;
            MDS_TaskAddToDelayTab(pstrNode);

            /* ��������delay����־ */
            pstrTcb->uiTaskFlag |= DELAYQUEFLAG;
        }
    }

    /* ��������������ж�, ����������� */
    (void)MDS_IntUnlock();

    return pstrTcb;
}

/***********************************************************************************
��������: �������delay̬, �ӳ�ָ����ʱ��. ���ô˺������������л�.
��ڲ���: uiDelaytick: ������Ҫ�ӳٵ�ʱ��, ��λ: tick.
                       DELAYNOWAIT: ���ȴ�, �����������л�.
                       DELAYWAITFEV: ����һֱ�ȴ�.
                       ����: ������Ҫ�ӳٵ�tick��Ŀ.
�� �� ֵ: RTN_SUCD: �����ӳٳɹ�.
          RTN_FAIL: �����ӳ�ʧ��.
          RTN_TKDLTO: �����ӳ�ʱ��ľ�, ��ʱ����.
          RTN_TKDLBK: �����ӳ�״̬���ж�, ���񷵻�.
***********************************************************************************/
U32 MDS_TaskDelay(U32 uiDelayTick)
{
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8 ucTaskPrio;

    /* �ӳ�ʱ�䲻Ϊ0 tick��������� */
    if(DELAYNOWAIT != uiDelayTick)
    {
        /* idle�����ܴ���delay״̬ */
        if(gpstrCurTcb == gpstrIdleTaskTcb)
        {
            return RTN_FAIL;
        }

        /* ��ȡ��ǰ�������ص��Ȳ��� */
        ucTaskPrio = gpstrCurTcb->ucTaskPrio;
        pstrList = &gstrReadyTab.astrList[ucTaskPrio];
        pstrPrioFlag = &gstrReadyTab.strFlag;

        (void)MDS_IntLock();

        /* ����ǰ�����ready����� */
        pstrNode = MDS_TaskDelFromSchedTab(pstrList, pstrPrioFlag, ucTaskPrio);

        /* ��������ready״̬ */
        gpstrCurTcb->strTaskOpt.ucTaskSta &= ~((U8)TASKREADY);

        /* ���µ�ǰ������ӳ�ʱ�� */
        gpstrCurTcb->strTaskOpt.uiDelayTick = uiDelayTick;

        /* �����õȴ�����Ź���delay�� */
        if(DELAYWAITFEV != uiDelayTick)
        {
            /* ���������ӳٽ�����ʱ�� */
            gpstrCurTcb->uiStillTick = guiTick + uiDelayTick;

            /* ����ǰ������뵽delay�� */
            MDS_TaskAddToDelayTab(pstrNode);

            /* ��������delay����־ */
            gpstrCurTcb->uiTaskFlag |= DELAYQUEFLAG;
        }

        /* ���������delay״̬ */
        gpstrCurTcb->strTaskOpt.ucTaskSta |= TASKDELAY;

        (void)MDS_IntUnlock();
    }
    else /* �����ӳ�, �����������л� */
    {
        /* ����uiDelayTick���������ӳ�����ķ���ֵ */
        gpstrCurTcb->strTaskOpt.uiDelayTick = RTN_SUCD;
    }

    /* ʹ�����жϵ������� */
    MDS_TaskSwiSched();

    /* �����ӳ�����ķ���ֵ, �����delay״̬����ʱ����ֵ��������uiDelayTick�� */
    return gpstrCurTcb->strTaskOpt.uiDelayTick;
}

/***********************************************************************************
��������: ������ֻ�ܻ��������delay״̬.
��ڲ���: pstrTcb: �����������TCBָ��.
�� �� ֵ: RTN_SUCD: �����Ѳ����ɹ�.
          RTN_FAIL: �����Ѳ���ʧ��.
***********************************************************************************/
U32 MDS_TaskWake(M_TCB* pstrTcb)
{
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8 ucTaskPrio;

    /* ��ڲ������ */
    if(NULL == pstrTcb)
    {
        return RTN_FAIL;
    }

    (void)MDS_IntLock();

    /* �����Ի��������delay״̬ */
    if(TASKDELAY != (TASKDELAY & pstrTcb->strTaskOpt.ucTaskSta))
    {
        (void)MDS_IntUnlock();

        return RTN_FAIL;
    }

    pstrNode = &pstrTcb->strTcbQue.strQueHead;

    /* �����õȴ�����Ŵ�delay����� */
    if(DELAYWAITFEV != pstrTcb->strTaskOpt.uiDelayTick)
    {
        /* ��delay����������� */
        (void)MDS_DlistCurNodeDelete(&gstrDelayTab, pstrNode);

        /* ��������delay����־ */
        pstrTcb->uiTaskFlag &= (~((U32)DELAYQUEFLAG));
    }

    /* ��������delay״̬ */
    pstrTcb->strTaskOpt.ucTaskSta &= ~((U8)TASKDELAY);

    /* ����uiDelayTick���������ӳ�����ķ���ֵ */
    pstrTcb->strTaskOpt.uiDelayTick = RTN_TKDLBK;

    /* ��ȡ���������ز��� */
    ucTaskPrio = pstrTcb->ucTaskPrio;
    pstrList = &gstrReadyTab.astrList[ucTaskPrio];
    pstrPrioFlag = &gstrReadyTab.strFlag;

    /* �����������ӵ�ready���� */
    MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);

    /* ���������ready״̬ */
    pstrTcb->strTaskOpt.ucTaskSta |= TASKREADY;

    (void)MDS_IntUnlock();

    /* ʹ�����жϵ������� */
    MDS_TaskSwiSched();

    return RTN_SUCD;
}
