
#include "uart.h"


RING_BUF grx_ring;              /* ���ν��ջ��� */
RING_BUF gtx_ring;              /* ���η��ͻ��� */


U32 guart_tx_status;            /* ����Ӳ������״̬ */


M_SEM* gprx_sycn_sem;           /* ���ڽ���ͬ���ź��� */


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
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1
                           | RCC_APB2Periph_AFIO, ENABLE);

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
��������: ��ʼ������2.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void uart2_init(void)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable USART2 Clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* ����UART2�ܽ� */
    /* Configure USART2 Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART2 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* ����UART2������ */
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
��������: ��Ϣ�����жϺ���, �ú����ɴ���1�жϽ�������, �����յ����ݴ��뻷�ν��ջ���
          ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void msg_rx_isr(void)
{
    U32 temp;

    /* ���ν��ջ�����ͷβָ������1����, ˵���пռ���Խ������� */
    if(grx_ring.head != (grx_ring.tail + 1) % RING_BUF_LEN)
    {
        grx_ring.buf[grx_ring.tail++] = USART_ReceiveData(USART1);
        grx_ring.tail %= RING_BUF_LEN;
    }
    else /* ����������, �������յ����ݺ��� */
    {
        temp = USART_ReceiveData(USART1);
        temp = temp;
    }

    /* �ͷ��ź���, ֪ͨdata_receive�������Խ������� */
    (void)MDS_SemGive(gprx_sycn_sem);
}

/***********************************************************************************
��������: ��Ϣ�����жϺ���, �ú����ɴ���1�жϷ�������, ����Ҫ���͵����ݴӻ��η��ͻ�
          �����з��ͳ�ȥ.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void msg_tx_isr(void)
{
    /* ���η��ͻ�����ͷβָ�벻ͬ, ˵�������ݿ��Է��� */
    if(gtx_ring.head != gtx_ring.tail)
    {
        USART_SendData(USART1, gtx_ring.buf[gtx_ring.head++]);
        gtx_ring.head %= RING_BUF_LEN;
    }
    else
    {
        /* ���ݷ������, �رշ����ж� */
        USART_ITConfig(USART1, USART_IT_TXE, DISABLE);

        /* ��Ӳ����Ϊ����̬ */
        guart_tx_status = UART_TX_IDLE;
    }
}

/***********************************************************************************
��������: �������ݺ���, �ú����ӻ��ν��ջ������ж�ȡ����, ���û�н��յ�������ú���
          �ᱻ�ź�������.
��ڲ���: pbuf: �����������洢��buf.
          len: ָ�����յ�����ֽ���.
�� �� ֵ: ʵ�ʽ��յ����ֽ���.
***********************************************************************************/
U32 data_receive(U8* pbuf, U32 len)
{
    U32 copy_len;

    if(0 == len)
    {
        return 0;
    }

REREAD:

    /* ��ȡ����ǰ��ȡ�ź���, ��û�����ݸú����ᱻ�ź������� */
    (void)MDS_SemTake(gprx_sycn_sem, SEMWAITFEV);

    /* ���㻷�ν��ջ����ڵ����ݳ��� */
    copy_len = (grx_ring.tail + RING_BUF_LEN - grx_ring.head) % RING_BUF_LEN;

    /* ������ֶ�ȡ�������ݶ��ú����ֿ��Է��ص���� */
    if(0 == copy_len)
    {
        goto REREAD;
    }

    /* ȷ����Ҫcopy�����ݳ��� */
    if(len < copy_len)
    {
        copy_len = len;

        /* �ͷ��ź���, �����ٴ�ʹ�øú�����ȡ����ʱ��ȡ�����ź��� */
        (void)MDS_SemGive(gprx_sycn_sem);
    }

    /* �ӻ��ν��ջ�������copy���� */
    copy_data_from_ring_rx_buf(pbuf, copy_len);

    return copy_len;
}

/***********************************************************************************
��������: �������ݺ���, �ú��������ݴ��뻷�η��ͻ�������.
��ڲ���: pbuf: �����������洢��buf.
          len: ָ�����͵��ֽ���.
�� �� ֵ: ʵ�ʷ��͵����η��ͻ�����ֽ���.
***********************************************************************************/
U32 data_send(U8* pbuf, U32 len)
{
    U32 copy_len;

    /* ���㻷�η��ͻ���ɴ�ŵ����ݳ��� */
    copy_len = (gtx_ring.head + RING_BUF_LEN - gtx_ring.tail - 1) % RING_BUF_LEN;

    /* ȷ����Ҫcopy�����ݳ��� */
    if(len < copy_len)
    {
        copy_len = len;
    }

    /* ������copy�����η��ͻ������� */
    copy_data_to_ring_tx_buf(pbuf, copy_len);

    return copy_len;
}

/***********************************************************************************
��������: ���ν��ջ�������ʼ������.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void msg_rx_ring_buf_init(void)
{
    grx_ring.head = 0;
    grx_ring.tail = 0;

    gprx_sycn_sem = MDS_SemCreate(NULL, SEMBIN | SEMFIFO, SEMEMPTY);
}

/***********************************************************************************
��������: ���η��ͻ�������ʼ������.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void msg_tx_ring_buf_init(void)
{
    gtx_ring.head = 0;
    gtx_ring.tail = 0;

    /* ��Ӳ����Ϊ����̬ */
    guart_tx_status = UART_TX_IDLE;
}

/***********************************************************************************
��������: �����ν��ջ������е����ݸ��Ƶ���������.
��ڲ���: pbuf: ������ݵĻ�����ָ��.
          len: ���Ƶ����ν��ջ����������ݳ���.
�� �� ֵ: none.
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
��������: ����Ҫ���͵����ݴӻ��������Ƶ����η��ͻ�������.
��ڲ���: pbuf: ��Ҫ���͵��������ڵĻ�����ָ��.
          len: ���Ƶ����η��ͻ����������ݳ���.
�� �� ֵ: none.
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

    /* Ӳ�����ڿ���״̬, ����UART�жϿ�ʼ�������� */
    if(UART_TX_IDLE == guart_tx_status)
    {
        /* ��Ӳ����Ϊ����̬ */
        guart_tx_status = UART_TX_BUSY;

        /* ʹ�ܴ��ڷ����ж�, ��ʼ�������� */
        USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

        return;
    }
}

