
#include "mindows_inner.h"


U32 guiSystemStatus;                    /* ϵͳ״̬ */

M_TASKSCHEDTAB gstrReadyTab;            /* ��������� */
M_DLIST gstrDelayTab;                   /* �����ӳٱ� */

U32 guiTick;                            /* ����ϵͳ��tick���� */
U8 gucTickSched;                        /* tick���ȱ�־ */

STACKREG* gpstrCurTaskReg;              /* ��ǰ���е�����ļĴ�����ָ��, ���ڴ�C����
                                           ����� */
STACKREG* gpstrNextTaskReg;             /* ��Ҫ���е�����ļĴ�����ָ��, ���ڴ�C����
                                           ����� */

M_TCB* gpstrCurTcb;                     /* ��ǰ���е������TCBָ�� */
M_TCB* gpstrRootTaskTcb;                /* root����TCBָ�� */
M_TCB* gpstrIdleTaskTcb;                /* idle����TCBָ�� */

U32 guiUser;                            /* ����ϵͳ��ʹ�õ��û� */

const U8 caucTaskPrioUnmapTab[256] =    /* ���ȼ�������ұ� */
{
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};


/***********************************************************************************
��������: ������, C����Ӵ˺�����ʼ����. �ú�������ϵͳ���񲢴ӷ�����״̬�л���root
          ��������.
��ڲ���: none.
�� �� ֵ: ����ֵΪ0, ��ϵͳ�������е��ú�������.
***********************************************************************************/
S32 main(void)
{
    /**************************************************************************/
    /*            �������������    www.ifreecoding.com                       */
    /*                              bbs.ifreecoding.com                       */
    /*                              blog.sina.com.cn/ifreecoding              */
    /**************************************************************************/

    /* ��ʼ��ϵͳ����, ��������ϵͳ��������Ҫ�Ļ��� */
    MDS_SystemVarInit();

    /* ��ʼ�������, ��ǰ������ʼִ�� */
    MDS_TaskStart(gpstrRootTaskTcb);

    return 0;
}

/***********************************************************************************
��������: ��ʼ��ϵͳ����, ��������ϵͳ��������Ҫ�Ļ���.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_SystemVarInit(void)
{
    /* ��ʼ��ΪROOT�û�Ȩ�� */
    MDS_SetUser(USERROOT);

    /* ��ʼ��Ϊδ�������ϵͳ״̬ */
    guiSystemStatus = SYSTEMNOTSCHEDULE;

    /* ����ǰ��������, root�����idle�����TCB��ʼ��ΪNULL */
    gpstrCurTcb = (M_TCB*)NULL;
    gpstrRootTaskTcb = (M_TCB*)NULL;
    gpstrIdleTaskTcb = (M_TCB*)NULL;

    /* ��ʼ��tick������0��ʼ */
    guiTick = 0;

    /* ��ʼ��Ϊ��tick�жϵ���״̬ */
    gucTickSched = TICKSCHEDCLR;

    /* ��ʼ�����жϼ���Ϊ0 */
    guiIntLockCounter = 0;

#ifdef MDS_INCLUDETASKHOOK

    /* ��ʼ�����Ӻ��� */
    MDS_TaskHookInit();

#endif

#ifdef MDS_TASKROUNDROBIN

    /* Ĭ�����񲻷�����ת���� */
    MDS_TaskTimeSlice(0);

#endif

    /* ��ʼ������ready�� */
    MDS_TaskSchedTabInit(&gstrReadyTab);

    /* ��ʼ������delay�� */
    MDS_DlistInit(&gstrDelayTab);

    /* ����ǰ������ */
    gpstrRootTaskTcb = MDS_TaskCreate(ROOTTASKNAME, MDS_BeforeRootTask, NULL, NULL,
                                      ROOTTASKSTACK, USERHIGHESTPRIO, NULL);

    /* ������������ */
    gpstrIdleTaskTcb = MDS_TaskCreate(IDLETASKNAME, MDS_IdleTask, NULL, NULL,
                                      IDLETASKSTACK, LOWESTPRIO, NULL);
}

