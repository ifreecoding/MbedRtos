
#include "mindows_inner.h"


/***********************************************************************************
��������: ����һ������.
��ڲ���: vfFuncPointer: ����������ʹ�ú�����ָ��.
          pvPara: ������ڲ���ָ��.
          pucTaskStack: ������ʹ��ջ�������ʼ��ַ.
          uiStackSize: ջ��С, ��λ: �ֽ�.
          ucTaskPrio: �������ȼ�.
�� �� ֵ: NULL: ��������ʧ��.
          ����: �����TCBָ��.
***********************************************************************************/
M_TCB* MDS_TaskCreate(VFUNC vfFuncPointer, void* pvPara, U8* pucTaskStack,
                      U32 uiStackSize, U8 ucTaskPrio)
{
    M_TCB* pstrTcb;

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
                              ucTaskPrio);

    return pstrTcb;
}

/***********************************************************************************
��������: ��ʼ������TCB, ����������������ǰTCB��״̬.
��ڲ���: vfFuncPionter: ����������ʹ�õĺ���ָ��.
          pvPara: ������ڲ���ָ��.
          pucTaskStack: ������ʹ�õ�ջ��ַ.
          uiStackSize: ջ��С, ��λ: �ֽ�.
          ucTaskPrio: �������ȼ�.
�� �� ֵ: NULL: ��������ʧ��.
          ����: ��������ɹ�, ����ֵΪ�����TCBָ��.
***********************************************************************************/
M_TCB* MDS_TaskTcbInit(VFUNC vfFuncPointer, void* pvPara, U8* pucTaskStack,
                       U32 uiStackSize, U8 ucTaskPrio)
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

    /* ��ʼ��ָ��TCB��ָ�� */
    pstrTcb->strTcbQue.pstrTcb = pstrTcb;

    /* ��ʼ���������ȼ� */
    pstrTcb->ucTaskPrio = ucTaskPrio;

    pstrList = &gstrReadyTab.astrList[ucTaskPrio];
    pstrNode = &pstrTcb->strTcbQue.strQueHead;
    pstrPrioFlag = &gstrReadyTab.strFlag;

    /* ���ж�, ��ֹ��������Ӱ�� */
    (void)MDS_IntLock();

    /* ����������ӵ�ready���� */
    MDS_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);

    /* �������������ж�, ����������� */
    (void)MDS_IntUnlock();

    return pstrTcb;
}

