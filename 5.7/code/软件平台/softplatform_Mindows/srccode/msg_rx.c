
#include "msg_rx.h"


TASK_STR grx_task;              /* rx任务结构 */


/***********************************************************************************
函数功能: 创建rx任务并进行初始化.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void create_rx_task(void)
{
    /* 创建rx任务 */
    grx_task.ptcb = MDS_TaskCreate("rx_task", msg_rx_task, NULL, NULL,
                                   RX_TASK_STACK, RX_TASK_PRIO, NULL);
    if(NULL == grx_task.ptcb)
    {
        print_msg(PRINT_IMPORTANT, PRINT_RX, "\r\ncreate rx_task failed. (%s, %d)",
                  __FILE__, __LINE__);
    }

    /* 初始化任务的队列 */
    if(NULL == (grx_task.pque = MDS_QueCreate(NULL, QUEFIFO)))
    {
        print_msg(PRINT_IMPORTANT, PRINT_RX,
                  "\r\nrx_task queue init failed. (%s, %d)", __FILE__, __LINE__);
    }

    print_msg(PRINT_SUGGEST, PRINT_RX, "\r\nrx_task init finished. (%s, %d)",
              __FILE__, __LINE__);
}

/***********************************************************************************
函数功能: 消息接收任务, 从环形接收缓冲区中获取数据进行组包, 将组好包的数据通过队列发
          送给sched任务进行处理.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void msg_rx_task(void* pvPara)
{
    while(1)
    {
        /* 接收数据 */
        msg_rx();
    }
}

/***********************************************************************************
函数功能: 接收消息.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void msg_rx(void)
{
    static MSG_BUF* psrx_buf = NULL;
    static MSG_BUF* pspacket_buf = NULL;
    static U32 msg_len = 0;
    MSG_STR* ppacket_msg;
    U32 packet_len_in_rx_msg;
    U32 recv_len;
    U32 used_len;
    U32 rtn;

    /* 若没有缓冲在接收数据则申请缓冲, 永不释放, 循环利用 */
    if(NULL == psrx_buf)
    {
        /* 申请接收数据的缓冲 */
        psrx_buf = (MSG_BUF*)buf_malloc(BUF_MAX_LEN);
        if(NULL == psrx_buf)
        {
            print_msg(PRINT_IMPORTANT, PRINT_RX, "\r\ncan't malloc buf. (%s, %d)",
                      __FILE__, __LINE__);

            return;
        }
    }

    /* 若没有缓冲在组包数据则申请缓冲 */
    if(NULL == pspacket_buf)
    {
        /* 申请接收数据的缓冲 */
        pspacket_buf = (MSG_BUF*)buf_malloc(BUF_MAX_LEN);
        if(NULL == pspacket_buf)
        {
            print_msg(PRINT_IMPORTANT, PRINT_RX, "\r\ncan't malloc buf. (%s, %d)",
                      __FILE__, __LINE__);

            return;
        }
    }

    /* 读取接收到的数据 */
    recv_len = data_receive(psrx_buf->buf, BUF_MAX_LEN);

    packet_len_in_rx_msg = 0;

    /* 接收到的数据可能会组成多帧数据, 需要循环组包 */
    while(0 != recv_len)
    {
        /* 将消息结构指向消息缓冲 */
        ppacket_msg = (MSG_STR*)pspacket_buf->buf;

        /* 对接收的数据进行组包, 该函数需要用户根据通信协议进行改写 */
        rtn = rx_message_packet(psrx_buf->buf + packet_len_in_rx_msg, ppacket_msg,
                                recv_len, &used_len);
        msg_len += used_len;                /* 已组包的长度 */
        packet_len_in_rx_msg += used_len;   /* 接收的数据中已组包的长度 */
        recv_len -= used_len;               /* 减去已经用去的字节数 */

        /* 一包数据接收完毕 */
        if(RTN_SUCD == rtn)
        {
            /* **********以下需要用户根据通信协议补充该函数********** */





            /* **********以上需要用户根据通信协议补充该函数********** */

            /* 将存放消息的缓冲挂入队列发送给sched任务 */
            send_msg_to_sched_task(&pspacket_buf->node);

            /* 重置组包长度 */
            msg_len = 0;

            /* 所有数据组包完毕 */
            if(0 == recv_len)
            {
                /* 组包缓冲置为NULL, 下次调用该函数时申请缓冲 */
                pspacket_buf = NULL;
            }
            /* 仍有剩余的字节没有组包, 则需要申请新的内存开始组包 */
            else
            {
                print_msg(PRINT_NORMAL, PRINT_RX,
                          "\r\nthere are still %d byte need packet. (%s, %d)",
                          recv_len, __FILE__, __LINE__);

                /* 申请组包数据存放的缓冲 */
                pspacket_buf = (MSG_BUF*)buf_malloc(BUF_MAX_LEN);
                if(NULL == pspacket_buf)
                {
                    print_msg(PRINT_IMPORTANT, PRINT_RX,
                              "\r\ncan't malloc buf. (%s, %d)", __FILE__, __LINE__);

                    return;
                }
            }
        }
    }
}

/***********************************************************************************
函数功能: 向rx任务发送消息.
入口参数: pbuf: 发送消息的buf.
返 回 值: none.
***********************************************************************************/
void send_msg_to_rx_task(BUF_NODE* pbuf)
{
    /* 将消息放入队列 */
    if(RTN_SUCD != MDS_QuePut(grx_task.pque, &pbuf->list))
    {
        print_msg(PRINT_IMPORTANT, PRINT_RX,
                  "\r\n%s put message to rx_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return;
    }
}

/***********************************************************************************
函数功能: rx任务接收消息.
入口参数: none.
返 回 值: 接收到的消息的buf指针, 如果为NULL代表出错.
***********************************************************************************/
BUF_NODE* rx_task_receive_msg(void)
{
    M_DLIST* pbuf;

    /* 从队列中获取消息 */
    if(RTN_SUCD != MDS_QueGet(grx_task.pque, &pbuf, QUEWAITFEV))
    {
        print_msg(PRINT_IMPORTANT, PRINT_RX,
                  "\r\n%s get message from rx_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return NULL;
    }

    return (BUF_NODE*)pbuf;
}

/***********************************************************************************
函数功能: 对接收到的数据进行组包, 并将组好包的消息发送给相关流程. 该函数具有记忆功能
          可重复调用, 每次继续上次组包的数据继续组包.
入口参数: pbuf: 需要组包的数据所在的缓冲.
          ppacket_buf: 组包后的消息存放的缓冲.
          data_len: 需要组包的数据长度.
          pused_len: 返回组包已经用掉的数据长度.
返 回 值: RTN_SUCD: 消息组包完毕, 生成一条完整消息, 可发给相关任务处理.
          RTN_FAIL: 消息未组包完毕, 未生成一条完整消息, 需要继续解析.
***********************************************************************************/
U32 rx_message_packet(U8* pbuf, MSG_STR* ppacket_buf, U32 data_len, U32* pused_len)
{
    /* **********以下需要用户根据通信协议补充该函数********** */

    *pused_len = 0;

    return RTN_SUCD;

    /* **********以上需要用户根据通信协议补充该函数********** */
}