/***********************************************************************************
��������: ǰ������, �ڸ���������ǰ����, �����ڸ���������ǰ��ʼ������ϵͳ���ڵ���ʹ��
          ��Ӳ��, Ȼ���ٵ��ø���������û�����.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void MDS_BeforeRootTask(void* pvPara)
{
    /* ��ʼ�����ڲ���ϵͳ�������ʹ�õ�Ӳ�� */
    MDS_SystemHardwareInit();

    /* �ѽ������ϵͳ״̬ */
    guiSystemStatus = SYSTEMSCHEDULE;

    /* �������뵽�û�����, ����ΪGUEST�û�Ȩ�� */
    MDS_SetUser(USERGUEST);

    /* ���ø�����, �����û����� */
    MDS_RootTask();
}

/***********************************************************************************
��������: tick�жϵ���������. tick�жϻ�ִ�иú���, �ɸú�������PendSv�жϽ�������
          ����.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskTick(void)
{
    /* ÿ��tick�ж�tick������1 */
    guiTick++;

    /* ��tick�жϴ����ĵ���, ��Ϊtick�жϵ���״̬ */
    gucTickSched = TICKSCHEDSET;

#ifdef MDS_TASKROUNDROBIN

    /* ������õ���ת��������ֵ��Ϊ0, �����ӵ�ǰ�������ȼ�����ת���ȼ���ֵ */
    if((0 != guiTimeSlice) && (NULL != gpstrCurTcb))
    {
        gauiSliceCnt[gpstrCurTcb->ucTaskPrio]++;
    }

#endif

    /* ����PendSv�ж�, �ڸ��ж��е������� */
    MDS_IntPendSvSet();
}

/***********************************************************************************
��������: Ϊ�����������л���׼��, ���������л������л�ຯ����ʹ�õı���.
��ڲ���: pstrTcb: �������е������TCBָ��.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskSwitch(M_TCB* pstrTcb)
{
    /* ���ڵ�ǰ���� */
    if(NULL != gpstrCurTcb)
    {
        /* ��ǰ����ļĴ������ַ, �������ͨ������������ݼĴ��� */
        gpstrCurTaskReg = &gpstrCurTcb->strStackReg;
    }
    else /* ��ǰ����ɾ�� */
    {
        gpstrCurTaskReg = NULL;
    }

    /* ������������ļĴ������ַ, �������ͨ����������ָ��Ĵ��� */
    gpstrNextTaskReg = &pstrTcb->strStackReg;

    /* �������������TCBָ�� */
    gpstrCurTcb = pstrTcb;
}

/***********************************************************************************
��������: ʵ�ִӷǲ���ϵͳ״̬�л�������ϵͳ״̬, ִ�е�һ���������.
��ڲ���: pstrTcb: �������е������TCBָ��.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskStart(M_TCB* pstrTcb)
{
    /* ������������ļĴ������ַ, �������ͨ����������ָ��Ĵ��� */
    gpstrNextTaskReg = &pstrTcb->strStackReg;

    /* �������������TCBָ�� */
    gpstrCurTcb = pstrTcb;

    /* �л�������ϵͳ״̬ */
    MDS_SwitchToTask();
}

/***********************************************************************************
��������: ��ʼ��������ȱ�.
��ڲ���: pstrSchedTab: ������ȱ�ָ��.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskSchedTabInit(M_TASKSCHEDTAB* pstrSchedTab)
{
    U32 i;

    /* ��ʼ��ÿ�����ȼ������ready�� */
    for(i = 0; i < PRIORITYNUM; i++)
    {
        MDS_DlistInit(&pstrSchedTab->astrList[i]);
    }

    /* ��ʼ��ready�����ȼ���־ */
#if PRIORITYNUM >= PRIORITY128

    for(i = 0; i < PRIOFLAGGRP1; i++)
    {
        pstrSchedTab->strFlag.aucPrioFlagGrp1[i] = 0;
    }

    for(i = 0; i < PRIOFLAGGRP2; i++)
    {
        pstrSchedTab->strFlag.aucPrioFlagGrp2[i] = 0;
    }

    pstrSchedTab->strFlag.ucPrioFlagGrp3 = 0;

#elif PRIORITYNUM >= PRIORITY16

    for(i = 0; i< PRIOFLAGGRP1; i++)
    {
        pstrSchedTab->strFlag.aucPrioFlagGrp1[i] = 0;
    }

    pstrSchedTab->strFlag.ucPrioFlagGrp2 = 0;

#else

    pstrSchedTab->strFlag.ucPrioFlagGrp1 = 0;

#endif
}

