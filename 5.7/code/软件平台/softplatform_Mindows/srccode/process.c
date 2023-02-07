
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


    /* 由用户补充消息的处理过程 */

    /* 由用户生成需要发送的消息 */

    /* 将生成的消息发送给tx任务 */
    //send_msg_to_tx_task();

    /* 释放接收的消息缓冲 */
    buf_free(pbuf);
}

