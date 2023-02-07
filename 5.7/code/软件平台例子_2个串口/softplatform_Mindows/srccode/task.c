
#include <stdlib.h>
#include "task.h"


TASK_POOL gtask_pool;           /* 任务池 */


/***********************************************************************************
函数功能: 创建软件平台所使用的任务.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void task_create(void)
{
    /**************************************************************************/
    /*            更多资料请访问    www.ifreecoding.com                       */
    /*                              bbs.ifreecoding.com                       */
    /*                              blog.sina.com.cn/ifreecoding              */
    /**************************************************************************/

    /* 创建print_task任务, 需要先创建print任务, 为其它任务提供打印功能 */
    create_print_task();

    /* 创建sched_task任务 */
    create_sched_task();

    /* 创建rx_task任务 */
    create_rx_task();

    /* 创建tx_task任务 */
    create_tx_task();

    /* process任务在任务池调度时创建, 此处只初始化任务池 */
    process_task_pool_init();
}

/***********************************************************************************
函数功能: 初始化任务池.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void process_task_pool_init(void)
{
    /* 初始化任务池信号量 */
    if(NULL == (gtask_pool.psem = MDS_SemCreate(NULL, SEMCNT | SEMPRIO, SEMFULL)))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TASK,
                  "\r\ntask_pool sem init failed. (%s, %d)", __FILE__, __LINE__);
    }

    /* 初始化任务池链表 */
    slist_init(&gtask_pool.list);

    /* 初始化任务池统计值 */
    gtask_pool.s_num = 0;
    gtask_pool.d_num = 0;
}

/***********************************************************************************
函数功能: 从任务池中申请一个任务. 若任务池中有空闲任务则从任务池中申请一个任务, 若任
          务池中没有空闲任务则创建一个新的任务.
入口参数: none.
返 回 值: 申请到的任务结构指针, 若申请不到任务则返回NULL.
***********************************************************************************/
TASK_STR* process_task_malloc(void)
{
    TASK_STR* ptask;

    /* 获取信号量, 保证任务池链表操作的串行性 */
    if(RTN_SUCD != MDS_SemTake(gtask_pool.psem, SEMWAITFEV))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TASK,
                  "\r\nprocess_task wait sem failed. (%d, %s)", __LINE__, __FILE__);

        return NULL;
    }

    /* 从任务池链表拆除一个任务节点 */
    ptask = (TASK_STR*)slist_node_delete(&gtask_pool.list);

    /* 从链表上申请到任务 */
    if(NULL != ptask)
    {
        /* 增加动态申请任务的计数统计 */
        gtask_pool.d_num++;

        print_msg(PRINT_SUGGEST, PRINT_TASK,
                  "\r\nmalloc 0x%x process_task successfully. (%s, %d)", ptask,
                  __FILE__, __LINE__);
    }
    /* 如果链表上没有可用的任务则新创建一个任务 */
    else
    {
        /* 申请任务结构 */
        ptask = malloc(sizeof(TASK_STR));

        /*  从系统申请内存成功 */
        if(NULL != ptask)
        {
            print_msg(PRINT_SUGGEST, PRINT_TASK,
                      "\r\nmalloc 0x%x process_task from system successfully."
                      " (%s, %d)", ptask, __FILE__, __LINE__);
        }
        else /* 从系统申请内存失败 */
        {
            print_msg(PRINT_IMPORTANT, PRINT_TASK,
                      "\r\nmalloc %d bytes from system failed. (%s, %d)",
                      sizeof(TASK_STR), __FILE__, __LINE__);

            goto RTN;
        }

        /* 创建process任务 */
        if(RTN_SUCD != create_process_task(ptask))
        {
            /* 释放申请的任务结构缓冲 */
            free(ptask);

            goto RTN;
        }
        else /* 创建任务成功 */
        {
            /* 增加静态创建任务的计数统计 */
            gtask_pool.s_num++;

            /* 增加动态申请任务的计数统计 */
            gtask_pool.d_num++;
        }
    }

RTN:

    /* 释放信号量 */
    if(RTN_SUCD != MDS_SemGive(gtask_pool.psem))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TASK,
                  "\r\nprocess_task post sem failed. (%d, %s)", __LINE__, __FILE__);

        return NULL;
    }

    return ptask;
}

/***********************************************************************************
函数功能: 释放process任务. 将process任务挂入任务池链表, 供下次申请使用.
入口参数: ptask: 释放的任务结构指针.
返 回 值: none.
***********************************************************************************/
void process_task_free(TASK_STR* ptask)
{
    /* 获取信号量, 保证任务池链表操作的串行性 */
    if(RTN_SUCD != MDS_SemTake(gtask_pool.psem, SEMWAITFEV))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TASK,
                  "\r\nprocess_task wait sem failed. (%d, %s)", __LINE__, __FILE__);

        return;
    }

    /* 将释放的任务加入到任务链表中 */
    slist_node_add(&gtask_pool.list, &ptask->list);

    gtask_pool.d_num--;

    /* 释放信号量 */
    if(RTN_SUCD != MDS_SemGive(gtask_pool.psem))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TASK,
                  "\r\nprocess_task post sem failed. (%d, %s)", __LINE__, __FILE__);

        return;
    }

    print_msg(PRINT_SUGGEST, PRINT_TASK,
              "\r\nfree process_task 0x%x to taskpool successfully. (%s, %d)",
              ptask, __FILE__, __LINE__);
}