/***********************************************************************************
��������: �����������ȼ���ӵ�������ȱ�.
��ڲ���: pstrList: ���ȱ�ָ��, ��������ӵ��õ��ȱ�.
          pstrNode: ��Ҫ��ӵ����ȱ������ڵ�ָ��.
          pstrPrioFlag: �õ��ȱ��Ӧ�����ȼ���־��ָ��.
          ucTaskPrio: ��Ҫ��ӵ���������ȼ�.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskAddToSchedTab(M_DLIST* pstrList, M_DLIST* pstrNode,
                           M_PRIOFLAG* pstrPrioFlag, U8 ucTaskPrio)
{
    /* ��������ڵ���ӵ����ȱ��� */
    MDS_DlistNodeAdd(pstrList, pstrNode);

    /* ���ø������Ӧ���ȱ�����ȼ���־�� */
    MDS_TaskSetPrioFlag(pstrPrioFlag, ucTaskPrio);
}

/***********************************************************************************
��������: ��������ӵ�delay��, ���������ӳ�ʱ����ٵ�����ӵ�delay����.
��ڲ���: pstrNode: ��Ҫ��ӵ�����ڵ�ָ��.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskAddToDelayTab(M_DLIST* pstrNode)
{
    M_DLIST* pstrTempNode;
    M_DLIST* pstrNextNode;
    M_TCBQUE* pstrTcbQue;
    U32 uiStillTick;
    U32 uiTempStillTick;

    /* ��ȡdelay���еĵ�һ������ڵ�ָ�� */
    pstrTempNode = MDS_DlistEmpInq(&gstrDelayTab);

    /* delay��ǿ� */
    if(NULL != pstrTempNode)
    {
        /* ��ȡҪ����delay���������ӳ�ʱ�� */
        pstrTcbQue = (M_TCBQUE*)pstrNode;
        uiStillTick = pstrTcbQue->pstrTcb->uiStillTick;

        /* Ѱ��Ҫ����delay���������delay���е�λ�� */
        while(1)
        {
            /* ��ȡdelay����������ӳ�ʱ�� */
            pstrTcbQue = (M_TCBQUE*)pstrTempNode;
            uiTempStillTick = pstrTcbQue->pstrTcb->uiStillTick;

            /* �����¸��������ж�Ҫ����delay�������Ӧ�ò����λ�� */
            if(uiStillTick < uiTempStillTick)
            {
                if(uiStillTick > guiTick)
                {
                    MDS_DlistCurNodeInsert(&gstrDelayTab, pstrTempNode, pstrNode);

                    return;
                }
                else if(uiStillTick < guiTick)
                {
                    if(guiTick > uiTempStillTick)
                    {
                        MDS_DlistCurNodeInsert(&gstrDelayTab, pstrTempNode,
                                               pstrNode);

                        return;
                    }
                }
            }
            else if(uiStillTick > uiTempStillTick)
            {
                if(uiStillTick > guiTick)
                {
                    if(guiTick > uiTempStillTick)
                    {
                        MDS_DlistCurNodeInsert(&gstrDelayTab, pstrTempNode,
                                               pstrNode);

                        return;
                    }
                }
            }

            /* delay����ȡ���������ӳ�ʱ��С����Ҫ����delay��������ӳ�ʱ��, ȡ����
               ������ڵ�����ж� */
            pstrNextNode = MDS_DlistNextNodeEmpInq(&gstrDelayTab, pstrTempNode);

            /* ������delay������һ���ڵ�, ����Ҫ����delay���������뵽��β */
            if(NULL == pstrNextNode)
            {
                MDS_DlistNodeAdd(&gstrDelayTab, pstrNode);

                return;
            }
            else /* ����delay���¸�����ڵ�����ж� */
            {
                pstrTempNode = pstrNextNode;
            }
        }
    }
    else /* delay��Ϊ����ֱ�ӽ���Ҫ����delay���������뵽delay��β */
    {
        MDS_DlistNodeAdd(&gstrDelayTab, pstrNode);

        return;
    }
}

