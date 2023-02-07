
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "mindows_inner.h"


#ifdef MDS_DEBUGCONTEXT

M_CONTHEAD* gpstrContext;               /* ջ�л���Ϣͷ��ŵĵ�ַ */

#endif


/***********************************************************************************
��������: ��¼�����л�����, ���л�ǰ����ļĴ������ջ��Ϣ���浽ָ���ڴ�.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_SaveTaskContext(void)
{
#ifdef MDS_DEBUGCONTEXT

    U32 uiStack;
    U32 uiTcbAddr;
    U32 uiSaveLen;

    /* uiAbsAddrΪ0����˹��ܲ����� */
    if(0 == gpstrContext->uiAbsAddr)
    {
        return;
    }

    /* ͬһ����֮���л��򲻼�¼�����л���Ϣ */
    if(gpstrCurTaskReg == gpstrNextTaskReg)
    {
        return;
    }

    /* ��ȡ�л�ǰ�����SP�Ĵ�����ֵ */
    uiStack = gpstrCurTaskReg->uiR13;

    /* ��ȡ�л�ǰ�����TCB��ŵĵ�ַ */
    uiTcbAddr = (U32)gpstrCurTaskReg;

    /* ������Ҫ��������ݳ���, ����ջ�ṹ���Լ��Ĵ�������л�ǰ����ľֲ����� */
    uiSaveLen = sizeof(M_CONTMSG) + (uiTcbAddr - uiStack);

    /* Ҫ��������ݳ��ȴ����ܳ���, ��uiAbsAddr��Ϊ0, ����˹��ܲ����� */
    if(uiSaveLen > gpstrContext->uiLen)
    {
        gpstrContext->uiAbsAddr = 0;

        return;
    }

    while(1)
    {
        /* Ҫ��������ݳ��ȴ���ʣ�೤��, �򸲸��ʼ��¼������ */
        if(uiSaveLen > gpstrContext->uiRemLen)
        {
            MDS_CoverFirstContext();
        }
        else /* ʣ�೤���ܹ�������Ҫ��������� */
        {
            break;
        }
    }

    /* �������� */
    MDS_SaveLastContext(uiTcbAddr - uiStack);

#endif
}

/***********************************************************************************
��������: оƬ������Ӳ�������жϷ������, �ڸú����д�ӡ�����ջ����.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_FaultIsrPrint(void)
{
#ifdef MDS_DEBUGCONTEXT

    /* ��gpstrCurTaskRegָ��ǰ�ļĴ����� */
    gpstrCurTaskReg = &gpstrCurTcb->strStackReg;

    /* ��gpstrNextTaskRegָ���, �Ա��������쳣�ж� */
    gpstrNextTaskReg = (STACKREG*)NULL;

    /* ����ǰ�Ĵ������ջ��Ϣ���浽ָ���ڴ��� */
    MDS_SaveTaskContext();

    /* ��ӡ�ڴ��м�¼������Ĵ������ջ��Ϣ */
    MDS_PrintTaskContext();

#endif
}

#ifdef MDS_DEBUGCONTEXT

