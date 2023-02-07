
#include "mds_userroot.h"


MSG_BUF gtx_buf[TEST_PROCESS_NUM];  /* ��Ϣ���ͻ��� */


/***********************************************************************************
��������: ������, �û���������������ʼִ��, ���ȼ����.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_RootTask(void)
{
    U32 i;

    /* ���ڳ�ʼ�� */
    uart1_init();

    put_string("main process is running!!!\r\n");

    /* ��ʼ����Ϣ���ͻ���, ����־��Ϊû����Ϣ״̬ */
    for(i = 0; i < TEST_PROCESS_NUM; i++)
    {
        gtx_buf[i].flag = MSG_FLAG_NONE;
    }

    /* �������ȼ���תʱ��Ϊ0.5�� */
    MDS_TaskTimeSlice(50);

    /* ����rx_thread�߳� */
    (void)MDS_TaskCreate(NULL, rx_thread, NULL, NULL, TASK_STASK_LEN, 2, NULL);

    /* ����tx_thread�߳� */
    (void)MDS_TaskCreate(NULL, tx_thread, NULL, NULL, TASK_STASK_LEN, 3, NULL);
}

/***********************************************************************************
��������: ��������, CPU����ʱִ���������, ���ȼ����.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void MDS_IdleTask(void* pvPara)
{
    while(1)
    {
        ;
    }
}

/***********************************************************************************
��������: ��Ϣ�����߳�.
��ڲ���: pdata: �̲߳���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void rx_thread(void* pdata)
{
    M_TCB* pprocess_tcb[TEST_PROCESS_NUM] = {NULL, NULL, NULL};
    VFUNC process_addr;
    U32 reset_vector_addr;
    U16 cmd;

    while(1)
    {
        /* ��ʱ0.2�� */
        MDS_TaskDelay(20);

        /* û���յ����� */
        if(RESET == USART_GetFlagStatus(USART1, USART_FLAG_RXNE))
        {
            continue;
        }

        /* �������� */
        cmd = USART_ReceiveData(USART1);

        /* ���յ��Ϸ������� */
        if((cmd >= '1') && (cmd <= '0' + TEST_PROCESS_NUM))
        {
            /* �����ʮ�������� */
            cmd -= '1';

            /* �ý����Ѿ�ִ����ֱ�ӷ��� */
            if(NULL != pprocess_tcb[cmd])
            {
                continue;
            }

            /* ���ݽ��յ�������ȷ�����Խ��̵ĸ�λ������ַ */
            reset_vector_addr = TEST_PROCESS_ADDR + TEST_PROCESS_MAX_SIZE * cmd
                                + RESET_VECTOR_ADDR;

            /* ���ݸ�λ������ȡ���Խ��̵ĵ�һ��ָ�� */
            process_addr = (VFUNC)(*(U32*)reset_vector_addr);

            /* ִ�ж�Ӧ�Ĳ��Խ��� */
            pprocess_tcb[cmd] = MDS_TaskCreate(NULL, process_addr, NULL, NULL,
                                               TASK_STASK_LEN, 4, NULL);
        }
    }
}

/***********************************************************************************
��������: ��Ϣ�����߳�.
��ڲ���: pdata: �̲߳���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void tx_thread(void* pdata)
{
    U32 i;

    while(1)
    {
        /* ��ʱ0.2�� */
        MDS_TaskDelay(20);

        /* Ѱ����Ҫ���͵Ļ��� */
        for(i = 0; i < TEST_PROCESS_NUM; i++)
        {
            if(MSG_FLAG_NEED_SEND == gtx_buf[i].flag)
            {
                /* ���Ͷ˿ں� */
                put_string("[PORT ");
                put_char('1' + i);
                put_string("]: ");

                /* ���ͻ����е��ַ��� */
                put_string(gtx_buf[i].buf);

                /* ��Ϊû����Ϣ��־ */
                gtx_buf[i].flag = MSG_FLAG_NONE;
            }
        }
    }
}

/***********************************************************************************
��������: ��ӡ��Ϣ, �ú�������Ϣ��ӡ����ӡ����������, ���ɴ�ӡ������ӡ������.
��ڲ���: port: ��ӡ��Ϣ��ʹ�õĶ˿ں�.
          pstring: ��Ҫ��ӡ���ַ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void print_msg(U32 port, U8* pstring)
{
    /* ������Ϣ */
    strcpy(gtx_buf[port - 1].buf, pstring);

    /* ��Ϊ����Ϣ��־ */
    gtx_buf[port - 1].flag = MSG_FLAG_NEED_SEND;
}

/***********************************************************************************
��������: ��ʼ������1.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void uart1_init(void)
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
��ڲ���: send_char: ������ַ�.
�� �� ֵ: none.
***********************************************************************************/
void put_char(U8 send_char)
{
    /* Loop until USARTy DR register is empty */
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
        ;
    }

    USART_SendData(USART1, send_char);
}

/***********************************************************************************
��������: �򴮿ڷ����ַ���.
��ڲ���: pstring: �ַ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void put_string(U8* pstring)
{
    U32 i;
    U8 send_char;

    i = 0;

    /* ѭ�������ַ����е�ÿ���ַ� */
    while(1)
    {
        /* ��ȡ��������Ҫ���͵����� */
        send_char = pstring[i++];

        /* ����δ������� */
        if(0 != send_char)
        {
            put_char(send_char);
        }
        else
        {
            break;
        }
    }
}