/***********************************************************************************
��������: ��������ȱ�ɾ������.
��ڲ���: pstrList: ���ȱ�ָ��, �Ӹ�����ɾ������.
          pstrPrioFlag: ���ȱ��Ӧ�����ȼ���־��ָ��.
          ucTaskPrio: Ҫɾ����������ȼ�.
�� �� ֵ: ��ɾ��������Ľڵ�ָ��.
***********************************************************************************/
M_DLIST* MDS_TaskDelFromSchedTab(M_DLIST* pstrList, M_PRIOFLAG* pstrPrioFlag,
                                 U8 ucTaskPrio)
{
    M_DLIST* pstrDelNode;

    /* ��������ڵ�ӵ��ȱ���ɾ�� */
    pstrDelNode = MDS_DlistNodeDelete(pstrList);

    /* ������ȱ��и����ȼ�Ϊ����������ȼ���־ */
    if(NULL == MDS_DlistEmpInq(pstrList))
    {
        MDS_TaskClrPrioFlag(pstrPrioFlag, ucTaskPrio);
    }

    /* ���ر�ɾ������Ľڵ�ָ�� */
    return pstrDelNode;
}

/***********************************************************************************
��������: ��������ӵ�sem���ȱ�.
��ڲ���: pstrTcb: �����TCB.
          pstrSem: �������ź���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskAddToSemTab(M_TCB* pstrTcb, M_SEM* pstrSem)
{
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_PRIOFLAG* pstrPrioFlag;
    U8 ucTaskPrio;

    /* �ź����ǲ������ȼ������㷨 */
    if(SEMPRIO == (SEMSCHEDOPTMASK & pstrSem->uiSemOpt))
    {
        /* ��ȡ�������ز��� */
        ucTaskPrio = pstrTcb->ucTaskPrio;
        pstrList = &pstrSem->strSemTab.astrList[ucTaskPrio];
        pstrNode = &pstrTcb->strSemQue.strQueHead;
        pstrPrioFlag = &pstrSem->strSemTab.strFlag;

        /* ��ӵ�sem���ȱ� */
        MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);
    }
    else /* �ź��������Ƚ��ȳ������㷨 */
    {
        /* ��ȡ�������ز���, ʹ��0���ȼ�������Ϊ�Ƚ��ֳ����� */
        pstrList = &pstrSem->strSemTab.astrList[LOWESTPRIO];
        pstrNode = &pstrTcb->strSemQue.strQueHead;

        /* ��ӵ�sem���ȱ� */
        MDS_DlistNodeAdd(pstrList, pstrNode);
    }

    /* ��������������ź��� */
    pstrTcb->pstrSem = pstrSem;
}

/***********************************************************************************
��������: �������sem���ȱ�ɾ��.
��ڲ���: pstrTcb: �����TCB.
�� �� ֵ: ��ɾ������Ľڵ�ָ��.
***********************************************************************************/
M_DLIST* MDS_TaskDelFromSemTab(M_TCB* pstrTcb)
{
    M_SEM* pstrSem;
    M_DLIST* pstrList;
    M_PRIOFLAG* pstrPrioFlag;
    U8 ucTaskPrio;

    /* ��ȡ����������ź��� */
    pstrSem = pstrTcb->pstrSem;

    /* �ź����ǲ������ȼ������㷨 */
    if(SEMPRIO == (SEMSCHEDOPTMASK & pstrSem->uiSemOpt))
    {
        /* ��ȡ�������ز��� */
        ucTaskPrio = pstrTcb->ucTaskPrio;
        pstrList = &pstrSem->strSemTab.astrList[ucTaskPrio];
        pstrPrioFlag = &pstrSem->strSemTab.strFlag;

        /* ��sem���ȱ�ɾ�������� */
        return MDS_TaskDelFromSchedTab(pstrList, pstrPrioFlag, ucTaskPrio);
    }
    else /* �ź��������Ƚ��ȳ������㷨 */
    {
        /* ��ȡ�������ز���, ʹ��0���ȼ�������Ϊ�Ƚ��ֳ����� */
        pstrList = &pstrSem->strSemTab.astrList[LOWESTPRIO];

        /* ��sem���ȱ�ɾ�������� */
        return MDS_DlistNodeDelete(pstrList);
    }
}

