
#include <stdlib.h>
#include "mindows_inner.h"


#ifdef MDS_TASKROUNDROBIN
U32 guiTimeSlice;                       /* ��ת���ȵ�ʱ������ֵ */
U32 gauiSliceCnt[PRIORITYNUM];          /* ��ת���ȵ�ʱ�����ֵ */
#endif


#ifdef MDS_INCLUDETASKHOOK
VFHCRT gvfTaskCreateHook;               /* ���񴴽����ӱ��� */
VFHSWT gvfTaskSwitchHook;               /* �����л����ӱ��� */
VFHDLT gvfTaskDeleteHook;               /* ����ɾ�����ӱ��� */
#endif


/***********************************************************************************
��������: ����һ������.
��ڲ���: pucTaskName: ָ���������Ƶ�ָ��.
          vfFuncPointer: ����������ʹ�ú�����ָ��.
          pvPara: ������ڲ���ָ��.
          pucTaskStack: ������ʹ��ջ�������ʼ��ַ. ��Ϊ��, ����������������.
          uiStackSize: ջ��С, ��λ: �ֽ�.
          ucTaskPrio: �������ȼ�.
          pstrTaskOpt: �������ָ��.
�� �� ֵ: NULL: ��������ʧ��.
          ����: �����TCBָ��.
***********************************************************************************/
M_TCB* MDS_TaskCreate(U8* pucTaskName, VFUNC vfFuncPointer, void* pvPara,
           U8* pucTaskStack, U32 uiStackSize, U8 ucTaskPrio, M_TASKOPT* pstrTaskOpt)
{
    M_TCB* pstrTcb;

    /* pucTaskName��pstrTaskOpt�������Ǳ���, ������� */

    /* �Դ���������ʹ�ú�����ָ��Ϸ��Խ��м�� */
    if(NULL == vfFuncPointer)
    {
        /* ָ��Ϊ��, ����ʧ�� */
        return (M_TCB*)NULL;
    }

    /* ������ջ�Ϸ��Խ��м�� */
    if(0 == uiStackSize)
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
    pstrTcb = MDS_TaskTcbInit(pucTaskName, vfFuncPointer, pvPara, pucTaskStack,
                              uiStackSize, ucTaskPrio, pstrTaskOpt);

    /* ��ʼ��TCBʧ��, �򷵻�ʧ�� */
    if(NULL == pstrTcb)
    {
        return NULL;
    }

    /* �ڲ���ϵͳ״̬�´����������Ҫ����������� */
    if(SYSTEMSCHEDULE == guiSystemStatus)
    {
#ifdef MDS_INCLUDETASKHOOK

        /* ������񴴽������Ѿ��ҽӺ�����ִ�иú��� */
        if((VFHCRT)NULL != gvfTaskCreateHook)
        {
            gvfTaskCreateHook(pstrTcb);
        }

#endif

        /* ʹ�����жϵ������� */
        MDS_TaskSwiSched();
    }

    return pstrTcb;
}