/***********************************************************************************
��������: ��ʼ����¼����Ĵ������ջ��Ϣ�Ĺ���.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_TaskContextInit(void)
{
    M_CONTMSG* pstrCurAddr;

    /* ȷ�����ջ��Ϣ�Ĵ�ŵ�ַ */
    gpstrContext = (M_CONTHEAD*)((MDS_CONTEXTADDR + 3) & ALIGN4MASK);

    /* ��¼����ʼ��ַ���뵽4�ֽ�, Ϊ���Ե�ַ */
    gpstrContext->uiAbsAddr = (U32)gpstrContext + sizeof(M_CONTHEAD);

    /* uiAbsAddrΪ0����˹��ܲ����� */
    if(0 == gpstrContext->uiAbsAddr)
    {
        return;
    }

    /* ��¼�ĵ�ǰ��ַ, Ϊ��Ե�ַ */
    gpstrContext->uiRelAddr = 0;

    /* ��¼����ʼ�ͽ�����ַ����Ҫ���뵽4�ֽ�, ��������ĳ��� */
    gpstrContext->uiLen = ((MDS_CONTEXTADDR + MDS_CONTEXTLEN) & ALIGN4MASK)
                          - gpstrContext->uiAbsAddr;

    /* ������ȹ�С, �˹��ܲ����� */
    if(gpstrContext->uiLen < sizeof(M_CONTMSG))
    {
        /* ��uiAbsAddr��Ϊ0, ����˹��ܲ����� */
        gpstrContext->uiAbsAddr = 0;

        return;
    }

    /* ������ȳ�����32λ������, �˹��ܲ����� */
    if(gpstrContext->uiAbsAddr > 0xFFFFFFFF - gpstrContext->uiLen)
    {
        /* ��uiAbsAddr��Ϊ0, ����˹��ܲ����� */
        gpstrContext->uiAbsAddr = 0;

        return;
    }

    /* ʣ��Ŀɼ�¼��Ϣ�ĳ��� */
    gpstrContext->uiRemLen = gpstrContext->uiLen;

    /* ��ʼ����ǰ��¼������ */
    pstrCurAddr = (M_CONTMSG*)(gpstrContext->uiAbsAddr + gpstrContext->uiRelAddr);
    pstrCurAddr->uiLen = 0;
}

/***********************************************************************************
��������: ��ӡ�����л�����, ���ڴ��м�¼������Ĵ������ջ��Ϣ��ӡ������.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_PrintTaskContext(void)
{
    /* uiAbsAddrΪ0����˹��ܲ����� */
    if(0 == gpstrContext->uiAbsAddr)
    {
        return;
    }

    MDS_SendString("\r\n\r\n!!!!!EXCEPTION OCCUR!!!!!\n\r");

    /* ��ӡ�����л���Ϣ */
    while(gpstrContext->uiLen > gpstrContext->uiRemLen)
    {
        MDS_PrintContext();
    }

    MDS_SendString("\r\n\r\n!!!!!EXCEPTION PRINT OVER!!!!!\n\r");
}

/***********************************************************************************
��������: ���Ǽ�¼�ĵ�һ�������л���Ϣ, ������ռ�õ��ڴ����ʣ��ռ�.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_CoverFirstContext(void)
{
    M_CONTMSG* pstrLstMsg;
    M_CONTMSG* pstrFstMsg;
    U32 uiIdleAddr;
    U32 uiFstAddr;

    /* ���һ���������Ϣ��ַ */
    pstrLstMsg = (M_CONTMSG*)(gpstrContext->uiAbsAddr + gpstrContext->uiRelAddr);

    /* ���һ����Ϣ��Ŀ��е�ַ */
    uiIdleAddr = gpstrContext->uiRelAddr + pstrLstMsg->uiLen;

    /* �����е�ַ���ڵ��ڱ������ܳ���˵�������ڴ��н��������Ϣ�����Խ�ߵͶ��ڴ� */
    if(uiIdleAddr >= gpstrContext->uiLen)
    {
        /* �����е��ڴ��ַ���µ��������Ͷ� */
        uiIdleAddr -= gpstrContext->uiLen;

        /* ��ȡ�ѱ���ĵ�һ����Ϣ��ַ */
        uiFstAddr = uiIdleAddr + gpstrContext->uiRemLen;
        pstrFstMsg = (M_CONTMSG*)(gpstrContext->uiAbsAddr + uiFstAddr);
    }
    else /* �����ڴ��н��������Ϣ��Խ�˸ߵͶ��ڴ� */
    {
        /* ��ȡ�ѱ���ĵ�һ����Ϣ��ַ */
        uiFstAddr = uiIdleAddr + gpstrContext->uiRemLen;

        /* ����һ����Ϣ�ĵ�ַ���ڵ��ڱ������ܳ���, ˵�������һ����Ϣ���ڴ�û�п�Խ
           �ߵͶ��ڴ� */
        if(uiFstAddr >= gpstrContext->uiLen)
        {
            /* ����һ����Ϣ���ڴ��ַ���µ��������Ͷ� */
            uiFstAddr -= gpstrContext->uiLen;
        }

        pstrFstMsg = (M_CONTMSG*)(gpstrContext->uiAbsAddr + uiFstAddr);
    }

    /* ���ǵ���һ����Ϣ, ����һ����Ϣ�Ŀռ��������ڴ��� */
    gpstrContext->uiRemLen += pstrFstMsg->uiLen;
}