/***********************************************************************************
��������: �ڱ��ź��������������л�ȡһ����Ҫ�������ͷŵ�����.
��ڲ���: pstrSem: �ź���ָ��.
�� �� ֵ: ��Ҫ�ͷŵ������TCBָ��, ��û����Ҫ�ͷŵ������򷵻�NULL.
***********************************************************************************/
M_TCB* MDS_SemGetActiveTask(M_SEM* pstrSem)
{
    M_DLIST* pstrNode;
    M_TCBQUE* pstrTaskQue;
    U8 ucTaskPrio;

    /* �ź����������ȼ������㷨 */
    if(SEMPRIO == (SEMSCHEDOPTMASK & pstrSem->uiSemOpt))
    {
        /* ��ȡ�ź����������ȼ���ߵ�����TCB */
        ucTaskPrio = MDS_TaskGetHighestPrio(&pstrSem->strSemTab.strFlag);
    }
    else /* �ź��������Ƚ��ȳ������㷨 */
    {
        /* ����0���ȼ������� */
        ucTaskPrio = LOWESTPRIO;
    }

    /* ��ѯsem���Ƿ�Ϊ�� */
    pstrNode = MDS_DlistEmpInq(&pstrSem->strSemTab.astrList[ucTaskPrio]);

    /* �ź�����û�б�����������, ����Ҫ�ͷ����� */
    if(NULL == pstrNode)
    {
        return (M_TCB*)NULL;
    }
    else /* �ź������б�����������, ������Ҫ���ͷŵ������TCBָ�� */
    {
        pstrTaskQue = (M_TCBQUE*)pstrNode;

        return pstrTaskQue->pstrTcb;
    }
}

/***********************************************************************************
��������: ������Ⱥ���, �ڴ˺���ʵ���������״̬ת���ĵ���.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskSched(void)
{
    M_TCB* pstrTcb;

    /* ��tick�жϴ����ĵ��� */
    if(TICKSCHEDSET == gucTickSched)
    {
        /* ��tick����״̬ */
        gucTickSched = TICKSCHEDCLR;

        /* ����delay������ */
        MDS_TaskDelayTabSched();
    }

    /* ����ready������ */
    pstrTcb = MDS_TaskReadyTabSched();

#ifdef MDS_INCLUDETASKHOOK

    /* ��������л������Ѿ��ҽӺ�����ִ�иú��� */
    if((VFHSWT)NULL != gvfTaskSwitchHook)
    {
        /* ִ�������л����Ӻ��� */
        gvfTaskSwitchHook(gpstrCurTcb, pstrTcb);
    }

#endif

    /* �����л� */
    MDS_TaskSwitch(pstrTcb);
}

/***********************************************************************************
��������: ��ready����е���.
��ڲ���: none.
�� �� ֵ: �������е������TCBָ��.
***********************************************************************************/
M_TCB* MDS_TaskReadyTabSched(void)
{
    M_TCB* pstrTcb;
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
#ifdef MDS_TASKROUNDROBIN
    M_DLIST* pstrNextNode;
#endif
    M_TCBQUE* pstrTaskQue;
    U8 ucTaskPrio;

    /* ��ȡready�������ȼ���ߵ������TCB */
    ucTaskPrio = MDS_TaskGetHighestPrio(&gstrReadyTab.strFlag);
    pstrList = &gstrReadyTab.astrList[ucTaskPrio];
    pstrNode = MDS_DlistEmpInq(pstrList);
    pstrTaskQue = (M_TCBQUE*)pstrNode;
    pstrTcb = pstrTaskQue->pstrTcb;

#ifdef MDS_TASKROUNDROBIN

    /* ��Ҫִ��������ת���� */
    if(0 != guiTimeSlice)
    {
        /* �¸�����������Ϊ��ǰ���� */
        if(gpstrCurTcb == pstrTcb)
        {
            /* ͬһ�����Ѿ��ﵽ��ת���ȵ�����ֵ */
            if(gauiSliceCnt[gpstrCurTcb->ucTaskPrio] >= guiTimeSlice)
            {
                /* ����ǰ�������ȼ�����ת���ȼ���ֵ��0 */
                gauiSliceCnt[gpstrCurTcb->ucTaskPrio] = 0;

                /* ȡ���뵱ǰ����ͬ�����ȼ������� */
                pstrNextNode = MDS_DlistNextNodeEmpInq(pstrList, pstrNode);

                /* ����, ����Ҫ����������ת���� */
                if(NULL != pstrNextNode)
                {
                    /* ����ǰ����ŵ�ready���е�ǰ������ȼ������������� */
                    /* ������ɾ����ǰ����ڵ� */
                    (void)MDS_DlistNodeDelete(pstrList);

                    /* ����ǰ����ڵ��������β */
                    MDS_DlistNodeAdd(pstrList, pstrNode);

                    /* �����¸����� */
                    pstrTaskQue = (M_TCBQUE*)pstrNextNode;
                    pstrTcb = pstrTaskQue->pstrTcb;
                }
            }
        }
    }

#endif

    return pstrTcb;
}

