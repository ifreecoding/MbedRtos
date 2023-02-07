
#include "main.h"


M_SEM* gprx_sem;                    /* ���������ź��� */

M_QUE* gprx_que;                    /* ��Ϣ���ն��� */
M_QUE* gptx_que;                    /* ��Ϣ���Ͷ��� */


MSG_BUF grx_buf;                    /* ��Ϣ���ջ��� */
MSG_BUF gtx_buf;                    /* ��Ϣ���ͻ��� */


/***********************************************************************************
��������: ��Ϣ�����߳�.
��ڲ���: pdata: �̲߳���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void rx_thread(void* pdata)
{
    while(1)
    {
        /* �������� */
        msg_rx();
    }
}

/***********************************************************************************
��������: ��Ϣ�����߳�.
��ڲ���: pdata: �̲߳���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void handle_thread(void* pdata)
{
    while(1)
    {
        /* ������Ϣ */
        msg_process();
    }
}

/***********************************************************************************
��������: ��Ϣ�����߳�.
��ڲ���: pdata: �̲߳���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void tx_thread(void* pdata)
{
    while(1)
    {
        /* �������� */
        msg_tx();
    }
}

/***********************************************************************************
��������: ������Ϣ.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void msg_rx(void)
{
    /* ��ȡ�ź���, �����Ѿ����յ����� */
    if(RTN_SUCD != MDS_SemTake(gprx_sem, SEMWAITFEV))
    {
        return;
    }

    /* �����յ�����Ϣ������Ϣ���ն��� */
    if(RTN_SUCD != MDS_QuePut(gprx_que, &grx_buf.list))
    {
        return;
    }

    return;
}

/***********************************************************************************
��������: ������Ϣ.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void msg_process(void)
{
    MSG_BUF* prx_buf;
    M_DLIST* pque_node;

    /* ����Ϣ���ն��л�ȡ���յ�����Ϣ */
    if(RTN_SUCD != MDS_QueGet(gprx_que, &pque_node, QUEWAITFEV))
    {
        return;
    }

    prx_buf = (MSG_BUF*)pque_node;

    /* �����ջ����ڵ���Ϣ���Ƶ����ͻ����� */
    memcpy(gtx_buf.buf, prx_buf->buf, prx_buf->len);
    gtx_buf.len = prx_buf->len;

    /* ���ݴ������, ���ջ�����0, Ϊ�´ν���������׼�� */
    grx_buf.len = 0;

    /* ����Ҫ���͵���Ϣ���뷢�Ͷ��� */
    if(RTN_SUCD != MDS_QuePut(gptx_que, &gtx_buf.list))
    {
        return;
    }
}

/***********************************************************************************
��������: ������Ϣ, ����Ϣ��������.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void msg_tx(void)
{
    M_DLIST* pque_node;

    /* ����Ϣ���Ͷ��л�ȡ��Ҫ���͵���Ϣ */
    if(RTN_SUCD != MDS_QueGet(gptx_que, &pque_node, QUEWAITFEV))
    {
        return;
    }

    /* �������ڷ����жϷ������� */
    start_to_send_data();
}

/***********************************************************************************
��������: ��ʼ������1.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void uart1_init(void)
{
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable USART1 Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);

    /* ����UART1�ܽ� */
    /* Configure USART1 Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* ����UART1������ */
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

    /* ����UART1���ж� */
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
��������: ���������жϿ�ʼ��������.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void start_to_send_data(void)
{
    /* ʹ�ܷ��ʹ���, ��ʼ�������� */
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

/***********************************************************************************
��������: ��Ϣ�����жϺ���.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void msg_rx_isr(void)
{
    U32 rcv_char;

    /* �������� */
    rcv_char = USART_ReceiveData(USART1);

    /* �пռ���Խ������� */
    if(grx_buf.len < BUF_LEN)
    {
        /* û���յ��س�����洢���� */
        if(0x0D != rcv_char)
        {
            grx_buf.buf[grx_buf.len++] = rcv_char;
        }
        else /* ���յ��س����������ݽ������, �ͷ��ź����������������� */
        {
            (void)MDS_SemGive(gprx_sem);
        }
    }
}

/***********************************************************************************
��������: ��Ϣ�����жϺ���.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void msg_tx_isr(void)
{
    static U32 sent_len = 0;

    /* ����û��������������� */
    if(sent_len < gtx_buf.len)
    {
        USART_SendData(USART1, gtx_buf.buf[sent_len++]);
    }
    else
    {
        /* ���ݷ������, �رշ����ж� */
        USART_ITConfig(USART1, USART_IT_TXE, DISABLE);

        /* �ѷ��͵����ݳ�����0, Ϊ�´η���������׼�� */
        sent_len = 0;
    }
}