/***********************************************************************************
��������: ɾ��һ������.
��ڲ���: pstrTcb: ��Ҫɾ���������TCBָ��.
�� �� ֵ: RTN_SUCD: ����ɾ���ɹ�.
          RTN_FAIL: ����ɾ��ʧ��.
***********************************************************************************/
U32 MDS_TaskDelete(M_TCB* pstrTcb)
{
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8 ucTaskPrio;
    U8 ucTaskSta;

    /* ��ڲ������ */
    if(NULL == pstrTcb)
    {
        return RTN_FAIL;
    }

    /* idle�����ܱ�ɾ�� */
    if(pstrTcb == gpstrIdleTaskTcb)
    {
        return RTN_FAIL;
    }

    (void)MDS_IntLock();

#ifdef MDS_INCLUDETASKHOOK

    /* �������ɾ�������Ѿ��ҽӺ�����ִ�иú��� */
    if((VFHDLT)NULL != gvfTaskDeleteHook)
    {
        gvfTaskDeleteHook(pstrTcb);
    }

#endif

    /* ��ȡҪɾ�����������״̬ */
    ucTaskSta = pstrTcb->strTaskOpt.ucTaskSta;

    /* ������ready�����ready���� */
    if(TASKREADY == (TASKREADY & ucTaskSta))
    {
        /* ��ȡ���������ص��Ȳ��� */
        ucTaskPrio = pstrTcb->ucTaskPrio;
        pstrList = &gstrReadyTab.astrList[ucTaskPrio];
        pstrPrioFlag = &gstrReadyTab.strFlag;

        /* ���������ready���� */
        (void)MDS_TaskDelFromSchedTab(pstrList, pstrPrioFlag, ucTaskPrio);
    }

    /* ������delay�����delay���� */
    if(DELAYQUEFLAG == (pstrTcb->uiTaskFlag & DELAYQUEFLAG))
    {
        /* ��ȡ������TCB�йҽ���delay���ȱ��ϵĽڵ� */
        pstrNode = &pstrTcb->strTcbQue.strQueHead;

        /* ��delay���������� */
        (void)MDS_DlistCurNodeDelete(&gstrDelayTab, pstrNode);
    }

    /* ����ӵ��pend״̬ */
    if(TASKPEND == (TASKPEND & ucTaskSta))
    {
        /* ���ź������ȱ������� */
        (void)MDS_TaskDelFromSemTab(pstrTcb);
    }

    /* ɾ�����ǵ�ǰ���� */
    if(pstrTcb == gpstrCurTcb)
    {
        /* ����ǰ�Ĵ������ջ��Ϣ���浽ָ���ڴ��� */
        MDS_SaveTaskContext();

        /* ��gpstrCurTcb��ΪNULL, �����������������л�ʱ�����ݵ�ǰ���� */
        gpstrCurTcb = NULL;
    }

    /* ����������Լ������ջ, ����Ҫ�ͷ� */
    if(TASKSTACKFLAG == (pstrTcb->uiTaskFlag & TASKSTACKFLAG))
    {
        free(pstrTcb->pucTaskStack);
    }

    (void)MDS_IntUnlock();

    /* ʹ�����жϵ������� */
    MDS_TaskSwiSched();

    return RTN_SUCD;
}

/***********************************************************************************
��������: ɾ����������.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskSelfDelete(void)
{
    (void)MDS_TaskDelete(gpstrCurTcb);
}

/***********************************************************************************
��������: ��ʼ������TCB, ����������������ǰTCB��״̬.
��ڲ���: pucTaskName: ָ���������Ƶ�ָ��.
          vfFuncPionter: ����������ʹ�õĺ���ָ��.
          pvPara: ������ڲ���ָ��.
          pucTaskStack: ������ʹ�õ�ջ��ַ.
          uiStackSize: ջ��С, ��λ: �ֽ�.
          ucTaskPrio: �������ȼ�.
          pstrTaskOpt: �������ָ��.
�� �� ֵ: NULL: ��������ʧ��.
          ����: ��������ɹ�, ����ֵΪ�����TCBָ��.
***********************************************************************************/
M_TCB* MDS_TaskTcbInit(U8* pucTaskName, VFUNC vfFuncPointer, void* pvPara,
           U8* pucTaskStack, U32 uiStackSize, U8 ucTaskPrio, M_TASKOPT* pstrTaskOpt)
{
    M_TCB* pstrTcb;
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8* pucStackBy4;
    U32 uiTaskFlag;

    /* ���ж�, ��ֹ��������Ӱ�� */
    (void)MDS_IntLock();

    /* ������ջΪ��, ���������Լ������ڴ� */
    if(NULL == pucTaskStack)
    {
        pucTaskStack = malloc(uiStackSize);
        if(NULL == pucTaskStack)
        {
            /* ����ǰ�����ж� */
            (void)MDS_IntUnlock();

            /* ���벻���ڴ�, ���ؿ�ָ�� */
            return (M_TCB*)NULL;
        }

        /* �������־��Ϊջ����״̬ */
        uiTaskFlag = TASKSTACKFLAG;
    }
    else /* �Ǵ����ջ, �Ƚ������־��Ϊ�� */
    {
        uiTaskFlag = 0;
    }

    /* ջ����ַ, ��Ҫ4�ֽڶ��� */
    pucStackBy4 = (U8*)(((U32)pucTaskStack + uiStackSize) & ALIGN4MASK);

    /* TCB�ṹ��ŵĵ�ַ, ��Ҫ8�ֽڶ��� */
    pstrTcb = (M_TCB*)(((U32)pucStackBy4 - sizeof(M_TCB)) & STACKALIGNMASK);

    /* ��������ջ��ʼ��ַ */
    pstrTcb->pucTaskStack = pucTaskStack;

    /* ��ʼ������ջ */
    MDS_TaskStackInit(pstrTcb, vfFuncPointer, pvPara);

    /* �Ƚ������־��ʼ��Ϊȫ��, ������Ϊ�����Ӿ���Ĺ��ܱ�־ */
    pstrTcb->uiTaskFlag = 0;

    /* �������־��Ϊ����ջ��״̬ */
    pstrTcb->uiTaskFlag |= uiTaskFlag;

    /* ��ʼ��ָ��TCB��ָ�� */
    pstrTcb->strTcbQue.pstrTcb = pstrTcb;
    pstrTcb->strSemQue.pstrTcb = pstrTcb;

    /* ��ʼ��ָ���������Ƶ�ָ�� */
    pstrTcb->pucTaskName = pucTaskName;

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

    /* �������������ready̬, ���������ready�� */
    if(TASKREADY == (TASKREADY & pstrTcb->strTaskOpt.ucTaskSta))
    {
        pstrList = &gstrReadyTab.astrList[ucTaskPrio];
        pstrNode = &pstrTcb->strTcbQue.strQueHead;
        pstrPrioFlag = &gstrReadyTab.strFlag;

        /* ����������ӵ�ready���� */
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

            /* ������������ȡdelay��ڵ㲢���뵽delay�� */
            pstrNode = &pstrTcb->strTcbQue.strQueHead;
            MDS_TaskAddToDelayTab(pstrNode);

            /* ��������delay���־ */
            pstrTcb->uiTaskFlag |= DELAYQUEFLAG;
        }
    }

    /* �������������ж�, ����������� */
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

        /* ����ǰ�����ready���� */
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

            /* ��������delay���־ */
            gpstrCurTcb->uiTaskFlag |= DELAYQUEFLAG;
        }

        /* ���������delay״̬ */
        gpstrCurTcb->strTaskOpt.ucTaskSta |= TASKDELAY;

