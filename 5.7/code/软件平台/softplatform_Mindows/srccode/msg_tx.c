
#include "msg_tx.h"


TASK_STR gtx_task;              /* tx任务结构 */


/***********************************************************************************
函数功能: 创建tx任务并进行初始化.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void create_tx_task(void)
{
    /* 创建tx任务 */
    gtx_task.ptcb = MDS_TaskCreate("tx_task", msg_tx_task, NULL, NULL,
                                   TX_TASK_STACK, TX_TASK_PRIO, NULL);
    if(NULL == gtx_task.ptcb)
    {
        print_msg(PRINT_IMPORTANT, PRINT_TX, "\r\ncreate tx_task failed. (%s, %d)",
                  __FILE__, __LINE__);
    }

    /* 初始化任务的队列 */
    if(NULL == (gtx_task.pque = MDS_QueCreate(NULL, QUEFIFO)))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TX,
                  "\r\ntx_task queue init failed. (%s, %d)", __FILE__, __LINE__);
    }

    print_msg(PRINT_SUGGEST, PRINT_TX, "\r\ntx_task init finished. (%s, %d)",
              __FILE__, __LINE__);
}

/***********************************************************************************
函数功能: 消息发送任务, 将需要发送数据拆包并发送到环形发送缓冲区内, 由硬件将环形发送
          缓冲区内的数据发送出去.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void msg_tx_task(void* pvPara)
{
    while(1)
    {
        /* 发送数据 */
        msg_tx();
    }
}

/***********************************************************************************
函数功能: 发送消息.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void msg_tx(void)
{
    BUF_NODE* pbuf;

    /* tx任务接收消息 */
    pbuf = tx_task_receive_msg();
    if(NULL == pbuf)
    {
        return;
    }

    /* **********以下需要用户根据通信协议补充该函数********** */



    /* tx任务发送数据 */
    tx_task_send_data(pbuf);

    /* **********以上需要用户根据通信协议补充该函数********** */
}

/***********************************************************************************
函数功能: 向tx任务发送消息.
入口参数: pbuf: 发送消息的buf.
返 回 值: none.
***********************************************************************************/
void send_msg_to_tx_task(BUF_NODE* pbuf)
{
    /* 将消息放入队列 */
    if(RTN_SUCD != MDS_QuePut(gtx_task.pque, &pbuf->list))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TX,
                  "\r\n%s put message to tx_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return;
    }
}

/***********************************************************************************
函数功能: tx任务接收消息.
入口参数: none.
返 回 值: 接收到的消息的buf指针, 如果为NULL代表出错.
***********************************************************************************/
BUF_NODE* tx_task_receive_msg(void)
{
    M_DLIST* pbuf;

    /* 从队列中获取消息 */
    if(RTN_SUCD != MDS_QueGet(gtx_task.pque, &pbuf, QUEWAITFEV))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TX,
                  "\r\n%s get message from tx_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return NULL;
    }

    return (BUF_NODE*)pbuf;
}

/***********************************************************************************
函数功能: tx任务发送接收到的消息.
入口参数: pbuf: 需要发送的消息的buf指针.
返 回 值: none.
***********************************************************************************/
void tx_task_send_data(BUF_NODE* pbuf)
{
    MSG_BUF* pmsg_buf;
    MSG_STR* pmsg_str;
    U32 sent_len;
    U32 send_len;
    U32 len;

    pmsg_buf = (MSG_BUF*)pbuf;
    pmsg_str = (MSG_STR*)pmsg_buf->buf;

    sent_len = 0;
    send_len = pmsg_str->head.len;

    /* 如果不能一次发送完所有数据则循环发送数据 */
    while(sent_len < send_len)
    {
        /* 发送数据 */
        len = data_send(pmsg_str->buf + sent_len, send_len - sent_len);
        sent_len += len;
    }

    /* 释放存放消息的缓冲 */
    buf_free(pbuf);
}