/***********************************************************************************
��������: ��delay����е���.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskDelayTabSched(void)
{
    M_TCB* pstrTcb;
    M_DLIST* pstrList;
    M_DLIST* pstrNode;
    M_DLIST* pstrDelayNode;
    M_DLIST* pstrNextNode;
    M_PRIOFLAG* pstrPrioFlag;
    M_TCBQUE* pstrTcbQue;
    U32 uiTick;
    U8 ucTaskPrio;

    /* ��ȡdelay���е�����ڵ� */
    pstrDelayNode = MDS_DlistEmpInq(&gstrDelayTab);

    /* delay����������, ����delay���е����� */
    if(NULL != pstrDelayNode)
    {
        /* �ж�delay����������ӳ�ʱ���Ƿ���� */
        while(1)
        {
            /* ��ȡdelay����������ӳ�ʱ�� */
            pstrTcbQue = (M_TCBQUE*)pstrDelayNode;
            pstrTcb = pstrTcbQue->pstrTcb;
            uiTick = pstrTcb->uiStillTick;

            /* �������ӳ�ʱ�����, ��delay����ɾ�������뵽���ȱ��� */
            if(uiTick == guiTick)
            {
                /* ��delay���������� */
                pstrNextNode = MDS_DlistCurNodeDelete(&gstrDelayTab, pstrDelayNode);

                /* ��������delay���־ */
                pstrTcb->uiTaskFlag &= (~((U32)DELAYQUEFLAG));

                /* ���¸������delay��pend��ز��� */
                /* �������ӵ��delay״̬���delay״̬�ָ� */
                if(TASKDELAY == (TASKDELAY & pstrTcb->strTaskOpt.ucTaskSta))
                {
                    /* ��������delay״̬ */
                    pstrTcb->strTaskOpt.ucTaskSta &= ~((U8)TASKDELAY);

                    /* ����uiDelayTick��������delay����ķ���ֵ */
                    pstrTcb->strTaskOpt.uiDelayTick = RTN_TKDLTO;
                }
                /* �������ӵ��pend״̬���pend״̬�ָ� */
                else if(TASKPEND == (TASKPEND & pstrTcb->strTaskOpt.ucTaskSta))
                {
                    /* ��sem���ȱ������� */
                    (void)MDS_TaskDelFromSemTab(pstrTcb);

                    /* ��������pend״̬ */
                    pstrTcb->strTaskOpt.ucTaskSta &= ~((U8)TASKPEND);

                    /* ����uiDelayTick��������pend����ķ���ֵ */
                    pstrTcb->strTaskOpt.uiDelayTick = RTN_SMTKTO;
                }

                /* ��ȡ���������ز��� */
                pstrNode = &pstrTcb->strTcbQue.strQueHead;
                ucTaskPrio = pstrTcb->ucTaskPrio;
                pstrList = &gstrReadyTab.astrList[ucTaskPrio];
                pstrPrioFlag = &gstrReadyTab.strFlag;

                /* ����������ӵ�ready���� */
                MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);

                /* ���������ready״̬ */
                pstrTcb->strTaskOpt.ucTaskSta |= TASKREADY;

                /* delay���Ѿ��������, ������delay��ĵ��� */
                if(NULL == pstrNextNode)
                {
                    break;
                }
                else /* delay��û������, �����¸��ڵ�����ж� */
                {
                    pstrDelayNode = pstrNextNode;
                }
            }
            else /* ����������ӳ�ʱ��û�н���, ������delay��ĵ��� */
            {
                break;
            }
        }
    }
}

