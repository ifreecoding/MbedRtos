
#include "wlx_userroot.h"


/***********************************************************************************
函数功能: 根任务, 用户代码从这个函数开始执行.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void WLX_RootTask(void)
{
    /* 初始化软件 */
    DEV_SoftwareInit();

    /* 初始化硬件 */
    DEV_HardwareInit();

    /* 创建任务 */
    gpstrTask1Tcb = WLX_TaskCreate(TEST_TestTask1, gaucTask1Stack, TASKSTACK);
    gpstrTask2Tcb = WLX_TaskCreate(TEST_TestTask2, gaucTask2Stack, TASKSTACK);
    gpstrTask3Tcb = WLX_TaskCreate(TEST_TestTask3, gaucTask3Stack, TASKSTACK);

    /* 任务切换 */
    WLX_TaskSwitch(gpstrTask1Tcb);
}

