
#include "mindows_inner.h"


/***********************************************************************************
��������: ��ʼ������ջ����, ����������������ǰ��ջ״̬.
��ڲ���: pstrTcb: ��Ҫ��ʼ��������TCBָ��.
          vfFuncPointer: �������������еĺ���.
          pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskStackInit(M_TCB* pstrTcb, VFUNC vfFuncPointer, void* pvPara)
{
    STACKREG* pstrRegSp;
    U32* puiStack;

    pstrRegSp = &pstrTcb->strStackReg;          /* �Ĵ������ַ */

    /* ��TCB�еļĴ������ʼ�� */
    pstrRegSp->uiR0 = (U32)pvPara;              /* R0 */
    pstrRegSp->uiR1 = 0;                        /* R1 */
    pstrRegSp->uiR2 = 0;                        /* R2 */
    pstrRegSp->uiR3 = 0;                        /* R3 */
    pstrRegSp->uiR4 = 0;                        /* R4 */
    pstrRegSp->uiR5 = 0;                        /* R5 */
    pstrRegSp->uiR6 = 0;                        /* R6 */
    pstrRegSp->uiR7 = 0;                        /* R7 */
    pstrRegSp->uiR8 = 0;                        /* R8 */
    pstrRegSp->uiR9 = 0;                        /* R9 */
    pstrRegSp->uiR10 = 0;                       /* R10 */
    pstrRegSp->uiR11 = 0;                       /* R11 */
    pstrRegSp->uiR12 = 0;                       /* R12 */
    pstrRegSp->uiR13 = (U32)pstrTcb - 32;       /* R13 */
    pstrRegSp->uiR14 = 0;                       /* R14 */
    pstrRegSp->uiR15 = (U32)vfFuncPointer;      /* R15 */
    pstrRegSp->uiXpsr = MODE_USR;               /* XPSR */
    pstrRegSp->uiExc_Rtn = RTN_THREAD_MSP;      /* EXC_RETURN */

    /* ���������ʼ����ʱ��ջ, ��ջ����������ʱ��Ӳ���Զ�ȡ�� */
    puiStack = (U32*)pstrTcb;
    *(--puiStack) = pstrRegSp->uiXpsr;
    *(--puiStack) = pstrRegSp->uiR15;
    *(--puiStack) = pstrRegSp->uiR14;
    *(--puiStack) = pstrRegSp->uiR12;
    *(--puiStack) = pstrRegSp->uiR3;
    *(--puiStack) = pstrRegSp->uiR2;
    *(--puiStack) = pstrRegSp->uiR1;
    *(--puiStack) = pstrRegSp->uiR0;
}

/***********************************************************************************
��������: ��ʼ�����ڲ���ϵͳ�������ʹ�õ�Ӳ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_SystemHardwareInit(void)
{
    /* ��ʼ��tick��ʱ�� */
    MDS_TickTimerInit();

    /* ��ʼ��PendSv�ж� */
    MDS_PendSvIsrInit();

    /* ����������ΪThread�û���ģʽ */
    MDS_SetChipWorkMode(UNPRIVILEGED);
}

/***********************************************************************************
��������: ��ʼ��tick��ʱ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TickTimerInit(void)
{
    /* ����Tick�ж϶�ʱ���� */
    (void)SysTick_Config(SystemCoreClock / (1000 / TICK));

    /* ����Tick�ж����ȼ�Ϊ6 */
    NVIC_SetPriority(SysTick_IRQn, 6 << 1);
}

/***********************************************************************************
��������: ��ʼ��PENDSV�ж�.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_PendSvIsrInit(void)
{
    /* ����PendSV�ж����ȼ�Ϊ7 */
    NVIC_SetPriority(PendSV_IRQn, 7 << 1);
}

/***********************************************************************************
��������: ���ô���������ģʽ.
��ڲ���: uiMode: PRIVILEGED: ��Ȩ��ģʽ.
                  UNPRIVILEGED: �û���ģʽ.
�� �� ֵ: none.
***********************************************************************************/
void MDS_SetChipWorkMode(U32 uiMode)
{
    U32 uiCtrl;

    /* ��ȡcontrol�Ĵ�����ֵ */
    uiCtrl = __get_CONTROL();

    /* �޸�control�Ĵ�����ֵ */
    uiCtrl &= 0xFFFFFFFE;
    uiCtrl |= uiMode;

    /* д��control�Ĵ�����ֵ */
    __set_CONTROL(uiCtrl);
}

/***********************************************************************************
��������: ����pendsv���жϵĺ���.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_IntPendSvSet(void)
{
#define HWREG(x)                (*((volatile unsigned long *)(x)))
#define NVIC_INT_CTRL           0xE000ED04  // Interrupt Control and State
#define NVIC_INT_CTRL_PEND_SV   0x10000000  // PendSV Set Pending

    /* ����PendSv�ж�, �ڸ��ж��е������� */
    HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;
}

/***********************************************************************************
��������: ͨ�����ô��������жϱ�־λ��ֹ�����жϲ���.
��ڲ���: none.
�� �� ֵ: RTN_SUCD: ���жϳɹ�.
          RTN_FAIL: ���ж�ʧ��.
***********************************************************************************/
U32 MDS_IntLock(void)
{
    __disable_irq();

    return RTN_SUCD;
}

/***********************************************************************************
��������: ʹ�ܴ��������жϱ�־λ, �����жϲ���.
��ڲ���: none.
�� �� ֵ: RTN_SUCD: �����жϳɹ�.
          RTN_FAIL: �����ж�ʧ��.
***********************************************************************************/
U32 MDS_IntUnlock(void)
{
    __enable_irq();

    return RTN_SUCD;
}