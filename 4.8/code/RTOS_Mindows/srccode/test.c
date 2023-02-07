
#include "test.h"


M_QUE* gpstrSerialMsgQue;               /* 串口打印消息队列指针 */
M_SEM* gpstrLcdMsgSem;                  /* LCD打印信号量指针 */

M_TCB* gpstrSerialTaskTcb;              /* 串口打印任务TCB指针 */
M_TCB* gpstrLcdTaskTcb;                 /* LCD打印任务TCB指针 */

M_SEM* gpstrSemSync;                    /* 用于任务1和任务2之间同步的信号量 */
M_SEM* gpstrSemMute;                    /* 用于任务3和任务4之间互斥的信号量 */


/***********************************************************************************
函数功能: 测试任务切换函数1, 向内存打印字符串.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void TEST_TestTask1(void* pvPara)
{
    U8 i;

    i = 0;

    while(1)
    {
        /* 任务打印 */
        DEV_PutStrToMem((U8*)"\r\nTask1 is running ! Tick is: %d",
                        MDS_GetSystemTick());

        /* 任务运行1.5秒 */
        TEST_TaskRun(1500);

        /* 任务延迟2秒 */
        (void)MDS_TaskDelay(200);

        /* 前10次, 每次运行释放一次gpstrSemSync信号量 */
        if(i < 10)
        {
            i++;

            /* 任务打印 */
            DEV_PutStrToMem((U8*)"\r\nTask1 give gpstrSemSync %d! Tick is: %d",
                            i, MDS_GetSystemTick());

            /* 同步其它任务 */
            (void)MDS_SemGive(gpstrSemSync);
        }
        /* 接下来5次, 每次运行flush一次gpstrSemSync信号量 */
        else if(i < 15)
        {
            i++;

            /* 任务打印 */
            DEV_PutStrToMem((U8*)"\r\nTask1 flush gpstrSemSync %d! Tick is: %d", i,
                            MDS_GetSystemTick());

            /* 释放所有被gpstrSemSync阻塞的任务 */
            (void)MDS_SemFlush(gpstrSemSync);
        }
        /* 删除gpstrSemSync信号量 */
        else if(15 == i)
        {
            i++;

            /* 任务打印 */
            DEV_PutStrToMem((U8*)"\r\nTask1 delete gpstrSemSync %d! Tick is: %d", i,
                            MDS_GetSystemTick());

            /* 删除gpstrSemSync信号量 */
            (void)MDS_SemDelete(gpstrSemSync);
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
    U8 i;

    i = 0;

    while(1)
    {
        /* 前3次获取gpstrSemMute信号量, 与TEST_TestTask3任务互锁 */
        if(i < 3)
        {
            i++;

            /* 获取到信号量才运行 */
            (void)MDS_SemTake(gpstrSemMute, SEMWAITFEV);

            /* 任务打印 */
            DEV_PutStrToMem((U8*)"\r\nTask2 take gpstrSemMute %d! Tick is: %d", i,
                            MDS_GetSystemTick());

            /* 任务运行0.5秒 */
            TEST_TaskRun(500);

            /* 任务延迟2秒 */
            (void)MDS_TaskDelay(200);

            /* 任务打印 */
            DEV_PutStrToMem((U8*)"\r\nTask2 give gpstrSemMute %d! Tick is: %d", i,
                            MDS_GetSystemTick());

            /* 释放信号量, 以便其它任务可以获得该信号量 */
            (void)MDS_SemGive(gpstrSemMute);
        }
        else /* 接下来获取gpstrSemSync信号量, 由TEST_TestTask1任务激活 */
        {
            i++;

            /* 信号量被删除, 任务返回 */
            if(RTN_SMTKDL == MDS_SemTake(gpstrSemSync, SEMWAITFEV))
            {
                /* 任务打印 */
                DEV_PutStrToMem((U8*)"\r\nTask2 gpstrSemSync deleted! Tick is: %d",
                                MDS_GetSystemTick());

                return;
            }
            else /* 获取到gpstrSemSync信号量才运行 */
            {
                /* 任务打印 */
                DEV_PutStrToMem((U8*)"\r\nTask2 take gpstrSemSync %d! Tick is: %d",
                                i, MDS_GetSystemTick());

                /* 任务运行0.5秒 */
                TEST_TaskRun(500);
            }
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
    U8 i;

    i = 0;

    while(1)
    {
        /* 前3次获取gpstrSemMute信号量, 与TEST_TestTask2任务互锁 */
        if(i < 3)
        {
            i++;

            /* 获取到信号量才运行 */
            (void)MDS_SemTake(gpstrSemMute, SEMWAITFEV);

            /* 任务打印 */
            DEV_PutStrToMem((U8*)"\r\nTask3 take gpstrSemMute %d! Tick is: %d", i,
                            MDS_GetSystemTick());

            /* 任务运行0.5秒 */
            TEST_TaskRun(500);

            /* 任务延迟1.5秒 */
            (void)MDS_TaskDelay(150);

            /* 任务打印 */
            DEV_PutStrToMem((U8*)"\r\nTask3 give gpstrSemMute %d! Tick is: %d", i,
                            MDS_GetSystemTick());

            /* 释放信号量, 以便其它任务可以获得该信号量 */
            (void)MDS_SemGive(gpstrSemMute);
        }
        else /* 接下来获取gpstrSemSync信号量, 由TEST_TestTask1任务激活 */
        {
            i++;

            /* 信号量被删除, 任务返回 */
            if(RTN_SMTKDL == MDS_SemTake(gpstrSemSync, SEMWAITFEV))
            {
                /* 任务打印 */
                DEV_PutStrToMem((U8*)"\r\nTask3 gpstrSemSync deleted! Tick is: %d",
                                MDS_GetSystemTick());

                return;
            }
            else /* 获取到gpstrSemSync信号量才运行 */
            {
                /* 任务打印 */
                DEV_PutStrToMem((U8*)"\r\nTask3 take gpstrSemSync %d! Tick is: %d",
                                i, MDS_GetSystemTick());

                /* 任务运行0.5秒 */
                TEST_TaskRun(500);
            }
        }
    }
}

/***********************************************************************************
函数功能: 测试任务切换函数4, 向内存打印字符串.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void TEST_TestTask4(void* pvPara)
{
    U8 i;

    i = 0;

    while(1)
    {
        /* 信号量被删除, 任务返回 */
        if(RTN_SMTKDL == MDS_SemTake(gpstrSemSync, SEMWAITFEV))
        {
            /* 任务打印 */
            DEV_PutStrToMem((U8*)"\r\nTask4 gpstrSemSync deleted! Tick is: %d",
                            MDS_GetSystemTick());

            return;
        }
        else /* 获取到gpstrSemSync信号量才运行 */
        {
            i++;

            /* 任务打印 */
            DEV_PutStrToMem((U8*)"\r\nTask4 take gpstrSemSync %d! Tick is: %d", i,
                            MDS_GetSystemTick());

            /* 任务运行0.5秒 */
            TEST_TaskRun(500);
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
        /* 从队列中获取到一条需要打印的消息, 向串口打印消息数据 */
        if(RTN_SUCD == MDS_QueGet(gpstrSerialMsgQue, &pstrMsgQueNode))
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
函数功能: LCD打印任务, 若能获取到信号量则将任务切换信息打印到LCD显示屏.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void TEST_LcdPrintTask(void* pvPara)
{
    /* 循环获取信号量 */
    while(1)
    {
        /* 获取信号量, 向LCD打印任务切换图形若获取不到信号量任务则挂在信号量上进入
           pend状态, 等待信号量被再次释放重新变为ready态 */
        if(RTN_SUCD == MDS_SemTake(gpstrLcdMsgSem, SEMWAITFEV))
        {
            /* 将任务切换信息打印到LCD屏幕 */
            DEV_PrintTaskSwitchMsgToLcd();
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

        /* 如果是创建根任务则存储第一个运行任务的任务切换信息 */
        if(MDS_GetRootTcb() == pstrTcb)
        {
            /* 存储任务切换信息 */
            DEV_SaveTaskSwitchMsg(pstrTcb->pucTaskName);
        }
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
    /* 为不打印串口打印任务和LCD打印任务的切换过程, 将这2个任务认为是空闲任务 */
    if((pstrOldTcb == gpstrSerialTaskTcb) || (pstrOldTcb == gpstrLcdTaskTcb))
    {
        pstrOldTcb = MDS_GetIdleTcb();
    }

    if((pstrNewTcb == gpstrSerialTaskTcb) || (pstrNewTcb == gpstrLcdTaskTcb))
    {
        pstrNewTcb = MDS_GetIdleTcb();
    }

    /* 同一个任务之间切换不打印信息 */
    if(pstrOldTcb == pstrNewTcb)
    {
        return;
    }

    /* 没有删除切换前任务 */
    if(NULL != pstrOldTcb)
    {
        /* 向内存打印任务切换信息 */
        DEV_PutStrToMem((U8*)"\r\nTask %s ---> Task %s! Tick is: %d",
                        pstrOldTcb->pucTaskName, pstrNewTcb->pucTaskName,
                        MDS_GetSystemTick());
    }
    else /* 切换前的任务被删除 */
    {
        /* 向内存打印任务切换信息 */
        DEV_PutStrToMem((U8*)"\r\nTask NULL ---> Task %s! Tick is: %d",
                        pstrNewTcb->pucTaskName, MDS_GetSystemTick());
    }

    /* 存储任务切换信息 */
    DEV_SaveTaskSwitchMsg(pstrNewTcb->pucTaskName);
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

