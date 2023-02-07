
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "device.h"


BUFPOOL gstrBufPool;                /* ��Ϣ����� */


/***********************************************************************************
��������: ��ʼ�����.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_SoftwareInit(void)
{
    /* ��ʼ����Ϣ����� */
    DEV_BufferInit(&gstrBufPool);

    /* ������ӡ��Ϣ�Ķ��� */
    (void)MDS_QueCreate(&gstrSerialMsgQue);
}

/***********************************************************************************
��������: ��ʼ��Ӳ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_HardwareInit(void)
{
    /* ��ʼ������ */
    DEV_UartInit();
}

/***********************************************************************************
��������: ��ʼ������.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void DEV_UartInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* Enable USART1 Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1
                           | RCC_APB2Periph_AFIO, ENABLE);

    /* Configure USART1 Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART1 configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Receive and transmit enabled
        - Hardware flow control disabled (RTS and CTS signals)
    */
    USART_StructInit(&USART_InitStructure);

    /* Configure USART1 */
    USART_Init(USART1, &USART_InitStructure);

    /* Enable the USART1 */
    USART_Cmd(USART1, ENABLE);
}

/***********************************************************************************
��������: �򴮿ڷ���һ���ַ�.
��ڲ���: ucChar: ������ַ�.
�� �� ֵ: none.
***********************************************************************************/
void DEV_PutChar(U8 ucChar)
{
    /* Loop until USART1 DR register is empty */
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
        ;
    }

    USART_SendData(USART1, ucChar);
}

/***********************************************************************************
��������: ���ַ�����ӡ���ڴ�. �ú�����ڲ�����printf����һ��.
��ڲ���: pvStringPt: ��һ��������ָ��, ����Ϊ�ַ���ָ��.
          ...: ��������.
�� �� ֵ: none.
***********************************************************************************/
void DEV_PutStrToMem(U8* pvStringPt, ...)
{
    MSGBUF* pstrMsgBuf;
    va_list args;

    /* ����buf, ���������Ҫ��ӡ���ַ� */
    pstrMsgBuf = (MSGBUF*)DEV_BufferAlloc(&gstrBufPool);
    if(NULL == pstrMsgBuf)
    {
        return;
    }

    /* ���ַ�����ӡ���ڴ� */
    va_start(args, pvStringPt);
    (void)vsprintf(pstrMsgBuf->aucBuf, pvStringPt, args);
    va_end(args);

    /* ���buf���� */
    pstrMsgBuf->ucLength = strlen(pstrMsgBuf->aucBuf);

    /* ��buf������� */
    (void)MDS_QuePut(&gstrSerialMsgQue, &pstrMsgBuf->strList);
}

/***********************************************************************************
��������: �򴮿ڴ�ӡ��Ϣ. ���ڴ���ָ����ַ�򴮿�������ӡһ�����ȵ�����, ���ô��ڲ�ѯ
          ��ʽ��ӡ.
��ڲ���: pucBuf: ��Ҫ��ӡ���ݴ�ŵĻ����ַ.
          uiLen: ��Ҫ��ӡ���ݵĳ���, ��λ: �ֽ�.
�� �� ֵ: none.
***********************************************************************************/
void DEV_PrintMsg(U8* pucBuf, U32 uiLen)
{
    U32 uiCounter;

    uiCounter = 0;

    /* ���ò�ѯ�ķ�ʽ�������� */
    while(uiCounter < uiLen)
    {
        /* �򴮿ڷ�һ���ַ� */
        (void)DEV_PutChar(pucBuf[uiCounter++]);
    }
}

/***********************************************************************************
��������: ��ʼ����Ϣ�����.
��ڲ���: pstrBufPool: ��Ϣ�����ָ��.
�� �� ֵ: none.
***********************************************************************************/
void DEV_BufferInit(BUFPOOL* pstrBufPool)
{
    U32 i;

    /* ��ʼ������ؿ������� */
    MDS_DlistInit(&pstrBufPool->strFreeList);

    /* �������������������� */
    for(i = 0; i < BUFPOOLNUM; i++)
    {
        MDS_DlistNodeAdd(&pstrBufPool->strFreeList,
                         &pstrBufPool->astrBufPool[i].strList);
    }
}

/***********************************************************************************
��������: ����Ϣ�������������Ϣ����.
��ڲ���: pstrBufPool: ��Ϣ�����ָ��.
�� �� ֵ: �������Ϣ����ָ��.
***********************************************************************************/
MSGBUF* DEV_BufferAlloc(BUFPOOL* pstrBufPool)
{
    MSGBUF* pstrBuf;

    (void)MDS_IntLock();

    /* �ӿ����������һ������ */
    pstrBuf = (MSGBUF*)MDS_DlistNodeDelete(&pstrBufPool->strFreeList);

    (void)MDS_IntUnlock();

    return pstrBuf;
}

/***********************************************************************************
��������: �ͷŽ���Ϣ�����ͷŻ���Ϣ�����.
��ڲ���: pstrBufPool: ��Ϣ�����ָ��.
          pstrQueNode: �ͷŵĻ���ڵ�ָ��.
�� �� ֵ: none.
***********************************************************************************/
void DEV_BufferFree(BUFPOOL* pstrBufPool, M_DLIST* pstrQueNode)
{
    (void)MDS_IntLock();

    /* ���ͷŵĻ���ҵ��������� */
    MDS_DlistNodeAdd(&pstrBufPool->strFreeList, pstrQueNode);

    (void)MDS_IntUnlock();
}