/***********************************************************************************
��������: ��������ӵ�������ȱ��Ӧ�����ȼ���־����.
��ڲ���: pstrPrioFlag: ���ȱ��Ӧ�����ȼ���־��ָ��.
          ucTaskPrio: ��������ȼ�.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskSetPrioFlag(M_PRIOFLAG* pstrPrioFlag, U8 ucTaskPrio)
{
#if PRIORITYNUM >= PRIORITY128
    U8 ucPrioFlagGrp1;
    U8 ucPrioFlagGrp2;
    U8 ucPosInGrp1;
    U8 ucPosInGrp2;
    U8 ucPosInGrp3;
#elif PRIORITYNUM >= PRIORITY16
    U8 ucPrioFlagGrp1;
    U8 ucPosInGrp1;
    U8 ucPosInGrp2;
#endif

    /* ���õ��ȱ��Ӧ�����ȼ���־�� */
#if PRIORITYNUM >= PRIORITY128

    /* ��ȡ���ȼ���־�ڵ�һ��͵ڶ����е���� */
    ucPrioFlagGrp1 = ucTaskPrio / 8;
    ucPrioFlagGrp2 = ucPrioFlagGrp1 / 8;

    /* ��ȡ���ȼ���־��ÿһ���е�λ�� */
    ucPosInGrp1 = ucTaskPrio % 8;
    ucPosInGrp2 = ucPrioFlagGrp1 % 8;
    ucPosInGrp3 = ucPrioFlagGrp2;

    /* ��ÿһ�����������ȼ���־ */
    pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1] |= (U8)(1 << ucPosInGrp1);
    pstrPrioFlag->aucPrioFlagGrp2[ucPrioFlagGrp2] |= (U8)(1 << ucPosInGrp2);
    pstrPrioFlag->ucPrioFlagGrp3 |= (U8)(1 << ucPosInGrp3);

#elif PRIORITYNUM >= PRIORITY16

    ucPrioFlagGrp1 = ucTaskPrio / 8;

    ucPosInGrp1 = ucTaskPrio % 8;
    ucPosInGrp2 = ucPrioFlagGrp1;

    pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1] |= (U8)(1 << ucPosInGrp1);
    pstrPrioFlag->ucPrioFlagGrp2 |= (U8)(1 << ucPosInGrp2);

#else

    pstrPrioFlag->ucPrioFlagGrp1 |= (U8)(1 << ucTaskPrio);

#endif
}

/***********************************************************************************
��������: ���������������ȱ��Ӧ�����ȼ���־�������.
��ڲ���: pstrPrioFlag: ���ȱ��Ӧ�����ȼ���־��ָ��.
          ucTaskPrio: ��������ȼ�.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskClrPrioFlag(M_PRIOFLAG* pstrPrioFlag, U8 ucTaskPrio)
{
#if PRIORITYNUM >= PRIORITY128
    U8 ucPrioFlagGrp1;
    U8 ucPrioFlagGrp2;
    U8 ucPosInGrp1;
    U8 ucPosInGrp2;
    U8 ucPosInGrp3;
#elif PRIORITYNUM >= PRIORITY16
    U8 ucPrioFlagGrp1;
    U8 ucPosInGrp1;
    U8 ucPosInGrp2;
#endif

    /* ������ȱ��Ӧ�����ȼ���־�� */
#if PRIORITYNUM >= PRIORITY128

    /* ��ȡ���ȼ���־�ڵ�һ��͵ڶ����е���� */
    ucPrioFlagGrp1 = ucTaskPrio / 8;
    ucPrioFlagGrp2 = ucPrioFlagGrp1 / 8;

    /* ��ȡ���ȼ���־��ÿһ���е�λ�� */
    ucPosInGrp1 = ucTaskPrio % 8;
    ucPosInGrp2 = ucPrioFlagGrp1 % 8;
    ucPosInGrp3 = ucPrioFlagGrp2;

    /* ��ÿһ����������ȼ���־ */
    pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1] &= ~((U8)(1 << ucPosInGrp1));
    if(0 == pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1])
    {
        pstrPrioFlag->aucPrioFlagGrp2[ucPrioFlagGrp2] &= ~((U8)(1 << ucPosInGrp2));
        if(0 == pstrPrioFlag->aucPrioFlagGrp2[ucPrioFlagGrp2])
        {
            pstrPrioFlag->ucPrioFlagGrp3 &= ~((U8)(1 << ucPosInGrp3));
        }
    }

#elif PRIORITYNUM >= PRIORITY16

    ucPrioFlagGrp1 = ucTaskPrio / 8;

    ucPosInGrp1 = ucTaskPrio % 8;
    ucPosInGrp2 = ucPrioFlagGrp1;

    pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1] &= ~((U8)(1 << ucPosInGrp1));
    if(0 == pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1])
    {
        pstrPrioFlag->ucPrioFlagGrp2 &= ~((U8)(1 << ucPosInGrp2));
    }

