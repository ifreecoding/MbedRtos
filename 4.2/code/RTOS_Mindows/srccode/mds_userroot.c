
#include "mds_userroot.h"


/***********************************************************************************
函数功能: 根任务, 用户代码从这个函数开始执行, 优先级最高.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_RootTask(void)
{
    /* 初始化软件 */
    DEV_SoftwareInit();

    /* 初始化硬件 */
    DEV_HardwareInit();

    /* 任务打印 */
    DEV_PutString((U8*)"\r\nRootTask is running!");

    /* 任务运行1秒 */
    DEV_DelayMs(1000);

    /* 创建任务 */
    (void)MDS_TaskCreate(TEST_TestTask1, NULL, gaucTask1Stack, TASKSTACK, 4);

    DEV_DelayMs(1000);

    (void)MDS_TaskCreate(TEST_TestTask2, NULL, gaucTask2Stack, TASKSTACK, 3);

    DEV_DelayMs(1000);

    (void)MDS_TaskCreate(TEST_TestTask3, NULL, gaucTask3Stack, TASKSTACK, 1);

    DEV_DelayMs(1000);
}

