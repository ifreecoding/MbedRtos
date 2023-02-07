
#include "mds_userroot.h"


MSG_BUF gtx_buf[TEST_PROCESS_NUM];  /* 消息发送缓冲 */


/***********************************************************************************
函数功能: 根任务, 用户代码从这个函数开始执行, 优先级最高.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_RootTask(void)
{
    U32 i;

    /* 串口初始化 */
    uart1_init();

    put_string("main process is running!!!\r\n");

    /* 初始化消息发送缓冲, 将标志置为没有消息状态 */
    for(i = 0; i < TEST_PROCESS_NUM; i++)
    {
        gtx_buf[i].flag = MSG_FLAG_NONE;
    }

    /* 设置优先级轮转时间为0.5秒 */
    MDS_TaskTimeSlice(50);

    /* 创建rx_thread线程 */
    (void)MDS_TaskCreate(NULL, rx_thread, NULL, NULL, TASK_STASK_LEN, 2, NULL);

    /* 创建tx_thread线程 */
    (void)MDS_TaskCreate(NULL, tx_thread, NULL, NULL, TASK_STASK_LEN, 3, NULL);
}

/***********************************************************************************
函数功能: 空闲任务, CPU空闲时执行这个任务, 优先级最低.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void MDS_IdleTask(void* pvPara)
{
    while(1)
    {
        ;
    }
}

/***********************************************************************************
函数功能: 消息接收线程.
入口参数: pdata: 线程参数指针.
返 回 值: none.
***********************************************************************************/
void rx_thread(void* pdata)
{
    M_TCB* pprocess_tcb[TEST_PROCESS_NUM] = {NULL, NULL, NULL};
    VFUNC process_addr;
    U32 reset_vector_addr;
    U16 cmd;

    while(1)
    {
        /* 延时0.2秒 */
        MDS_TaskDelay(20);

        /* 没接收到数据 */
        if(RESET == USART_GetFlagStatus(USART1, USART_FLAG_RXNE))
        {
            continue;
        }

        /* 接收数据 */
        cmd = USART_ReceiveData(USART1);

        /* 接收到合法的命令 */
        if((cmd >= '1') && (cmd <= '0' + TEST_PROCESS_NUM))
        {
            /* 换算成十六进制数 */
            cmd -= '1';

            /* 该进程已经执行则直接返回 */
            if(NULL != pprocess_tcb[cmd])
            {
                continue;
            }

            /* 根据接收到的命令确定测试进程的复位向量地址 */
            reset_vector_addr = TEST_PROCESS_ADDR + TEST_PROCESS_MAX_SIZE * cmd
                                + RESET_VECTOR_ADDR;

            /* 根据复位向量获取测试进程的第一条指令 */
            process_addr = (VFUNC)(*(U32*)reset_vector_addr);

            /* 执行对应的测试进程 */
            pprocess_tcb[cmd] = MDS_TaskCreate(NULL, process_addr, NULL, NULL,
                                               TASK_STASK_LEN, 4, NULL);
        }
    }
}

/***********************************************************************************
函数功能: 消息发送线程.
入口参数: pdata: 线程参数指针.
返 回 值: none.
***********************************************************************************/
void tx_thread(void* pdata)
{
    U32 i;

    while(1)
    {
        /* 延时0.2秒 */
        MDS_TaskDelay(20);

        /* 寻找需要发送的缓冲 */
        for(i = 0; i < TEST_PROCESS_NUM; i++)
        {
            if(MSG_FLAG_NEED_SEND == gtx_buf[i].flag)
            {
                /* 发送端口号 */
                put_string("[PORT ");
                put_char('1' + i);
                put_string("]: ");

                /* 发送缓冲中的字符串 */
                put_string(gtx_buf[i].buf);

                /* 置为没有消息标志 */
                gtx_buf[i].flag = MSG_FLAG_NONE;
            }
        }
    }
}

/***********************************************************************************
函数功能: 打印消息, 该函数将消息打印到打印驱动缓冲中, 再由打印驱动打印到串口.
入口参数: port: 打印消息所使用的端口号.
          pstring: 需要打印的字符串指针.
返 回 值: none.
***********************************************************************************/
void print_msg(U32 port, U8* pstring)
{
    /* 复制消息 */
    strcpy(gtx_buf[port - 1].buf, pstring);

    /* 置为有消息标志 */
    gtx_buf[port - 1].flag = MSG_FLAG_NEED_SEND;
}

/***********************************************************************************
函数功能: 初始化串口1.
入口参数: none.
返 回 值: none.
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
函数功能: 向串口发送一个字符.
入口参数: send_char: 输出的字符.
返 回 值: none.
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
函数功能: 向串口发送字符串.
入口参数: pstring: 字符串指针.
返 回 值: none.
***********************************************************************************/
void put_string(U8* pstring)
{
    U32 i;
    U8 send_char;

    i = 0;

    /* 循环发送字符串中的每个字符 */
    while(1)
    {
        /* 获取缓冲中需要发送的数据 */
        send_char = pstring[i++];

        /* 数据未发送完毕 */
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

