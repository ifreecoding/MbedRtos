
#include "mds_userroot.h"
#include "test.h"


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

    /* 创建刷屏任务 */
    (void)MDS_TaskCreate("FlushScn", TEST_FlushScnTask, NULL, NULL, FLUAHSCNSTK, 2, NULL);

    /* 创建按键任务 */
    (void)MDS_TaskCreate("Key", TEST_KeyTask, NULL, NULL, KEYSTK, 4, NULL);

    /* 创建按键响应任务 */
    (void)MDS_TaskCreate("Process", TEST_ProcessTask, NULL, NULL, PROSTK, 3, NULL);

    /* 创建串口打印任务 */
    gpstrSerialTaskTcb = MDS_TaskCreate("SrlPrt", TEST_SerialPrintTask, NULL, NULL, PRINTSTK, 5, NULL);
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

