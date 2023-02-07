
#include "wlx_userroot.h"


/***********************************************************************************
函数功能: 根任务, 用户代码从这个函数开始执行.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void WLX_RootTask(void* pvPara)
{
    U32 uiRunTime1;
    U32 uiRunTime2;
    U32 uiRunTime3;

    /* 初始化软件 */
    DEV_SoftwareInit();

    /* 初始化硬件 */
    DEV_HardwareInit();

    /* 创建任务 */
    uiRunTime1 = 1000;
    gpstrTask1Tcb = WLX_TaskCreate(TEST_TestTask1, (void*)&uiRunTime1,
                                   gaucTask1Stack, TASKSTACK);

    uiRunTime2 = 2000;
    gpstrTask2Tcb = WLX_TaskCreate(TEST_TestTask2, (void*)&uiRunTime2,
                                   gaucTask2Stack, TASKSTACK);

    uiRunTime3 = 3000;
    gpstrTask3Tcb = WLX_TaskCreate(TEST_TestTask3, (void*)&uiRunTime3,
                                   gaucTask3Stack, TASKSTACK);

    /* 任务切换 */
    WLX_TaskSwitch(gpstrTask1Tcb);
}

