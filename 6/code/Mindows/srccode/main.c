
#include "main.h"


M_SEM* gprx_sem;                    /* 接收任务信号量 */

M_QUE* gprx_que;                    /* 消息接收队列 */
M_QUE* gptx_que;                    /* 消息发送队列 */


MSG_BUF grx_buf;                    /* 消息接收缓冲 */
MSG_BUF gtx_buf;                    /* 消息发送缓冲 */


/***********************************************************************************
函数功能: 消息接收线程.
入口参数: pdata: 线程参数指针.
返 回 值: none.
***********************************************************************************/
void rx_thread(void* pdata)
{
    while(1)
    {
        /* 接收数据 */
        msg_rx();
    }
}

/***********************************************************************************
函数功能: 消息处理线程.
入口参数: pdata: 线程参数指针.
返 回 值: none.
***********************************************************************************/
void handle_thread(void* pdata)
{
    while(1)
    {
        /* 处理消息 */
        msg_process();
    }
}

/***********************************************************************************
函数功能: 消息发送线程.
入口参数: pdata: 线程参数指针.
返 回 值: none.
***********************************************************************************/
void tx_thread(void* pdata)
{
    while(1)
    {
        /* 发送数据 */
        msg_tx();
    }
}

/***********************************************************************************
函数功能: 接收消息.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void msg_rx(void)
{
    /* 获取信号量, 串口已经接收到数据 */
    if(RTN_SUCD != MDS_SemTake(gprx_sem, SEMWAITFEV))
    {
        return;
    }

    /* 将接收到的消息放入消息接收队列 */
    if(RTN_SUCD != MDS_QuePut(gprx_que, &grx_buf.list))
    {
        return;
    }

    return;
}

/***********************************************************************************
函数功能: 处理消息.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void msg_process(void)
{
    MSG_BUF* prx_buf;
    M_DLIST* pque_node;

    /* 从消息接收队列获取接收到的消息 */
    if(RTN_SUCD != MDS_QueGet(gprx_que, &pque_node, QUEWAITFEV))
    {
        return;
    }

    prx_buf = (MSG_BUF*)pque_node;

    /* 将接收缓冲内的消息复制到发送缓冲内 */
    memcpy(gtx_buf.buf, prx_buf->buf, prx_buf->len);
    gtx_buf.len = prx_buf->len;

    /* 数据处理完毕, 接收缓冲清0, 为下次接收数据做准备 */
    grx_buf.len = 0;

    /* 将需要发送的消息放入发送队列 */
    if(RTN_SUCD != MDS_QuePut(gptx_que, &gtx_buf.list))
    {
        return;
    }
}

/***********************************************************************************
函数功能: 发送消息, 将消息发给主机.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void msg_tx(void)
{
    M_DLIST* pque_node;

    /* 从消息发送队列获取需要发送的消息 */
    if(RTN_SUCD != MDS_QueGet(gptx_que, &pque_node, QUEWAITFEV))
    {
        return;
    }

    /* 启动串口发送中断发送数据 */
    start_to_send_data();
}

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
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);

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
函数功能: 启动串口中断开始发送数据.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void start_to_send_data(void)
{
    /* 使能发送串口, 开始发送数据 */
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

/***********************************************************************************
函数功能: 消息接收中断函数.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void msg_rx_isr(void)
{
    U32 rcv_char;

    /* 接收数据 */
    rcv_char = USART_ReceiveData(USART1);

    /* 有空间可以接收数据 */
    if(grx_buf.len < BUF_LEN)
    {
        /* 没接收到回车符则存储数据 */
        if(0x0D != rcv_char)
        {
            grx_buf.buf[grx_buf.len++] = rcv_char;
        }
        else /* 接收到回车符代表数据接收完毕, 释放信号量激活任务处理数据 */
        {
            (void)MDS_SemGive(gprx_sem);
        }
    }
}

/***********************************************************************************
函数功能: 消息发送中断函数.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void msg_tx_isr(void)
{
    static U32 sent_len = 0;

    /* 数据没发送完毕则发送数据 */
    if(sent_len < gtx_buf.len)
    {
        USART_SendData(USART1, gtx_buf.buf[sent_len++]);
    }
    else
    {
        /* 数据发送完毕, 关闭发送中断 */
        USART_ITConfig(USART1, USART_IT_TXE, DISABLE);

        /* 已发送的数据长度清0, 为下次发送数据做准备 */
        sent_len = 0;
    }
}

