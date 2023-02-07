
#include "process.h"


/***********************************************************************************
函数功能: 创建process任务并进行初始化.
入口参数: ptask: 任务结构指针..
返 回 值: RTN_SUCD: 成功.
          RTN_FAIL: 失败.
***********************************************************************************/
U32 create_process_task(TASK_STR* ptask)
{
    /* 创建process_task任务 */
    ptask->ptcb = MDS_TaskCreate("process_task", msg_process_task, ptask, NULL,
                                 PROCESS_TASK_STACK, PROCESS_TASK_PRIO, NULL);
    if(NULL == ptask->ptcb)
    {
        print_msg(PRINT_IMPORTANT, PRINT_PROCESS,
                  "\r\ncreate process_task failed. (%s, %d)", __FILE__, __LINE__);

        return RTN_FAIL;
    }

    /* 初始化任务的队列 */
    if(NULL == (ptask->pque = MDS_QueCreate(NULL, QUEFIFO)))
    {
        print_msg(PRINT_IMPORTANT, PRINT_PROCESS,
                  "\r\nprocess_task queue init failed. (%s, %d)",
                  __FILE__, __LINE__);

        return RTN_FAIL;
    }

    print_msg(PRINT_SUGGEST, PRINT_PROCESS,
              "\r\nprocess_task init finished. (%s, %d)", __FILE__, __LINE__);

    return RTN_SUCD;
}

/***********************************************************************************
函数功能: 消息处理任务, 从队列获取消息进行处理.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void msg_process_task(void* pvPara)
{
    while(1)
    {
        /* 处理消息 */
        msg_process((TASK_STR*)pvPara);
    }
}

/***********************************************************************************
函数功能: 处理接收到的消息.
入口参数: ptask: 任务结构指针.
返 回 值: none.
***********************************************************************************/
void msg_process(TASK_STR* ptask)
{
    BUF_NODE* pbuf;

    /* process任务接收消息 */
    pbuf = process_task_receive_msg(ptask);
    if(NULL == pbuf)
    {
        return;
    }

    /* 处理一条消息 */
    process_one_msg(pbuf);

    /* 消息处理完毕, 将process任务释放到任务池 */
    process_task_free(ptask);
}

/***********************************************************************************
函数功能: 向process任务发送消息.
入口参数: ptask: 任务结构指针.
          pbuf: 发送消息的buf.
返 回 值: none.
***********************************************************************************/
void send_msg_to_process_task(TASK_STR* ptask, BUF_NODE* pbuf)
{
    /* 将消息放入队列 */
    if(RTN_SUCD != MDS_QuePut(ptask->pque, &pbuf->list))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TX,
                  "\r\n%s put message to process_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return;
    }
}

/***********************************************************************************
函数功能: process任务接收消息.
入口参数: ptask: 任务结构指针.
返 回 值: 接收到的消息的buf指针, 如果为NULL代表出错.
***********************************************************************************/
BUF_NODE* process_task_receive_msg(TASK_STR* ptask)
{
    M_DLIST* pbuf;

    /* 从队列中获取消息 */
    if(RTN_SUCD != MDS_QueGet(ptask->pque, &pbuf, QUEWAITFEV))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TX,
                  "\r\n%s get message from process_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return NULL;
    }

    return (BUF_NODE*)pbuf;
}

