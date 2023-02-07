
#include "test.h"


M_QUE gstrSerialMsgQue;                 /* 串口打印消息队列指针 */

M_TCB* gpstrSerialTaskTcb;              /* 串口打印任务TCB指针 */

U8 gaucTaskSrlStack[TASKSTACK];         /* 串口打印任务的栈 */
U8 gaucTask1Stack[TASKSTACK];           /* 任务1的栈 */
U8 gaucTask2Stack[TASKSTACK];           /* 任务2的栈 */
U8 gaucTask3Stack[TASKSTACK];           /* 任务3的栈 */
U8 gaucTask4Stack[TASKSTACK];           /* 任务4的栈 */

M_TCB* gpstrTask4Tcb;                   /* task4任务TCB指针 */


/***********************************************************************************
函数功能: 测试任务切换函数1, 向内存打印字符串.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void TEST_TestTask1(void* pvPara)
{
    while(1)
    {
        /* 任务打印 */
        DEV_PutStrToMem((U8*)"\r\nTask1 is running! Tick is: %d",
                        MDS_GetSystemTick());

        /* 任务运行1秒 */
        TEST_TaskRun(1000);

        /* 任务延迟1秒 */
        (void)MDS_TaskDelay(100);
    }
}

/***********************************************************************************
函数功能: 测试任务切换函数2, 向内存打印字符串.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void TEST_TestTask2(void* pvPara)
{
    while(1)
    {
        /* 任务打印 */
        DEV_PutStrToMem((U8*)"\r\nTask2 is running! Tick is: %d",
                        MDS_GetSystemTick());

        /* 任务运行2秒 */
        TEST_TaskRun(2000);

        /* 任务延迟1.5秒 */
        (void)MDS_TaskDelay(150);
    }
}

/***********************************************************************************
函数功能: 测试任务切换函数3, 向内存打印字符串.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void TEST_TestTask3(void* pvPara)
{
    /* 任务打印 */
    DEV_PutStrToMem((U8*)"\r\nTask3 is running! Tick is: %d", MDS_GetSystemTick());

    /* 唤醒task4 */
    (void)MDS_TaskWake(gpstrTask4Tcb);

    while(1)
    {
        /* 任务打印 */
        DEV_PutStrToMem((U8*)"\r\nTask3 is running! Tick is: %d",
                        MDS_GetSystemTick());

        /* 任务运行5秒 */
        TEST_TaskRun(5000);

        /* 任务延迟5秒 */
        (void)MDS_TaskDelay(500);
    }
}

/***********************************************************************************
函数功能: 测试任务切换函数4, 向内存打印字符串.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void TEST_TestTask4(void* pvPara)
{
    while(1)
    {
        /* 任务打印 */
        DEV_PutStrToMem((U8*)"\r\nTask4 is running! Tick is: %d",
                        MDS_GetSystemTick());

        /* 任务运行1秒 */
        TEST_TaskRun(1000);

        /* 任务延迟10秒 */
        (void)MDS_TaskDelay(1000);
    }
}

/***********************************************************************************
函数功能: 串口打印任务, 从队列获取需要打印的消息缓冲, 将缓冲中的打印数据打印到串口.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void TEST_SerialPrintTask(void* pvPara)
{
    M_DLIST* pstrMsgQueNode;
    MSGBUF* pstrMsgBuf;

    /* 从队列循环获取消息 */
    while(1)
    {
        /* 从队列中获取到一条需要打印的消息, 向串口打印消息数据 */
        if(RTN_SUCD == MDS_QueGet(&gstrSerialMsgQue, &pstrMsgQueNode))
        {
            pstrMsgBuf = (MSGBUF*)pstrMsgQueNode;

            /* 将缓冲中的数据打印到串口 */
            DEV_PrintMsg(pstrMsgBuf->aucBuf, pstrMsgBuf->ucLength);

            /* 缓冲消息中的数据发送完毕, 释放缓冲 */
            DEV_BufferFree(&gstrBufPool, pstrMsgQueNode);
        }
        else /* 没有获取到消息, 延迟一段时间后再查询队列 */
        {
            (void)MDS_TaskDelay(100);
        }
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
}

