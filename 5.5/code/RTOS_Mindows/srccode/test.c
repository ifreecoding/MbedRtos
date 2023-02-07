
#include "test.h"


M_QUE* gpstrSerialMsgQue;               /* 串口打印消息队列指针 */
M_SEM* gpstrLcdMsgSem;                  /* LCD打印信号量指针 */

M_TCB* gpstrSerialTaskTcb;              /* 串口打印任务TCB指针 */
M_TCB* gpstrLcdTaskTcb;                 /* LCD打印任务TCB指针 */


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
        DEV_PutStrToMem((U8*)"\r\nTask1 will run 2s! Tick is: %d",
                        MDS_GetSystemTick());

        /* 任务运行2秒 */
        TEST_TaskRun(2000);

        /* 任务打印 */
        DEV_PutStrToMem((U8*)"\r\nTask1 will delay 5s! Tick is: %d",
                        MDS_GetSystemTick());

        /* 任务延迟5秒 */
        (void)MDS_TaskDelay(500);

        /* 超过70秒后退出该任务 */
        if(MDS_GetSystemTick() > 7000)
        {
            break;
        }
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
        DEV_PutStrToMem((U8*)"\r\nTask2 will run 0.5s! Tick is: %d",
                        MDS_GetSystemTick());

        /* 任务运行0.5秒 */
        TEST_TaskRun(500);

        /* 任务打印 */
        DEV_PutStrToMem((U8*)"\r\nTask2 will delay 0.5s! Tick is: %d",
                        MDS_GetSystemTick());

        /* 任务延迟2秒 */
        (void)MDS_TaskDelay(200);

        /* 超过50秒后退出该任务 */
        if(MDS_GetSystemTick() > 5000)
        {
            break;
        }
    }
}

/***********************************************************************************
函数功能: 测试任务切换函数3, 向内存打印字符串.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void TEST_TestTask3(void* pvPara)
{
    while(1)
    {
        /* 任务打印 */
        DEV_PutStrToMem((U8*)"\r\nTask3 will run 0.5s! Tick is: %d",
                        MDS_GetSystemTick());

        /* 任务运行0.2秒 */
        TEST_TaskRun(200);

        /* 任务打印 */
        DEV_PutStrToMem((U8*)"\r\nTask3 will delay 0.5s! Tick is: %d",
                        MDS_GetSystemTick());

        /* 任务延迟0.4秒 */
        (void)MDS_TaskDelay(40);

        /* 超过30秒后退出该任务 */
        if(MDS_GetSystemTick() > 3000)
        {
            break;
        }
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
        /* 从队列中获取到一条需要打印的消息, 向串口打印消息数据, 若获取不到队列消息
           任务则挂在队列上进入pend状态, 等待有新消息进入队列重新变为ready态 */
        if(RTN_SUCD == MDS_QueGet(gpstrSerialMsgQue, &pstrMsgQueNode, QUEWAITFEV))
        {
            pstrMsgBuf = (MSGBUF*)pstrMsgQueNode;

            /* 将缓冲中的数据打印到串口 */
            DEV_PrintMsg(pstrMsgBuf->aucBuf, pstrMsgBuf->ucLength);

            /* 缓冲消息中的数据发送完毕, 释放缓冲 */
            DEV_BufferFree(&gstrBufPool, pstrMsgQueNode);
        }
    }
}

/***********************************************************************************
函数功能: LCD打印任务, 每秒打印一次CPU占有率.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void TEST_LcdPrintTask(void* pvPara)
{
    while(1)
    {
        /* 获取到信号量则打印CPU占有率 */
        if(RTN_SUCD == MDS_SemTake(gpstrLcdMsgSem, SEMWAITFEV))
        {
            /* 将CPU占有率信息打印到LCD屏幕 */
            DEV_PrintCpuShareMsgToLcd();
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

/***********************************************************************************
函数功能: 将任务创建过程信息打印到内存中.
入口参数: pstrTcb: 新创建的任务的TCB指针.
返 回 值: none.
***********************************************************************************/
void TEST_TaskCreatePrint(M_TCB* pstrTcb)
{
    /* 任务创建成功 */
    if((M_TCB*)NULL != pstrTcb)
    {
        /* 打印任务创建成功信息 */
        DEV_PutStrToMem((U8*)"\r\nTask %s is created! Tick is: %d",
                        pstrTcb->pucTaskName, MDS_GetSystemTick());
    }
    else /* 任务创建失败 */
    {
        /* 打印任务创建失败信息 */
        DEV_PutStrToMem((U8*)"\r\nFail to create task! Tick is: %d",
                        MDS_GetSystemTick());
    }
}

/***********************************************************************************
函数功能: 将任务切换过程信息打印到内存中.
入口参数: pstrOldTcb: 切换前的任务TCB指针.
          pstrNewTcb: 切换后的任务TCB指针.
返 回 值: none.
***********************************************************************************/
void TEST_TaskSwitchPrint(M_TCB* pstrOldTcb, M_TCB* pstrNewTcb)
{
    static U32 suiLastTime = 0;
    U32 uiTime;

    /* 获取当前时间 */
    uiTime = MDS_GetSystemTick() / 100;

    /* 经过1秒时间 */
    if(uiTime != suiLastTime)
    {
        /* 发送一次信号量, 显示CPU占有率 */
        (void)MDS_SemGive(gpstrLcdMsgSem);

        /* 更新时间 */
        suiLastTime = uiTime;
    }
}

/***********************************************************************************
函数功能: 将任务删除过程信息打印到内存中.
入口参数: pstrTcb: 被删除的任务的TCB指针.
返 回 值: none.
***********************************************************************************/
void TEST_TaskDeletePrint(M_TCB* pstrTcb)
{
    /* 打印任务删除信息 */
    DEV_PutStrToMem((U8*)"\r\nTask %s is deleted! Tick is: %d",
                    pstrTcb->pucTaskName, MDS_GetSystemTick());
}

