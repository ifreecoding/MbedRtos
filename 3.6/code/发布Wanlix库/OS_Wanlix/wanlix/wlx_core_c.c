
#include "wlx_core_c.h"


W_TCB* gpstrCurTcb;                     /* ��ǰ���е������TCB */

W_TCB* gpstrRootTaskTcb;                /* �������TCBָ�� */


/***********************************************************************************
��������: ��ʼ��Wanlix����ϵͳ. �ú�������ϵͳ���񲢴ӷ�����״̬�л���root��������.
��ڲ���: pucTaskStack: root����ջ��ʼ��ַ, �õ�ַ��ջ�е���͵�ַ.
          uiStackSize: root����ջ��С, ��λ: �ֽ�.
�� �� ֵ: none.
***********************************************************************************/
void WLX_WanlixInit(U8* pucTaskStack, U32 uiStackSize)
{
    /**************************************************************************/
    /*            �������������    www.ifreecoding.com                       */
    /*                              bbs.ifreecoding.com                       */
    /*                              blog.sina.com.cn/ifreecoding              */
    /**************************************************************************/

    /* ���������� */
    gpstrRootTaskTcb = WLX_TaskCreate(WLX_RootTask, NULL, pucTaskStack,
                                      uiStackSize);

    /* ��ʼ�������, �Ӹ�����ʼִ�� */
    WLX_TaskStart(gpstrRootTaskTcb);
}

/***********************************************************************************
��������: ����һ������.
��ڲ���: vfFuncPointer: ����������ʹ�ú�����ָ��.
          pvPara: ������ڲ���ָ��.
          pucTaskStack: ������ʹ��ջ�������ʼ��ַ.
          uiStackSize: ջ��С, ��λ: �ֽ�.
�� �� ֵ: NULL: ��������ʧ��.
          ����: �����TCBָ��.
***********************************************************************************/
W_TCB* WLX_TaskCreate(VFUNC vfFuncPointer, void* pvPara, U8* pucTaskStack,
                      U32 uiStackSize)
{
    W_TCB* pstrTcb;

    /* �Դ���������ʹ�ú�����ָ��Ϸ��Խ��м�� */
    if(NULL == vfFuncPointer)
    {
        /* ָ��Ϊ��, ����ʧ�� */
        return (W_TCB*)NULL;
    }

    /* ������ջ�Ϸ��Խ��м�� */
    if((NULL == pucTaskStack) || (0 == uiStackSize))
    {
        /* ջ���Ϸ�, ����ʧ�� */
        return (W_TCB*)NULL;
    }

    /* ��ʼ��TCB */
    pstrTcb = WLX_TaskTcbInit(vfFuncPointer, pvPara, pucTaskStack, uiStackSize);

    return pstrTcb;
}

/***********************************************************************************
��������: ��ʼ������TCB, ����������������ǰTCB��״̬.
��ڲ���: vfFuncPointer: �������еĺ���.
          pvPara: ������ڲ���ָ��.
          pucTaskStack: ������ʹ�õ�ջ��ַ.
          uiStackSize: ջ��С, ��λ: �ֽ�.
�� �� ֵ: ����TCB��ָ��.
***********************************************************************************/
W_TCB* WLX_TaskTcbInit(VFUNC vfFuncPointer, void* pvPara, U8* pucTaskStack,
                       U32 uiStackSize)
{
    W_TCB* pstrTcb;
    U8* pucStackBy4;

    /* ��������ʱ��ջ����ַ�����TCB, ��Ҫ4�ֽڶ��� */
    pucStackBy4 = (U8*)(((U32)pucTaskStack + uiStackSize) & ALIGN4MASK);

    /* TCB��ַ������ʱʹ�õ�ջ��ʼ��ַ, cortex�ں�ʹ��8�ֽڶ��� */
    pstrTcb = (W_TCB*)(((U32)pucStackBy4 - sizeof(W_TCB)) & ALIGN8MASK);

    /* ��ʼ������ջ */
    WLX_TaskStackInit(pstrTcb, vfFuncPointer, pvPara);

    return pstrTcb;
}

/***********************************************************************************
��������: ��ʼ������ջ����, ����������������ǰ��ջ״̬.
��ڲ���: pstrTcb: �����TCBָ��.
          vfFuncPointer: �������еĺ���.
          pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void WLX_TaskStackInit(W_TCB* pstrTcb, VFUNC vfFuncPointer, void* pvPara)
{
    STACKREG* pstrStackReg;

    /* �Ĵ������ַ */
    pstrStackReg = &pstrTcb->strStackReg;

    /* ��TCB�еļĴ������ʼ�� */
    pstrStackReg->uiR0 = (U32)pvPara;           /* R0 */
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
}

/***********************************************************************************
��������: ���øú��������������л�.
��ڲ���: pstrTcb: �������е������TCBָ��.
�� �� ֵ: none.
***********************************************************************************/
void WLX_TaskSwitch(W_TCB* pstrTcb)
{
    STACKREG* pstrCurTaskStackRegAddr;
    STACKREG* pstrNextTaskStackRegAddr;

    /* ��ǰ����Ĵ�����ĵ�ַ */
    pstrCurTaskStackRegAddr = &gpstrCurTcb->strStackReg;

    /* ������������Ĵ�����ĵ�ַ */
    pstrNextTaskStackRegAddr = &pstrTcb->strStackReg;

    /* ���漴�����������TCB */
    gpstrCurTcb = pstrTcb;

    /* �л����� */
    WLX_ContextSwitch(pstrCurTaskStackRegAddr, pstrNextTaskStackRegAddr);
}

/***********************************************************************************
��������: ��ʼ�����������.
��ڲ���: pstrTcb: �������е������TCBָ��.
�� �� ֵ: none.
***********************************************************************************/
void WLX_TaskStart(W_TCB* pstrTcb)
{
    STACKREG* pstrNextTaskStackRegAddr;

    /* ������������Ĵ�����ĵ�ַ */
    pstrNextTaskStackRegAddr = &pstrTcb->strStackReg;

    /* ���漴�����������TCB */
    gpstrCurTcb = pstrTcb;

    /* �л�������״̬ */
    WLX_SwitchToTask(pstrNextTaskStackRegAddr);
}

/***********************************************************************************
��������: ��ȡWanlix�İ汾��.
��ڲ���: none.
�� �� ֵ: �洢�汾�ŵ��ַ�����ַ.
***********************************************************************************/
U8* WLX_GetWalixVersion(void)
{
    return WANLIX_VER;
}

