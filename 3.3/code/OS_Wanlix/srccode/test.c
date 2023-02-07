
#include "test.h"


U8 gaucTask1Stack[TASKSTACK];       /* 任务1的栈 */
U8 gaucTask2Stack[TASKSTACK];       /* 任务2的栈 */
U8 gaucTask3Stack[TASKSTACK];       /* 任务3的栈 */

W_TCB* gpstrTask1Tcb;               /* 任务1的TCB指针 */
W_TCB* gpstrTask2Tcb;               /* 任务2的TCB指针 */
W_TCB* gpstrTask3Tcb;               /* 任务3的TCB指针 */


/***********************************************************************************
函数功能: 主函数, C程序从此函数开始运行. 该函数创建任务并从非任务状态切换到任务状态
          运行.
入口参数: none.
返 回 值: 返回值为0, 但系统不会运行到该函数返回.
***********************************************************************************/
S32 main(void)
{
    /**************************************************************************/
    /*            更多资料请访问    www.ifreecoding.com                       */
    /*                              bbs.ifreecoding.com                       */
    /*                              blog.sina.com.cn/ifreecoding              */
    /**************************************************************************/

    /* 初始化软件 */
    DEV_SoftwareInit();

    /* 初始化硬件 */
    DEV_HardwareInit();

    /* 创建任务 */
    gpstrTask1Tcb = WLX_TaskCreate(TEST_TestTask1, gaucTask1Stack, TASKSTACK);
    gpstrTask2Tcb = WLX_TaskCreate(TEST_TestTask2, gaucTask2Stack, TASKSTACK);
    gpstrTask3Tcb = WLX_TaskCreate(TEST_TestTask3, gaucTask3Stack, TASKSTACK);

    /* 开始任务调度 */
    WLX_TaskStart(gpstrTask1Tcb);

    return 0;
}

/***********************************************************************************
函数功能: 测试任务切换函数1, 向串口打印任务字符串, 向LCD屏打印任务切换图形.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void TEST_TestTask1(void)
{
    while(1)
    {
        /* 任务打印信息 */
        DEV_TaskPrintMsg(1);

        /* 任务运行1秒 */
        TEST_TaskRun(1000);

        /* 任务切换 */
        WLX_TaskSwitch(gpstrTask3Tcb);
    }
}

/***********************************************************************************
函数功能: 测试任务切换函数2, 向串口打印任务字符串, 向LCD屏打印任务切换图形.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void TEST_TestTask2(void)
{
    while(1)
    {
        /* 任务打印信息 */
        DEV_TaskPrintMsg(2);

        /* 任务运行2秒 */
        TEST_TaskRun(2000);

        /* 任务切换 */
        WLX_TaskSwitch(gpstrTask1Tcb);
    }
}

/***********************************************************************************
函数功能: 测试任务切换函数3, 向串口打印任务字符串, 向LCD屏打印任务切换图形.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void TEST_TestTask3(void)
{
    while(1)
    {
        /* 任务打印信息 */
        DEV_TaskPrintMsg(3);

        /* 任务运行3秒 */
        TEST_TaskRun(3000);

        /* 任务切换 */
        WLX_TaskSwitch(gpstrTask2Tcb);
    }
}

/***********************************************************************************
函数功能: 模拟任务运行函数, 并更新任务运行时间.
入口参数: uiMs: 要延迟的时间, 单位ms.
返 回 值: none.
***********************************************************************************/
void TEST_TaskRun(U32 uiMs)
{
    /* 用延迟时间模拟任务运行时间 */
    DEV_DelayMs(uiMs);

    /* 增加任务运行时间 */
    DEV_AddTaskTime(uiMs);
}