/***********************************************************************************
��������: �������һ�������л���Ϣ.
��ڲ���: uiStackLen: ��Ҫ�����ջ�ĳ���.
�� �� ֵ: none.
***********************************************************************************/
void MDS_SaveLastContext(U32 uiStackLen)
{
    M_CONTMSG* pstrLstMsg;
    U32* puiDataAddr;
    U32 uiLstAddr;
    U32 uiDataAddr;
    U32 uiTcbAddr;
    U32 i;

    /* ���һ���������Ϣ */
    pstrLstMsg = (M_CONTMSG*)(gpstrContext->uiAbsAddr + gpstrContext->uiRelAddr);

    /* ������������Ϣ, �������һ��������Ϣ�ĵ�ַ */
    uiLstAddr = gpstrContext->uiRelAddr + pstrLstMsg->uiLen;

    /* ������Ϣ��ַ���ڵ��ڱ������ܳ���, ��Ҫ������Ϣ��ַ���µ��Ͷ��ڴ� */
    if(uiLstAddr >= gpstrContext->uiLen)
    {
        uiLstAddr -= gpstrContext->uiLen;
    }

    /* ������Ϣͷ�ṹ */
    /* ���泤�� */
    uiDataAddr = MDS_SaveOneContextData(uiLstAddr, sizeof(M_CONTMSG) + uiStackLen);

    /* ������һ����¼�ĵ�ַ */
    uiDataAddr = MDS_SaveOneContextData(uiDataAddr, gpstrContext->uiRelAddr);

    /* ������һ�������TCB */
    uiTcbAddr = (U32)gpstrCurTaskReg;
    uiDataAddr = MDS_SaveOneContextData(uiDataAddr, uiTcbAddr);

    /* ������һ�������TCB */
    if(NULL != gpstrNextTaskReg)
    {
        uiTcbAddr = (U32)gpstrNextTaskReg;
    }
    else /* gpstrNextTaskRegΪNULL˵�������쳣�ж�, �¸�TCB��¼Ϊ�� */
    {
        uiTcbAddr = 0;
    }

    uiDataAddr = MDS_SaveOneContextData(uiDataAddr, uiTcbAddr);

    /* ����Ĵ��������� */
    puiDataAddr = &gpstrCurTaskReg->uiExc_Rtn;
    for(i = 0; i < (sizeof(STACKREG) / sizeof(U32)); i++)
    {
        uiDataAddr = MDS_SaveOneContextData(uiDataAddr, *puiDataAddr--);
    }

    /* ����ջ���� */
    for(i = 0; i < (uiStackLen / sizeof(U32)); i++)
    {
        uiDataAddr = MDS_SaveOneContextData(uiDataAddr, *puiDataAddr--);
    }

    /* ����ȫ�ֱ��� */
    gpstrContext->uiRelAddr = uiLstAddr;
    gpstrContext->uiRemLen -= (sizeof(M_CONTMSG) + uiStackLen);
}

/***********************************************************************************
��������: ����һ������.
��ڲ���: uiAddr: �������ݵĵ�ַ.
          uiData: ���������.
�� �� ֵ: �¸��������ݵĵ�ַ.
***********************************************************************************/
U32 MDS_SaveOneContextData(U32 uiAddr, U32 uiData)
{
    ADDRVAL(gpstrContext->uiAbsAddr + uiAddr) = uiData;

    if(gpstrContext->uiLen != uiAddr + 4)
    {
        uiAddr += 4;
    }
    else
    {
        uiAddr = 0;
    }

    return uiAddr;
}