#ifdef MDS_TASKROUNDROBIN

        /* ����ǰ�������ȼ�����ת���ȼ���ֵ��0 */
        gauiSliceCnt[gpstrCurTcb->ucTaskPrio] = 0;

#endif

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

    /* �����õȴ�����Ŵ�delay���� */
    if(DELAYWAITFEV != pstrTcb->strTaskOpt.uiDelayTick)
    {
        /* ��delay���������� */
        (void)MDS_DlistCurNodeDelete(&gstrDelayTab, pstrNode);

        /* ��������delay���־ */
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

    /* ����������ӵ�ready���� */
    MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);

    /* ���������ready״̬ */
    pstrTcb->strTaskOpt.ucTaskSta |= TASKREADY;

    (void)MDS_IntUnlock();

    /* ʹ�����жϵ������� */
    MDS_TaskSwiSched();

    return RTN_SUCD;
}

/***********************************************************************************
��������: �������pend̬, �ӳ�ָ����ʱ��. ���ô˺������������л�.
��ڲ���: pstrSem: ����������ź���ָ��.
          uiDelaytick: ������Ҫ�ӳٵ�ʱ��, ��λ: tick.
                       DELAYNOWAIT: ���ȴ�, �����������л�.
                       DELAYWAITFEV: ����һֱ�ȴ�.
                       ����: ������Ҫ�ӳٵ�tick��Ŀ.
�� �� ֵ: RTN_SUCD: ���������ɹ�.
          RTN_FAIL: ��������ʧ��.
***********************************************************************************/
U32 MDS_TaskPend(M_SEM* pstrSem, U32 uiDelayTick)
{
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8 ucTaskPrio;

    /* idle�����ܴ���pend״̬ */
    if(gpstrCurTcb == gpstrIdleTaskTcb)
    {
        return RTN_FAIL;
    }

    /* ��ȡ��ǰ�������ص��Ȳ��� */
    ucTaskPrio = gpstrCurTcb->ucTaskPrio;
    pstrList = &gstrReadyTab.astrList[ucTaskPrio];
    pstrPrioFlag = &gstrReadyTab.strFlag;

    /* ����ǰ�����ready���� */
    pstrNode = MDS_TaskDelFromSchedTab(pstrList, pstrPrioFlag, ucTaskPrio);

    /* ��������ready״̬ */
    gpstrCurTcb->strTaskOpt.ucTaskSta &= ~((U8)TASKREADY);

    /* ���µ�ǰ������ӳ�ʱ�� */
    gpstrCurTcb->strTaskOpt.uiDelayTick = uiDelayTick;

    /* �����õȴ�����Ź���delay�� */
    if(SEMWAITFEV != uiDelayTick)
    {
        gpstrCurTcb->uiStillTick = guiTick + uiDelayTick;

        /* ����ǰ������뵽delay�� */
        MDS_TaskAddToDelayTab(pstrNode);

        /* ��������delay���־ */
        gpstrCurTcb->uiTaskFlag |= DELAYQUEFLAG;
    }

    /* ����������ӵ��ź������ȱ��� */
    MDS_TaskAddToSemTab(gpstrCurTcb, pstrSem);

    /* ���������pend״̬ */
    gpstrCurTcb->strTaskOpt.ucTaskSta |= TASKPEND;

#ifdef MDS_TASKROUNDROBIN

    /* ����ǰ�������ȼ�����ת���ȼ���ֵ��0 */
    gauiSliceCnt[gpstrCurTcb->ucTaskPrio] = 0;

#endif

    return RTN_SUCD;
}

