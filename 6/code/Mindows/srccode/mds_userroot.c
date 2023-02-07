
#include "mds_userroot.h"


/***********************************************************************************
函数功能: 根任务, 用户代码从这个函数开始执行, 优先级最高.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_RootTask(void)
{
    /* 串口初始化 */
    uart1_init();

    /* 初始化接收缓冲 */
    grx_buf.len = 0;

    /* 创建信号量 */
    gprx_sem = MDS_SemCreate(NULL, SEMBIN | SEMPRIO, SEMEMPTY);

    /* 创建队列 */
    gprx_que = MDS_QueCreate(NULL, QUEPRIO);
    gptx_que = MDS_QueCreate(NULL, QUEPRIO);

    /* 创建rx_thread任务 */
    (void)MDS_TaskCreate(NULL, rx_thread, NULL, NULL, TASK_STASK_LEN, 2, NULL);

    /* 创建handle_thread任务 */
    (void)MDS_TaskCreate(NULL, handle_thread, NULL, NULL, TASK_STASK_LEN, 3, NULL);

    /* 创建tx_thread任务 */
    (void)MDS_TaskCreate(NULL, tx_thread, NULL, NULL, TASK_STASK_LEN, 4, NULL);
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

