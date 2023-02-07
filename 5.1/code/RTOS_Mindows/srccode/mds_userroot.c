
#include "mds_userroot.h"


/***********************************************************************************
函数功能: 根任务, 用户代码从这个函数开始执行, 优先级最高.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_RootTask(void)
{
    M_TASKOPT strTaskOpt;

    /* 初始化软件 */
    DEV_SoftwareInit();

    /* 初始化硬件 */
    DEV_HardwareInit();

    /* 创建任务1 */
    (void)MDS_TaskCreate("Test1", TEST_TestTask1, NULL, NULL, TASKSTACK, 4, NULL);

    strTaskOpt.ucTaskSta = TASKDELAY;
    strTaskOpt.uiDelayTick = 1000;

    /* 创建任务2, 延迟10秒后运行 */
    (void)MDS_TaskCreate("Test2", TEST_TestTask2, NULL, NULL, TASKSTACK, 3,
                         &strTaskOpt);

    strTaskOpt.ucTaskSta = TASKDELAY;
    strTaskOpt.uiDelayTick = 200;

    /* 创建任务3, 延迟2秒后运行 */
    (void)MDS_TaskCreate("Test3", TEST_TestTask3, NULL, NULL, TASKSTACK, 2,
                         &strTaskOpt);

    /* 创建串口打印任务 */
    gpstrSerialTaskTcb = MDS_TaskCreate("SrlPrt", TEST_SerialPrintTask, NULL, NULL,
                                        TASKSTACK, 5, NULL);

    /* 创建LCD打印任务 */
    gpstrLcdTaskTcb = MDS_TaskCreate("LcdPrt", TEST_LcdPrintTask, NULL, NULL,
                                     LCDTASKSTACK, 6, NULL);
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