/***********************************************************************************
��������: ��ȡһ������.
��ڲ���: uiAddr: �������ݵĵ�ַ.
          uiData: ���������.
�� �� ֵ: �¸���ȡ���ݵĵ�ַ.
***********************************************************************************/
U32 MDS_GetOneContextData(U32 uiAddr, U32* puiData)
{
    *puiData = ADDRVAL(gpstrContext->uiAbsAddr + uiAddr);

    if(gpstrContext->uiLen != uiAddr + 4)
    {
        uiAddr += 4;
    }
    else
    {
        uiAddr = 0;
    }

    return uiAddr;
}

/***********************************************************************************
��������: ��ӡ�����л���Ϣ.
��ڲ���: uiAddr: ��Ҫ��ӡ����ʼ��ַ.
�� �� ֵ: none.
***********************************************************************************/
void MDS_PrintContext(void)
{
    U8* pucTaskName;
    U32 uiData;
    U32 uiLen;
    U32 uiDataAddr;
    U32 uiTcb;
    U32 i;

    uiDataAddr = gpstrContext->uiRelAddr;

    /* ��ȡ��Ϣͷ�ṹ */
    /* ��ȡ���� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiLen);

    /* ��ȡ�ϸ���¼�ĵ�ַ */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    gpstrContext->uiRelAddr = uiData;

    /* ��ȡǰһ�������TCB */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiTcb);
    pucTaskName = ((M_TCB*)uiTcb)->pucTaskName;
    MDS_IfValidString(&pucTaskName);
    MDS_SendString("\r\n\r\nTask %s 0x%08X ", pucTaskName, uiTcb);

    /* ��ȡ��һ�������TCB */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    if(0 != uiData)
    {
        pucTaskName = ((M_TCB*)uiData)->pucTaskName;
        MDS_IfValidString(&pucTaskName);
        MDS_SendString("switch to task %s 0x%08X ", pucTaskName, uiData);
    }
    else /* �¸������TCBΪ��, ˵���Ѿ������쳣�ж� */
    {
        MDS_SendString("occur exception!");
    }

    /* ��ȡ�Ĵ��������� */
    /* ��ȡExc_Rtn�Ĵ��� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString("\r\nExc_Rtn = 0x%08X", uiData);

    /* ��ȡXPSR�Ĵ��� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString("\r\nXPSR= 0x%08X", uiData);

    /* ��ȡR15�Ĵ��� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString("\r\nR15 = 0x%08X", uiData);

    /* ��ȡR14�Ĵ��� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R14 = 0x%08X", uiData);

    /* ��ȡR13�Ĵ��� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R13 = 0x%08X", uiData);

    /* ��ȡR12�Ĵ��� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R12 = 0x%08X", uiData);

    /* ��ȡR11�Ĵ��� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString("\r\nR11 = 0x%08X", uiData);

    /* ��ȡR10�Ĵ��� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R10 = 0x%08X", uiData);

    /* ��ȡR9�Ĵ��� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R9  = 0x%08X", uiData);

    /* ��ȡR8�Ĵ��� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R8  = 0x%08X", uiData);

    /* ��ȡR7�Ĵ��� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString("\r\nR7  = 0x%08X", uiData);

    /* ��ȡR6�Ĵ��� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R6  = 0x%08X", uiData);

    /* ��ȡR5�Ĵ��� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R5  = 0x%08X", uiData);

    /* ��ȡR4�Ĵ��� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R4  = 0x%08X", uiData);

    /* ��ȡR3�Ĵ��� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString("\r\nR3  = 0x%08X", uiData);

    /* ��ȡR2�Ĵ��� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R2  = 0x%08X", uiData);

    /* ��ȡR1�Ĵ��� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R1  = 0x%08X", uiData);

    /* ��ȡR0�Ĵ��� */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R0  = 0x%08X", uiData);

    /* ��ȡջ���� */
    for(i = 0; i < (uiLen - sizeof(M_CONTMSG)) / sizeof(U32); i++)
    {
        uiTcb -= 4;

        uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);

        MDS_SendString("\r\n0x%08X: 0x%08X", uiTcb, uiData);
    }

    /* ����ȫ�ֱ��� */
    gpstrContext->uiRemLen += uiLen;
}

