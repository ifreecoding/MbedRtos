
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "device.h"


BUFPOOL gstrBufPool;                /* 消息缓冲池 */


/***********************************************************************************
函数功能: 初始化软件.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_SoftwareInit(void)
{
    /* 初始化消息缓冲池 */
    DEV_BufferInit(&gstrBufPool);

    /* 创建打印消息的队列 */
    (void)MDS_QueCreate(&gstrSerialMsgQue);
}

/***********************************************************************************
函数功能: 初始化硬件.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void DEV_HardwareInit(void)
{
    /* 初始化串口 */
    DEV_UartInit();
}

/***********************************************************************************
函数功能: 初始化串口.
入口参数: none.
返 回 值: none.
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
函数功能: 向串口发送一个字符.
入口参数: ucChar: 输出的字符.
返 回 值: none.
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
函数功能: 将字符串打印到内存. 该函数入口参数与printf函数一致.
入口参数: pvStringPt: 第一个参数的指针, 必须为字符串指针.
          ...: 其它参数.
返 回 值: none.
***********************************************************************************/
void DEV_PutStrToMem(U8* pvStringPt, ...)
{
    MSGBUF* pstrMsgBuf;
    va_list args;

    /* 申请buf, 用来存放需要打印的字符 */
    pstrMsgBuf = (MSGBUF*)DEV_BufferAlloc(&gstrBufPool);
    if(NULL == pstrMsgBuf)
    {
        return;
    }

    /* 将字符串打印到内存 */
    va_start(args, pvStringPt);
    (void)vsprintf(pstrMsgBuf->aucBuf, pvStringPt, args);
    va_end(args);

    /* 填充buf参数 */
    pstrMsgBuf->ucLength = strlen(pstrMsgBuf->aucBuf);

    /* 将buf挂入队列 */
    (void)MDS_QuePut(&gstrSerialMsgQue, &pstrMsgBuf->strList);
}

/***********************************************************************************
函数功能: 向串口打印消息. 从内存中指定地址向串口连续打印一定长度的数据, 采用串口查询
          方式打印.
入口参数: pucBuf: 需要打印数据存放的缓冲地址.
          uiLen: 需要打印数据的长度, 单位: 字节.
返 回 值: none.
***********************************************************************************/
void DEV_PrintMsg(U8* pucBuf, U32 uiLen)
{
    U32 uiCounter;

    uiCounter = 0;

    /* 采用查询的方式发送数据 */
    while(uiCounter < uiLen)
    {
        /* 向串口发一个字符 */
        (void)DEV_PutChar(pucBuf[uiCounter++]);
    }
}

/***********************************************************************************
函数功能: 初始化消息缓冲池.
入口参数: pstrBufPool: 消息缓冲池指针.
返 回 值: none.
***********************************************************************************/
void DEV_BufferInit(BUFPOOL* pstrBufPool)
{
    U32 i;

    /* 初始化缓冲池空闲链表 */
    MDS_DlistInit(&pstrBufPool->strFreeList);

    /* 将各个缓冲挂入空闲链表 */
    for(i = 0; i < BUFPOOLNUM; i++)
    {
        MDS_DlistNodeAdd(&pstrBufPool->strFreeList,
                         &pstrBufPool->astrBufPool[i].strList);
    }
}

/***********************************************************************************
函数功能: 从消息缓冲池中申请消息缓冲.
入口参数: pstrBufPool: 消息缓冲池指针.
返 回 值: 分配的消息缓冲指针.
***********************************************************************************/
MSGBUF* DEV_BufferAlloc(BUFPOOL* pstrBufPool)
{
    MSGBUF* pstrBuf;

    (void)MDS_IntLock();

    /* 从空闲链表分配一个缓冲 */
    pstrBuf = (MSGBUF*)MDS_DlistNodeDelete(&pstrBufPool->strFreeList);

    (void)MDS_IntUnlock();

    return pstrBuf;
}

/***********************************************************************************
函数功能: 释放将消息缓冲释放回消息缓冲池.
入口参数: pstrBufPool: 消息缓冲池指针.
          pstrQueNode: 释放的缓冲节点指针.
返 回 值: none.
***********************************************************************************/
void DEV_BufferFree(BUFPOOL* pstrBufPool, M_DLIST* pstrQueNode)
{
    (void)MDS_IntLock();

    /* 将释放的缓冲挂到空闲链表 */
    MDS_DlistNodeAdd(&pstrBufPool->strFreeList, pstrQueNode);

    (void)MDS_IntUnlock();
}

