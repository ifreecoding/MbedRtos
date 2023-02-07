
#include "main.h"


OS_EVENT* gprx_sem;                 /* ���������ź��� */

OS_EVENT* gprx_que;                 /* ��Ϣ���ն��� */
OS_EVENT* gptx_que;                 /* ��Ϣ���Ͷ��� */

void* garx_buf[QUE_RING_LEN];       /* ���ն�����ʹ�õĻ��λ��� */
void* gatx_buf[QUE_RING_LEN];       /* ���Ͷ�����ʹ�õĻ��λ��� */

MSG_BUF grx_buf;                    /* ��Ϣ���ջ��� */
MSG_BUF gtx_buf;                    /* ��Ϣ���ͻ��� */


/* ������ʹ�õ�ջ */
OS_STK garx_stack[TASK_STASK_LEN];
OS_STK gahandle_stack[TASK_STASK_LEN];
OS_STK gatx_stack[TASK_STASK_LEN];


/***********************************************************************************
��������: ������.
��ڲ���: none.
�� �� ֵ: ����ֵΪ0, ��ϵͳ�������е��ú�������.
***********************************************************************************/
S32 main(void)
{
    /**************************************************************************/
    /*            �������������    www.ifreecoding.com                       */
    /*                              bbs.ifreecoding.com                       */
    /*                              blog.sina.com.cn/ifreecoding              */
    /**************************************************************************/

    /* ϵͳtick��ʱ����ʼ�� */
    sys_tick_init();

    /* ���ڳ�ʼ�� */
    uart1_init();

    /* ����ϵͳ��ʼ�� */
    OSInit();

    /* ��ʼ�����ջ��� */
    grx_buf.len = 0;

    /* �����ź��� */
    gprx_sem = OSSemCreate(0);

    /* �������� */
    gprx_que = OSQCreate(&garx_buf[0], QUE_RING_LEN);
    gptx_que = OSQCreate(&gatx_buf[0], QUE_RING_LEN);

    /* ����rx_thread���� */
    OSTaskCreate(rx_thread, NULL, (OS_STK *)&garx_stack[TASK_STASK_LEN - 1], 8);

    /* ����handle_thread���� */
    OSTaskCreate(handle_thread, NULL, (OS_STK *)&gahandle_stack[TASK_STASK_LEN - 1],
                 10);

    /* ����tx_thread���� */
    OSTaskCreate(tx_thread, NULL, (OS_STK *)&gatx_stack[TASK_STASK_LEN - 1], 12);

    /* ��������ϵͳ */
    OSStart();

    return 0;
}

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
    U8 err;

    /* ��ȡ�ź���, �����Ѿ����յ����� */
    OSSemPend(gprx_sem, 0, &err);
    if(OS_NO_ERR != err)
    {
        return;
    }

    /* �����յ�����Ϣ������Ϣ���ն��� */
    err = OSQPost(gprx_que, (void*)&grx_buf);
    if(OS_NO_ERR != err)
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
    U8 err;

    /* ����Ϣ���ն��л�ȡ���յ�����Ϣ */
    prx_buf = (MSG_BUF*)OSQPend(gprx_que, 0, &err);
    if((NULL == prx_buf) || (OS_NO_ERR != err))
    {
        return;
    }

    /* �����ջ����ڵ���Ϣ���Ƶ����ͻ����� */
    memcpy(gtx_buf.buf, prx_buf->buf, prx_buf->len);
    gtx_buf.len = prx_buf->len;

    /* ���ݴ������, ���ջ�����0, Ϊ�´ν���������׼�� */
    grx_buf.len = 0;

    /* ����Ҫ���͵���Ϣ���뷢�Ͷ��� */
    err = OSQPost(gptx_que, (void*)&gtx_buf);
    if(OS_NO_ERR != err)
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
    MSG_BUF* ptx_buf;
    U8 err;

    /* ����Ϣ���Ͷ��л�ȡ��Ҫ���͵���Ϣ */
    ptx_buf = (MSG_BUF*)OSQPend(gptx_que, 0, &err);
    if((NULL == ptx_buf) || (OS_NO_ERR != err))
    {
        return;
    }

    /* �������ڷ����жϷ������� */
    start_to_send_data();
}

/***********************************************************************************
��������: ��ʼ��ϵͳtick��ʱ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void sys_tick_init(void)
{
    /* ��ʼ��ϵͳtick��ʱ��, 10ms���� */
    (void)SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC);
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
            (void)OSSemPost(gprx_sem);
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

