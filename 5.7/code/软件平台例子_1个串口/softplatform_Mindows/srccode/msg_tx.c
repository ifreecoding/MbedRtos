
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
    BUF_NODE* pbuf_fill;
    MSG_BUF* pmsg_buf;
    MSG_STR* pmsg_str;

    /* tx任务接收消息 */
    pbuf = tx_task_receive_msg();
    if(NULL == pbuf)
    {
        return;
    }

    /* **********以下需要用户根据通信协议补充该函数********** */

    pmsg_buf = (MSG_BUF*)pbuf;
    pmsg_str = (MSG_STR*)pmsg_buf->buf;

    /* 通信消息需要进行填充处理 */
    if(MSG_TYPE_COM == pmsg_str->head.type)
    {
        /* 填充需要发送的数据包 */
        if(NULL == (pbuf_fill = fill_tx_msg(pbuf)))
        {
            return;
        }
    }
    else
    {
        pbuf_fill = pbuf;
    }

    /* tx任务发送数据 */
    tx_task_send_data(pbuf_fill);

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
    U32 print_level;
    U32 print_module;

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

    if(MSG_TYPE_PRT == pmsg_str->head.type)
    {
        /* 调用buf_free函数前屏蔽打印功能, 防止在buf_free函数内调用print_msg函数形成
           循环打印 */
        print_level = print_level_get();
        print_module = print_module_get();
        print_level_clr(print_level);
        print_module_clr(print_module);

        /* 释放存放消息的缓冲 */
        buf_free(pbuf);

        /* 开启原有的打印级别和模块 */
        print_level_set(print_level);
        print_module_set(print_module);
    }
    else
    {
        /* 释放存放消息的缓冲 */
        buf_free(pbuf);
    }
}

/***********************************************************************************
函数功能: 填充发送的消息.
入口参数: pbuf: 需要发送的消息的buf指针.
返 回 值: 填充后的消息缓冲, 若为NULL则表示填充失败.
***********************************************************************************/
BUF_NODE* fill_tx_msg(BUF_NODE* pbuf)
{
    BUF_NODE* pbuf_node_tran;
    MSG_BUF* pmsg_buf;
    MSG_STR* pmsg_str;
    USR_MSG* pusr_msg;

    pmsg_buf = (MSG_BUF*)pbuf;
    pmsg_str = (MSG_STR*)pmsg_buf->buf;
    pusr_msg = (USR_MSG*)pmsg_str->buf;

    /* 填充用户层消息 */
    pusr_msg->head = 0x7E;                          /* 消息头 */
    pusr_msg->s_addr = LOCAL_ADDR;                  /* 源地址 */
    pusr_msg->d_addr = pmsg_str->head.r_addr;       /* 目的地址 */
    pusr_msg->data[pusr_msg->len] = 0x8E;           /* 消息尾 */

    /* 添加校验和 */
    pusr_msg->crc = calc_msg_crc(&pusr_msg->s_addr,
                                 USR_DATA_LEN_2_USR_CRC_LEN(pusr_msg->len));

    /* 对需要发送的消息进行转义 */
    pbuf_node_tran = tx_message_transfer(pusr_msg);

    /* 释放消息缓冲 */
    buf_free(pbuf);

    return pbuf_node_tran;
}