#ifdef MDS_TASKPRIOINHER

/***********************************************************************************
��������: �̳���������ȼ�.
��ڲ���: pstrTcb: ��Ҫ�̳����ȼ��������TCB.
          ucTaskPrio: ��Ҫ�̳е����ȼ�.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskPrioInheritance(M_TCB* pstrTcb, U8 ucTaskPrio)
{
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8 ucTaskPrioTemp;

    /* ��ڲ������ */
    if(NULL == pstrTcb)
    {
        return;
    }

    /* �����������ȼ����ڻ����Ҫ�̳е����ȼ�, ��ֱ�ӷ���, ���̳����ȼ� */
    if(pstrTcb->ucTaskPrio <= ucTaskPrio)
    {
        return;
    }

    /* ��ܱ���澯 */
    pstrNode = (M_DLIST*)NULL;
    pstrPrioFlag = (M_PRIOFLAG*)NULL;

    /* ���������ready��������Ҫ����ready��, �ȴ�ready��ɾ�� */
    if(TASKREADY == (TASKREADY & pstrTcb->strTaskOpt.ucTaskSta))
    {
        ucTaskPrioTemp = pstrTcb->ucTaskPrio;
        pstrList = &gstrReadyTab.astrList[ucTaskPrioTemp];
        pstrPrioFlag = &gstrReadyTab.strFlag;
        pstrNode = MDS_TaskDelFromSchedTab(pstrList, pstrPrioFlag, ucTaskPrioTemp);
    }

    /* ���������ȼ�δ�̳�״̬�²ű��������ԭʼ���ȼ� */
    if(TASKPRIINHFLAG != (pstrTcb->uiTaskFlag & TASKPRIINHFLAG))
    {
        pstrTcb->ucTaskPrioBackup = pstrTcb->ucTaskPrio;
    }

    /* �̳���������ȼ� */
    pstrTcb->ucTaskPrio = ucTaskPrio;

    /* �ü̳����ȼ�������Ϊ���ȼ��̳�״̬ */
    pstrTcb->uiTaskFlag |= TASKPRIINHFLAG;

    /* �����������ready��������Ҫ�����¹����ȼ��ĸ��������¼���ready�� */
    if(TASKREADY == (TASKREADY & pstrTcb->strTaskOpt.ucTaskSta))
    {
        pstrList = &gstrReadyTab.astrList[ucTaskPrio];
        MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);
    }
}

