
#include "wlx_core_c.h"


U32 guiCurTask;                         /* ��ǰ���е����� */

W_TCB* gpstrTask1Tcb;                   /* ����1��TCBָ�� */
W_TCB* gpstrTask2Tcb;                   /* ����2��TCBָ�� */


/***********************************************************************************
��������: ������������ǰ��״̬.
��ڲ���: vfFuncPointer: �������еĺ���ָ��.
          puiTaskStack: ����ʹ�õ�ջ��ַ.
�� �� ֵ: �����TCBָ��.
***********************************************************************************/
W_TCB* WLX_TaskInit(VFUNC vfFuncPointer, U32* puiTaskStack)
{
    W_TCB* pstrTcb;
    STACKREG* pstrStackReg;

    /* ���ڵݼ�ջ, TCB����ջ�� */
    pstrTcb = (W_TCB*)((U32)puiTaskStack - sizeof(W_TCB));

    /* �Ĵ������ַ */
    pstrStackReg = &pstrTcb->strStackReg;

    /* ��TCB�еļĴ������ʼ�� */
    pstrStackReg->uiR4 = 0;                     /* R4 */
    pstrStackReg->uiR5 = 0;                     /* R5 */
    pstrStackReg->uiR6 = 0;                     /* R6 */
    pstrStackReg->uiR7 = 0;                     /* R7 */
    pstrStackReg->uiR8 = 0;                     /* R8 */
    pstrStackReg->uiR9 = 0;                     /* R9 */
    pstrStackReg->uiR10 = 0;                    /* R10 */
    pstrStackReg->uiR11 = 0;                    /* R11 */
    pstrStackReg->uiR12 = 0;                    /* R12 */
    pstrStackReg->uiR13 = (U32)pstrTcb;         /* R13 */
    pstrStackReg->uiR14 = (U32)vfFuncPointer;   /* R14 */
    pstrStackReg->uiXpsr = MODE_USR;            /* XPSR */

    /* ���������TCBָ�� */
    return pstrTcb;
}

/***********************************************************************************
��������: ���øú��������������л�.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void WLX_TaskSwitch(void)
{
    STACKREG* pstrCurTaskStackRegAddr;
    STACKREG* pstrNextTaskStackRegAddr;

    if(1 == guiCurTask)
    {
        /* ��ǰ����Ĵ�����ĵ�ַ */
        pstrCurTaskStackRegAddr = &gpstrTask1Tcb->strStackReg;

        /* ������������Ĵ�����ĵ�ַ */
        pstrNextTaskStackRegAddr = &gpstrTask2Tcb->strStackReg;

        /* �����´����е����� */
        guiCurTask = 2;
    }
    else //if(2 == guiCurTask)
    {
        pstrCurTaskStackRegAddr = &gpstrTask2Tcb->strStackReg;
        pstrNextTaskStackRegAddr = &gpstrTask1Tcb->strStackReg;

        guiCurTask = 1;
    }

    /* �л����� */
    WLX_ContextSwitch(pstrCurTaskStackRegAddr, pstrNextTaskStackRegAddr);
}

/***********************************************************************************
��������: ��ʼ�����������.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void WLX_TaskStart(void)
{
    STACKREG* pstrNextTaskStackRegAddr;

    /* ������������Ĵ�����ĵ�ַ */
    pstrNextTaskStackRegAddr = &gpstrTask1Tcb->strStackReg;

    /* �����´ε��ȵ����� */
    guiCurTask = 1;

    /* �л�������״̬ */
    WLX_SwitchToTask(pstrNextTaskStackRegAddr);
}