/***********************************************************************************
函数功能: 对发送的数据进行转义, 消息中出现的特殊字符按如下规则转义: 7D->7D 5D,
          7E->7D 5E, 8E->7D 6E.
入口参数: pbuf: 转义前的消息缓冲.
返 回 值: 转义后的消息缓冲, 若为NULL则表示转义失败.
***********************************************************************************/
BUF_NODE* tx_message_transfer(USR_MSG* pbuf)
{
    BUF_NODE* pbuf_node;
    MSG_BUF* pmsg_buf;
    MSG_STR* pmsg_str;
    U8* pin_buf;
    U8* pout_buf;
    TX_STA tx_status;
    U32 in_len;
    U32 in_cnt;
    U32 out_cnt;

    /* 申请存储转义消息的缓冲 */
    pbuf_node = buf_malloc(BUF_MAX_LEN);
    if(NULL == pbuf_node)
    {
        print_msg(PRINT_IMPORTANT, PRINT_TX, "\r\ncan't malloc buf. (%s, %d)",
                  __FILE__, __LINE__);

        return NULL;
    }

    pmsg_buf = (MSG_BUF*)pbuf_node;
    pmsg_str = (MSG_STR*)pmsg_buf->buf;

    /* 需要转换的数据的指针 */
    pin_buf = (U8*)pbuf;

    /* 转换后存放数据的指针 */
    pout_buf = pmsg_str->buf;

    in_cnt = 0;
    out_cnt = 0;

    /* 需要转换的数据长度 */
    in_len = USR_DATA_LEN_2_USR_TOTAL_LEN(pbuf->len);

    /* 初始化转换状态 */
    tx_status = TX_7E;

    /* 转义整条消息 */
    while(in_cnt < in_len - 1)
    {
        /* 转义的数据超长, 返回失败 */
        if(out_cnt >= GET_USR_BUF_LEN(BUF_MAX_LEN))
        {
            goto RTN;
        }

        if(TX_NORMAL == tx_status)
        {
            /* 需要转义发送7E */
            if(0x7E == pin_buf[in_cnt])
            {
                /* 先发送7D */
                pout_buf[out_cnt++] = 0x7D;

                tx_status = TX_TRANSFER_7E;
            }
            /* 需要转义发送7D */
            else if(0x7D == pin_buf[in_cnt])
            {
                /* 先发送7D */
                pout_buf[out_cnt++] = 0x7D;

                tx_status = TX_TRANSFER_7D;
            }
            /* 需要转义发送8E */
            else if(0x8E == pin_buf[in_cnt])
            {
                /* 先发送7D */
                pout_buf[out_cnt++] = 0x7D;

                tx_status = TX_TRANSFER_8E;
            }
            else /* 发送普通字符 */
            {
                pout_buf[out_cnt++] = pin_buf[in_cnt++];
            }
        }
        else if(TX_7E == tx_status)
        {
            /* 先发送7E头 */
            pout_buf[out_cnt++] = 0x7E;
            in_cnt++;

            tx_status = TX_NORMAL;
        }
        else if(TX_TRANSFER_7E == tx_status)
        {
            /* 7E转义, 发送5E转义字符 */
            pout_buf[out_cnt++] = 0x5E;
            in_cnt++;

            tx_status = TX_NORMAL;
        }
        else if(TX_TRANSFER_7D == tx_status)
        {
            /* 7D转义, 发送5D转义字符 */
            pout_buf[out_cnt++] = 0x5D;
            in_cnt++;

            tx_status = TX_NORMAL;
        }
        else if(TX_TRANSFER_8E == tx_status)
        {
            /* 8E转义, 发送5E转义字符 */
            pout_buf[out_cnt++] = 0x6E;
            in_cnt++;

            tx_status = TX_NORMAL;
        }
    }

    /* 转义的数据超长, 返回失败 */
    if(out_cnt >= GET_USR_BUF_LEN(BUF_MAX_LEN))
    {
        goto RTN;
    }

    /* 填充消息尾 */
    pout_buf[out_cnt++] = 0x8E;
    in_cnt++;

    print_msg(PRINT_SUGGEST, PRINT_TX, "\r\n%d data transfer to %d data. (%s, %d)",
              in_cnt, out_cnt, __FILE__, __LINE__);

    /* 填充维护层参数 */
    pmsg_str->head.len = out_cnt;
    pmsg_str->head.type = MSG_TYPE_COM;

    return pbuf_node;

RTN:

    /* 释放转义消息缓冲 */
    buf_free(pbuf_node);

    pbuf_node = NULL;

    return pbuf_node;
}