#else

    pstrPrioFlag->ucPrioFlagGrp1 &= ~((U8)(1 << ucTaskPrio));

#endif
}

/***********************************************************************************
��������: ��ȡ������ȱ��������������ȼ�.
��ڲ���: pstrPrioFlag: ���ȱ�����ȼ���־��ָ��.
�� �� ֵ: ������ȱ��е�������ȼ�.
***********************************************************************************/
U8 MDS_TaskGetHighestPrio(M_PRIOFLAG* pstrPrioFlag)
{
#if PRIORITYNUM >= PRIORITY128
    U8 ucPrioFlagGrp1;
    U8 ucPrioFlagGrp2;
    U8 ucHighestFlagInGrp1;
#elif PRIORITYNUM >= PRIORITY16
    U8 ucPrioFlagGrp1;
    U8 ucHighestFlagInGrp1;
#endif

    /* ��ȡ������ȱ��е�������ȼ� */
#if PRIORITYNUM >= PRIORITY128

    ucPrioFlagGrp2 = caucTaskPrioUnmapTab[pstrPrioFlag->ucPrioFlagGrp3];

    ucPrioFlagGrp1 =
                caucTaskPrioUnmapTab[pstrPrioFlag->aucPrioFlagGrp2[ucPrioFlagGrp2]];

    ucHighestFlagInGrp1 = caucTaskPrioUnmapTab[pstrPrioFlag->aucPrioFlagGrp1
                                             [ucPrioFlagGrp2 * 8 + ucPrioFlagGrp1]];

    return (U8)((ucPrioFlagGrp2 * 8 + ucPrioFlagGrp1) * 8 + ucHighestFlagInGrp1);

#elif PRIORITYNUM >= PRIORITY16

    ucPrioFlagGrp1 = caucTaskPrioUnmapTab[pstrPrioFlag->ucPrioFlagGrp2];

    ucHighestFlagInGrp1 =
                caucTaskPrioUnmapTab[pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1]];

    return (U8)(ucPrioFlagGrp1 * 8 + ucHighestFlagInGrp1);

#else

    return caucTaskPrioUnmapTab[pstrPrioFlag->ucPrioFlagGrp1];

#endif
}

/***********************************************************************************
��������: ��ȡϵͳ��ǰ��tick.
��ڲ���: none.
�� �� ֵ: ϵͳ��ǰ��tick.
***********************************************************************************/
U32 MDS_GetSystemTick(void)
{
    return guiTick;
}

/***********************************************************************************
��������: ��ȡϵͳ��ǰ�����TCBָ��.
��ڲ���: none.
�� �� ֵ: ϵͳ��ǰ��TCBָ��.
***********************************************************************************/
M_TCB* MDS_GetCurrentTcb(void)
{
    return gpstrCurTcb;
}

/***********************************************************************************
��������: ��ȡ�������TCBָ��.
��ڲ���: none.
�� �� ֵ: �������TCBָ��.
***********************************************************************************/
M_TCB* MDS_GetRootTcb(void)
{
    return gpstrRootTaskTcb;
}

/***********************************************************************************
��������: ��ȡ���������TCBָ��.
��ڲ���: none.
�� �� ֵ: ���������TCBָ��.
***********************************************************************************/
M_TCB* MDS_GetIdleTcb(void)
{
    return gpstrIdleTaskTcb;
}

/***********************************************************************************
��������: �����û�.
��ڲ���: uiUser: ��Ҫ���õ��û�.
                  USERROOT: ROOT�û�, ��������Ȩ��.
                  USERGUEST: GUEST�û�, ���в���Ȩ��.
�� �� ֵ: none.
***********************************************************************************/
void MDS_SetUser(U32 uiUser)
{
    guiUser = uiUser;
}

/***********************************************************************************
��������: ��ȡ�û�.
��ڲ���: none.
�� �� ֵ: ��ȡ�����û�.
          USERROOT: ROOT�û�, ��������Ȩ��.
          USERGUEST: GUEST�û�, ���в���Ȩ��.
***********************************************************************************/
U32 MDS_GetUser(void)
{
    return guiUser;
}