/***********************************************************************************
函数功能: 处理一条消息.
入口参数: pbuf: 需要处理的消息的buf指针.
返 回 值: none.
***********************************************************************************/
void process_one_msg(BUF_NODE* pbuf)
{
    /* **********用户需要根据自身需求修改该函数********** */

    BUF_NODE* pbuf_rtn;
    MSG_BUF* pmsg_buf;
    MSG_BUF* pmsg_buf_rtn;
    MSG_STR* pmsg_str;
    MSG_STR* pmsg_str_rtn;
    USR_MSG* pusr_msg;

    pmsg_buf = (MSG_BUF*)pbuf;
    pmsg_str = (MSG_STR*)pmsg_buf->buf;
    pusr_msg = (USR_MSG*)pmsg_str->buf;

    /* 处理不同的命令 */
    switch(pusr_msg->cmd)
    {
        case TEST_CMD1:

            pbuf_rtn = test_cmd1(pusr_msg);

            break;

        case TEST_CMD2:

            pbuf_rtn = test_cmd2(pusr_msg);

            break;

        default:

            pbuf_rtn = NULL;
    }

    /* 不需要回复消息 */
    if(NULL == pbuf_rtn)
    {
        goto RTN;
    }

    /* 填充接收到的消息的地址 */
    pmsg_buf_rtn = (MSG_BUF*)pbuf_rtn;
    pmsg_str_rtn = (MSG_STR*)pmsg_buf_rtn->buf;

    /* 填充维护层参数 */
    pmsg_str_rtn->head.r_addr = pusr_msg->s_addr;
    pmsg_str_rtn->head.type = MSG_TYPE_COM;

    /* 将返回的消息发送给tx任务 */
    send_msg_to_tx_task(pbuf_rtn);

RTN:

    /* 释放接收的消息缓冲 */
    buf_free(pbuf);
}

/***********************************************************************************
函数功能: 测试命令1, 在LCD上显示字符串, 持续10秒钟.
入口参数: pbuf: 需要处理的消息的buf指针.
返 回 值: 生成回复消息的缓冲地址, 如果不需要回复则为NULL.
***********************************************************************************/
BUF_NODE* test_cmd1(USR_MSG* pbuf)
{
    BUF_NODE* pbuf_node;
    MSG_BUF* pmsg_buf;
    MSG_STR* pmsg_str;
    USR_MSG* pusr_msg;
    U32 i;

    /* 执行命令 */

    /* 申请存储回复消息的缓冲 */
    pbuf_node = buf_malloc(BUF_MAX_LEN);
    if(NULL == pbuf_node)
    {
        print_msg(PRINT_IMPORTANT, PRINT_PROCESS, "\r\ncan't malloc buf. (%s, %d)",
                  __FILE__, __LINE__);

        return NULL;
    }

    /* 显示10次字符串 */
    for(i = 0; i < 10; i++)
    {
        /* 显示消息中的字符串 */
        display_string(pbuf->data, pbuf->len, i * 16, i * 8);

        MDS_TaskDelay(100);
    }

    pmsg_buf = (MSG_BUF*)pbuf_node;
    pmsg_str = (MSG_STR*)pmsg_buf->buf;
    pusr_msg = (USR_MSG*)pmsg_str->buf;

    /* 填充回复消息 */
    pusr_msg->cmd = pbuf->cmd;
    pusr_msg->len = 1;
    pusr_msg->data[0] = CMD_OK;

    return pbuf_node;
}

/***********************************************************************************
函数功能: 测试命令2, 改变LCD上显示字符串的颜色.
入口参数: pbuf: 需要处理的消息的buf指针.
返 回 值: 生成回复消息的缓冲地址, 如果不需要回复则为NULL.
***********************************************************************************/
BUF_NODE* test_cmd2(USR_MSG* pbuf)
{
    BUF_NODE* pbuf_node;
    MSG_BUF* pmsg_buf;
    MSG_STR* pmsg_str;
    USR_MSG* pusr_msg;

    /* 执行命令 */

    /* 申请存储回复消息的缓冲 */
    pbuf_node = buf_malloc(BUF_MAX_LEN);
    if(NULL == pbuf_node)
    {
        print_msg(PRINT_IMPORTANT, PRINT_PROCESS, "\r\ncan't malloc buf. (%s, %d)",
                  __FILE__, __LINE__);

        return NULL;
    }

    /* 设置画笔颜色 */
    set_pen_color(pbuf->data[0] | (U16)pbuf->data[1] << 8);

    pmsg_buf = (MSG_BUF*)pbuf_node;
    pmsg_str = (MSG_STR*)pmsg_buf->buf;
    pusr_msg = (USR_MSG*)pmsg_str->buf;

    /* 填充回复消息 */
    pusr_msg->cmd = pbuf->cmd;
    pusr_msg->len = 1;
    pusr_msg->data[0] = CMD_OK;

    return pbuf_node;
}

