
#include "uart.h"


RING_BUF grx_ring;              /* 环形接收缓冲 */
RING_BUF gtx_ring;              /* 环形发送缓冲 */


U32 guart_tx_status;            /* 串口硬件发送状态 */


M_SEM* gprx_sycn_sem;           /* 串口接收同步信号量 */


/***********************************************************************************
函数功能: 初始化串口1.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void uart1_init(void)
{
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable USART1 Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1
                           | RCC_APB2Periph_AFIO, ENABLE);

    /* 配置UART1管脚 */
    /* Configure USART1 Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 配置UART1的配置 */
    /* USARTy and USARTz configured as follow:
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

    /* 配置UART1的中断 */
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable USARTy Receive interrupts */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    /* Enable the USART1 */
    USART_Cmd(USART1, ENABLE);
}

/***********************************************************************************
函数功能: 初始化串口2.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void uart2_init(void)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable USART2 Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* 配置UART2管脚 */
    /* Configure USART2 Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART2 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 配置UART2的配置 */
    /* USARTy and USARTz configured as follow:
        - BaudRate = 115200 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Receive and transmit enabled
        - Hardware flow control disabled (RTS and CTS signals)
    */
    USART_StructInit(&USART_InitStructure);

    /* Configure USART2 */
    USART_Init(USART2, &USART_InitStructure);

    /* Enable the USART2 */
    USART_Cmd(USART2, ENABLE);
}

/***********************************************************************************
函数功能: 消息接收中断函数, 该函数由串口1中断接收数据, 将接收的数据存入环形接收缓冲
          区.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void msg_rx_isr(void)
{
    U32 temp;

    /* 环形接收缓冲区头尾指针间隔在1以上, 说明有空间可以接收数据 */
    if(grx_ring.head != (grx_ring.tail + 1) % RING_BUF_LEN)
    {
        grx_ring.buf[grx_ring.tail++] = USART_ReceiveData(USART1);
        grx_ring.tail %= RING_BUF_LEN;
    }
    else /* 缓冲区已满, 读出接收的数据后丢弃 */
    {
        temp = USART_ReceiveData(USART1);
        temp = temp;
    }

    /* 释放信号量, 通知data_receive函数可以接收数据 */
    (void)MDS_SemGive(gprx_sycn_sem);
}

/***********************************************************************************
函数功能: 消息发送中断函数, 该函数由串口1中断发送数据, 将需要发送的数据从环形发送缓
          冲区中发送出去.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void msg_tx_isr(void)
{
    /* 环形发送缓冲区头尾指针不同, 说明有数据可以发送 */
    if(gtx_ring.head != gtx_ring.tail)
    {
        USART_SendData(USART1, gtx_ring.buf[gtx_ring.head++]);
        gtx_ring.head %= RING_BUF_LEN;
    }
    else
    {
        /* 数据发送完毕, 关闭发送中断 */
        USART_ITConfig(USART1, USART_IT_TXE, DISABLE);

        /* 将硬件置为空闲态 */
        guart_tx_status = UART_TX_IDLE;
    }
}

/***********************************************************************************
函数功能: 接收数据函数, 该函数从环形接收缓冲区中读取数据, 如果没有接收到数据则该函数
          会被信号量阻塞.
入口参数: pbuf: 接收数据所存储的buf.
          len: 指定接收的最大字节数.
返 回 值: 实际接收到的字节数.
***********************************************************************************/
U32 data_receive(U8* pbuf, U32 len)
{
    U32 copy_len;

    if(0 == len)
    {
        return 0;
    }

REREAD:

    /* 读取数据前获取信号量, 若没有数据该函数会被信号量阻塞 */
    (void)MDS_SemTake(gprx_sycn_sem, SEMWAITFEV);

    /* 计算环形接收缓冲内的数据长度 */
    copy_len = (grx_ring.tail + RING_BUF_LEN - grx_ring.head) % RING_BUF_LEN;

    /* 避免出现读取不到数据而该函数又可以返回的情况 */
    if(0 == copy_len)
    {
        goto REREAD;
    }

    /* 确定需要copy的数据长度 */
    if(len < copy_len)
    {
        copy_len = len;

        /* 释放信号量, 避免再次使用该函数读取数据时获取不到信号量 */
        (void)MDS_SemGive(gprx_sycn_sem);
    }

    /* 从环形接收缓冲区中copy数据 */
    copy_data_from_ring_rx_buf(pbuf, copy_len);

    return copy_len;
}

/***********************************************************************************
函数功能: 发送数据函数, 该函数将数据存入环形发送缓冲区中.
入口参数: pbuf: 发送数据所存储的buf.
          len: 指定发送的字节数.
返 回 值: 实际发送到环形发送缓冲的字节数.
***********************************************************************************/
U32 data_send(U8* pbuf, U32 len)
{
    U32 copy_len;

    /* 计算环形发送缓冲可存放的数据长度 */
    copy_len = (gtx_ring.head + RING_BUF_LEN - gtx_ring.tail - 1) % RING_BUF_LEN;

    /* 确定需要copy的数据长度 */
    if(len < copy_len)
    {
        copy_len = len;
    }

    /* 将数据copy到环形发送缓冲区中 */
    copy_data_to_ring_tx_buf(pbuf, copy_len);

    return copy_len;
}

/***********************************************************************************
函数功能: 环形接收缓冲区初始化函数.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void msg_rx_ring_buf_init(void)
{
    grx_ring.head = 0;
    grx_ring.tail = 0;

    gprx_sycn_sem = MDS_SemCreate(NULL, SEMBIN | SEMFIFO, SEMEMPTY);
}

/***********************************************************************************
函数功能: 环形发送缓冲区初始化函数.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void msg_tx_ring_buf_init(void)
{
    gtx_ring.head = 0;
    gtx_ring.tail = 0;

    /* 将硬件置为空闲态 */
    guart_tx_status = UART_TX_IDLE;
}

/***********************************************************************************
函数功能: 将环形接收缓冲区中的数据复制到缓冲区中.
入口参数: pbuf: 存放数据的缓冲区指针.
          len: 复制到环形接收缓冲区的数据长度.
返 回 值: none.
***********************************************************************************/
void copy_data_from_ring_rx_buf(U8* pbuf, U32 len)
{
    U32 head;
    U32 i;

    head = grx_ring.head;

    for(i = 0; i < len; i++)
    {
        pbuf[i] = grx_ring.buf[head++];
        head %= RING_BUF_LEN;
    }

    grx_ring.head = head;
}

/***********************************************************************************
函数功能: 将需要发送的数据从缓冲区复制到环形发送缓冲区中.
入口参数: pbuf: 需要发送的数据所在的缓冲区指针.
          len: 复制到环形发送缓冲区的数据长度.
返 回 值: none.
***********************************************************************************/
void copy_data_to_ring_tx_buf(U8* pbuf, U32 len)
{
    U32 tail;
    U32 i;

    tail = gtx_ring.tail;

    for(i = 0; i < len; i++)
    {
        gtx_ring.buf[tail++] = pbuf[i];
        tail %= RING_BUF_LEN;
    }

    gtx_ring.tail = tail;

    /* 硬件处于空闲状态, 触发UART中断开始发送数据 */
    if(UART_TX_IDLE == guart_tx_status)
    {
        /* 将硬件置为发送态 */
        guart_tx_status = UART_TX_BUSY;

        /* 使能串口发送中断, 开始发送数据 */
        USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

        return;
    }
}