/***********************************************************************************
��������: �ҽӷ����ַ��ĺ���.
��ڲ���: vfFuncPointer: ��Ҫ�ҽӵķ����ַ�����, �ú�����ԭ��ΪVFUNC1.
�� �� ֵ: none.
***********************************************************************************/
void MDS_SendCharFuncInit(VFUNC1 vfFuncPointer)
{
    gpstrContext->vfSendChar = vfFuncPointer;
}

/***********************************************************************************
��������: �ҽӺ���õķ����ַ�����.
��ڲ���: ucChar: ��Ҫ���͵��ַ�.
�� �� ֵ: none.
***********************************************************************************/
void MDS_SendChar(U8 ucChar)
{
    if(NULL == gpstrContext->vfSendChar)
    {
        return;
    }

    gpstrContext->vfSendChar(ucChar);
}

/***********************************************************************************
��������: ��ӡ�ַ�������.
��ڲ���: pvStringPt: ��һ��������ָ��, ����Ϊ�ַ���ָ��.
          ...: ��������.
�� �� ֵ: none.
***********************************************************************************/
void MDS_SendString(U8* pvStringPt, ...)
{
#define BUFLEN      200     /* ��Ϣ���峤�� */
    U8 aucBuf[BUFLEN];
    U32 i;
    va_list args;

    /* ���ַ�����ʽ�����ڴ� */
    va_start(args, pvStringPt);
    (void)vsprintf(aucBuf, pvStringPt, args);
    va_end(args);

    /* ���ڴ��е���Ϣ��ӡ������ */
    for(i = 0; NULL != aucBuf[i]; i++)
    {
        MDS_SendChar(aucBuf[i]);
    }
}

/***********************************************************************************
��������: �ж��ַ����Ƿ���Ч, �Ϸ����ַ�����Ҫ�ǲ�Ϊ��, �ǿɴ�ӡ���ַ�, ����Ϊ���޳�
          ��. ������Ч���ַ����򽫴洢�ַ�����ַ��ָ���޸�ָ��"BAD DATA!".
��ڲ���: pucString: �洢�ַ�����ַ��ָ��.
�� �� ֵ: none.
***********************************************************************************/
void MDS_IfValidString(U8** pucString)
{
    U8* pucStr;
    U32 i;

    /* ��ڲ���Ϊ��, ����ʧ�� */
    if(NULL == pucString)
    {
        goto BAD_RTN;
    }

    /* ȡ���ַ�����ַ */
    pucStr = *pucString;

    /* �ַ�����ַΪ�Ƿ���ַ, ����ʧ�� */
    if(((U32)pucStr < ROMVALIDADDRSTART) || ((U32)pucStr > ROMVALIDADDREND)
       || (NULL == pucStr))
    {
        goto BAD_RTN;
    }

    /* �ڹ涨�ĳ����ڲ�ѯ�ַ����Ƿ�Ϊ�ɴ�ӡ�ַ� */
    for(i = 0; i < VALIDTASKNAMEMAXLEN; i++)
    {
        /* �����ַ���������, ���سɹ� */
        if(NULL == pucStr[i])
        {
            return;
        }

        /* �ַ����������ɴ�ӡ�ַ�, ����ʧ�� */
        if((pucStr[i] < ' ') || (pucStr[i] > '~'))
        {
            goto BAD_RTN;
        }
    }

    /* �ַ������ȳ���, ����ʧ�� */

BAD_RTN:

    *pucString = "BAD DATA!";

    return;
}

#endif

