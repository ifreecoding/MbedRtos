
#include "mindows_inner.h"


U8 gaucRootTaskStack[ROOTTASKSTACK];    /* �������ջ */


M_TASKSCHEDTAB gstrReadyTab;            /* ��������� */


STACKREG* gpstrCurTaskReg;              /* ��ǰ���е�����ļĴ�����ָ��, ���ڴ�C����
                                           ����� */
STACKREG* gpstrNextTaskReg;             /* ��Ҫ���е�����ļĴ�����ָ��, ���ڴ�C����
                                           ����� */

M_TCB* gpstrCurTcb;                     /* ��ǰ���е������TCBָ�� */
M_TCB* gpstrRootTaskTcb;                /* root����TCBָ�� */

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

    /* ����ǰ��������, root�����TCB��ʼ��ΪNULL */
    gpstrCurTcb = (M_TCB*)NULL;
    gpstrRootTaskTcb = (M_TCB*)NULL;

    /* ��ʼ������ready�� */
    MDS_TaskSchedTabInit(&gstrReadyTab);

    /* ����ǰ������ */
    gpstrRootTaskTcb = MDS_TaskCreate(MDS_BeforeRootTask, NULL, gaucRootTaskStack,
                       ROOTTASKSTACK, 2);
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
    /* ��ǰ����ļĴ������ַ, �������ͨ������������ݼĴ��� */
    gpstrCurTaskReg = &gpstrCurTcb->strStackReg;

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
��������: ������Ⱥ���, �ڴ˺���ʵ���������״̬ת���ĵ���.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskSched(void)
{
    M_TCB* pstrTcb;

    /* ����ready������ */
    pstrTcb = MDS_TaskReadyTabSched();

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
    M_TCBQUE* pstrTaskQue;
    U8 ucTaskPrio;

    /* ��ȡready�������ȼ���ߵ������TCB */
    ucTaskPrio = MDS_TaskGetHighestPrio(&gstrReadyTab.strFlag);
    pstrTaskQue = (M_TCBQUE*)MDS_DlistEmpInq(&gstrReadyTab.astrList[ucTaskPrio]);
    pstrTcb = pstrTaskQue->pstrTcb;

    return pstrTcb;
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

