
#include "mds_userroot.h"


/***********************************************************************************
函数功能: 根任务, 用户代码从这个函数开始执行, 优先级最高.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_RootTask(void)
{
    M_TASKOPT strOption;

    /* 初始化软件 */
    DEV_SoftwareInit();

    /* 初始化硬件 */
    DEV_HardwareInit();

    /* 使用option参数创建ready状态的任务1 */
    strOption.ucTaskSta = TASKREADY;
    (void)MDS_TaskCreate("Test1", TEST_TestTask1, NULL, gaucTask1Stack, TASKSTACK,
                         4, &strOption);

    /* 不使用option参数创建任务2 */
    (void)MDS_TaskCreate("Test2", TEST_TestTask2, NULL, gaucTask2Stack, TASKSTACK,
                         3, NULL);

    /* 使用option参数创建延迟20秒的delay状态的任务3 */
    strOption.ucTaskSta = TASKDELAY;
    strOption.uiDelayTick = 2000;
    (void)MDS_TaskCreate("Test3", TEST_TestTask3, NULL, gaucTask3Stack, TASKSTACK,
                         2, &strOption);

    /* 创建串口打印任务 */
    gpstrSerialTaskTcb = MDS_TaskCreate("SrlPrt", TEST_SerialPrintTask, NULL,
                                        gaucTaskSrlStack, TASKSTACK, 5, NULL);

    /* 创建LCD打印任务 */
    gpstrLcdTaskTcb = MDS_TaskCreate("LcdPrt", TEST_LcdPrintTask, NULL,
                                     gaucTaskLcdStack, LCDTASKSTACK, 6, NULL);

    (void)MDS_TaskDelay(10000);

    /* 系统运行100秒后root任务自动结束 */

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