/***********************************************************************************
��������: �ָ�����̳е����ȼ�.
��ڲ���: pstrTcb: ��Ҫ�ָ����ȼ��������TCB.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskPrioResume(M_TCB* pstrTcb)
{
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8 ucTaskPrioTemp;

    /* ��ڲ������ */
    if(NULL == pstrTcb)
    {
        return;
    }

    /* �������û�д������ȼ��̳�״̬, ��ֱ�ӷ���, ����Ҫ�ָ�ԭ���ȼ� */
    if(TASKPRIINHFLAG != (pstrTcb->uiTaskFlag & TASKPRIINHFLAG))
    {
        return;
    }

    /* ��ܱ���澯 */
    pstrNode = (M_DLIST*)NULL;
    pstrPrioFlag = (M_PRIOFLAG*)NULL;

    /* ��Ҫ����ready��, �ȴ�ready��ɾ�� */
    ucTaskPrioTemp = pstrTcb->ucTaskPrio;
    pstrList = &gstrReadyTab.astrList[ucTaskPrioTemp];
    pstrPrioFlag = &gstrReadyTab.strFlag;
    pstrNode = MDS_TaskDelFromSchedTab(pstrList, pstrPrioFlag, ucTaskPrioTemp);

    /* ���������ȼ��ָ�Ϊԭ���ȼ�, ��������״̬�ָ�Ϊδ�̳�״̬ */
    pstrTcb->ucTaskPrio = pstrTcb->ucTaskPrioBackup;
    pstrTcb->uiTaskFlag &= (~((U32)TASKPRIINHFLAG));

    /* �����¹����ȼ��ĸ��������¼���ready�� */
    pstrList = &gstrReadyTab.astrList[pstrTcb->ucTaskPrio];
    MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, pstrTcb->ucTaskPrio);
}

#endif

#ifdef MDS_TASKROUNDROBIN

/***********************************************************************************
��������: ����ͬ�����ȼ���ת��������ֵ. ��ready�������뵱ǰ����ͬ�����ȼ�������, ��
          �ҵ�ǰ�������еļ���ֵ�Ѿ��ﵽ����ֵ, �򽫵�ǰ�������ready��, ����ready��
          ���¸�ͬ�����ȼ�������.
��ڲ���: uiTimeSlice: ͬ�����ȼ�������ת����ʱ������ֵ, ��λ: Tick. ��Ϊ0�����ִ
          ��ͬ�����ȼ��������ת����.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskTimeSlice(U32 uiTimeSlice)
{
    U32 i;

    (void)MDS_IntLock();

    /* ������ת����ʱ������ֵ */
    guiTimeSlice = uiTimeSlice;

    /* ��ת���ȵ�ʱ�����ֵ��0 */
    for(i = 0; i < PRIORITYNUM; i++)
    {
        gauiSliceCnt[i] = 0;
    }

    (void)MDS_IntUnlock();
}

#endif

#ifdef MDS_INCLUDETASKHOOK

/***********************************************************************************
��������: ��ʼ�����Ӻ���.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskHookInit(void)
{
    /* ��ʼ�����ӱ��� */
    gvfTaskCreateHook = (VFHCRT)NULL;
    gvfTaskSwitchHook = (VFHSWT)NULL;
    gvfTaskDeleteHook = (VFHDLT)NULL;
}

/***********************************************************************************
��������: ������񴴽����Ӻ���.
��ڲ���: vfFuncPointer: ��ӵĹ��Ӻ���.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskCreateHookAdd(VFHCRT vfFuncPointer)
{
    gvfTaskCreateHook = vfFuncPointer;
}

/***********************************************************************************
��������: ɾ�����񴴽����Ӻ���.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskCreateHookDel(void)
{
    gvfTaskCreateHook = (VFHCRT)NULL;
}

/***********************************************************************************
��������: ��������л����Ӻ���.
��ڲ���: vfFuncPointer: ��ӵĹ��Ӻ���.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskSwitchHookAdd(VFHSWT vfFuncPointer)
{
    gvfTaskSwitchHook = vfFuncPointer;
}

/***********************************************************************************
��������: ɾ�������л����Ӻ���.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskSwitchHookDel(void)
{
    gvfTaskSwitchHook = (VFHSWT)NULL;
}

/***********************************************************************************
��������: �������ɾ�����Ӻ���.
��ڲ���: vfFuncPointer: ��ӵĹ��Ӻ���.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskDeleteHookAdd(VFHDLT vfFuncPointer)
{
    gvfTaskDeleteHook = vfFuncPointer;
}

/***********************************************************************************
��������: ɾ������ɾ�����Ӻ���.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskDeleteHookDel(void)
{
    gvfTaskDeleteHook = (VFHDLT)NULL;
}

#endif

