
#include "mindows_inner.h"


U8 gaucRootTaskStack[ROOTTASKSTACK];    /* 根任务的栈 */


M_TASKSCHEDTAB gstrReadyTab;            /* 任务就绪表 */


STACKREG* gpstrCurTaskReg;              /* 当前运行的任务的寄存器组指针, 用于从C传递
                                           到汇编 */
STACKREG* gpstrNextTaskReg;             /* 将要运行的任务的寄存器组指针, 用于从C传递
                                           到汇编 */

M_TCB* gpstrCurTcb;                     /* 当前运行的任务的TCB指针 */
M_TCB* gpstrRootTaskTcb;                /* root任务TCB指针 */

U32 guiUser;                            /* 操作系统所使用的用户 */

const U8 caucTaskPrioUnmapTab[256] =    /* 优先级反向查找表 */
{
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};


/***********************************************************************************
函数功能: 主函数, C程序从此函数开始运行. 该函数创建系统任务并从非任务状态切换到root
          任务运行.
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

    /* 初始化系统变量, 建立操作系统启动所需要的环境 */
    MDS_SystemVarInit();

    /* 开始任务调度, 从前根任务开始执行 */
    MDS_TaskStart(gpstrRootTaskTcb);

    return 0;
}

/***********************************************************************************
函数功能: 初始化系统变量, 建立操作系统启动所需要的环境.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_SystemVarInit(void)
{
    /* 初始化为ROOT用户权限 */
    MDS_SetUser(USERROOT);

    /* 将当前运行任务, root任务的TCB初始化为NULL */
    gpstrCurTcb = (M_TCB*)NULL;
    gpstrRootTaskTcb = (M_TCB*)NULL;

    /* 初始化任务ready表 */
    MDS_TaskSchedTabInit(&gstrReadyTab);

    /* 创建前根任务 */
    gpstrRootTaskTcb = MDS_TaskCreate(MDS_BeforeRootTask, NULL, gaucRootTaskStack,
                       ROOTTASKSTACK, 2);
}

/***********************************************************************************
函数功能: 前根任务, 在根任务运行前运行, 用于在根任务运行前初始化操作系统用于调度使用
          的硬件, 然后再调用根任务进入用户代码.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void MDS_BeforeRootTask(void* pvPara)
{
    /* 初始化用于操作系统任务调度使用的硬件 */
    MDS_SystemHardwareInit();

    /* 即将进入到用户程序, 设置为GUEST用户权限 */
    MDS_SetUser(USERGUEST);

    /* 调用根任务, 进入用户代码 */
    MDS_RootTask();
}

/***********************************************************************************
函数功能: tick中断调度任务函数. tick中断会执行该函数, 由该函数触发PendSv中断进行任务
          调度.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_TaskTick(void)
{
    /* 触发PendSv中断, 在该中断中调度任务 */
    MDS_IntPendSvSet();
}

/***********************************************************************************
函数功能: 为发生的任务切换做准备, 更新任务切换过程中汇编函数所使用的变量.
入口参数: pstrTcb: 即将运行的任务的TCB指针.
返 回 值: none.
***********************************************************************************/
void MDS_TaskSwitch(M_TCB* pstrTcb)
{
    /* 当前任务的寄存器组地址, 汇编语言通过这个变量备份寄存器 */
    gpstrCurTaskReg = &gpstrCurTcb->strStackReg;

    /* 即将运行任务的寄存器组地址, 汇编语言通过这个变量恢复寄存器 */
    gpstrNextTaskReg = &pstrTcb->strStackReg;

    /* 即将运行任务的TCB指针 */
    gpstrCurTcb = pstrTcb;
}

/***********************************************************************************
函数功能: 实现从非操作系统状态切换到操作系统状态, 执行第一个任务调度.
入口参数: pstrTcb: 即将运行的任务的TCB指针.
返 回 值: none.
***********************************************************************************/
void MDS_TaskStart(M_TCB* pstrTcb)
{
    /* 即将运行任务的寄存器组地址, 汇编语言通过这个变量恢复寄存器 */
    gpstrNextTaskReg = &pstrTcb->strStackReg;

    /* 即将运行任务的TCB指针 */
    gpstrCurTcb = pstrTcb;

    /* 切换到操作系统状态 */
    MDS_SwitchToTask();
}

/***********************************************************************************
函数功能: 初始化任务调度表.
入口参数: pstrSchedTab: 任务调度表指针.
返 回 值: none.
***********************************************************************************/
void MDS_TaskSchedTabInit(M_TASKSCHEDTAB* pstrSchedTab)
{
    U32 i;

    /* 初始化每个优先级任务的ready表 */
    for(i = 0; i < PRIORITYNUM; i++)
    {
        MDS_DlistInit(&pstrSchedTab->astrList[i]);
    }

    /* 初始化ready表优先级标志 */
#if PRIORITYNUM >= PRIORITY128

    for(i = 0; i < PRIOFLAGGRP1; i++)
    {
        pstrSchedTab->strFlag.aucPrioFlagGrp1[i] = 0;
    }

    for(i = 0; i < PRIOFLAGGRP2; i++)
    {
        pstrSchedTab->strFlag.aucPrioFlagGrp2[i] = 0;
    }

    pstrSchedTab->strFlag.ucPrioFlagGrp3 = 0;

#elif PRIORITYNUM >= PRIORITY16

    for(i = 0; i< PRIOFLAGGRP1; i++)
    {
        pstrSchedTab->strFlag.aucPrioFlagGrp1[i] = 0;
    }

    pstrSchedTab->strFlag.ucPrioFlagGrp2 = 0;

#else

    pstrSchedTab->strFlag.ucPrioFlagGrp1 = 0;

#endif
}

/***********************************************************************************
函数功能: 将任务按照优先级添加到任务调度表.
入口参数: pstrList: 调度表指针, 将任务添加到该调度表.
          pstrNode: 需要添加到调度表的任务节点指针.
          pstrPrioFlag: 该调度表对应的优先级标志表指针.
          ucTaskPrio: 需要添加的任务的优先级.
返 回 值: none.
***********************************************************************************/
void MDS_TaskAddToSchedTab(M_DLIST* pstrList, M_DLIST* pstrNode,
                           M_PRIOFLAG* pstrPrioFlag, U8 ucTaskPrio)
{
    /* 将该任务节点添加到调度表中 */
    MDS_DlistNodeAdd(pstrList, pstrNode);

    /* 设置该任务对应调度表的优先级标志表 */
    MDS_TaskSetPrioFlag(pstrPrioFlag, ucTaskPrio);
}

/***********************************************************************************
函数功能: 任务调度函数, 在此函数实现任务各种状态转换的调度.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_TaskSched(void)
{
    M_TCB* pstrTcb;

    /* 调度ready表任务 */
    pstrTcb = MDS_TaskReadyTabSched();

    /* 任务切换 */
    MDS_TaskSwitch(pstrTcb);
}

/***********************************************************************************
函数功能: 对ready表进行调度.
入口参数: none.
返 回 值: 即将运行的任务的TCB指针.
***********************************************************************************/
M_TCB* MDS_TaskReadyTabSched(void)
{
    M_TCB* pstrTcb;
    M_TCBQUE* pstrTaskQue;
    U8 ucTaskPrio;

    /* 获取ready表中优先级最高的任务的TCB */
    ucTaskPrio = MDS_TaskGetHighestPrio(&gstrReadyTab.strFlag);
    pstrTaskQue = (M_TCBQUE*)MDS_DlistEmpInq(&gstrReadyTab.astrList[ucTaskPrio]);
    pstrTcb = pstrTaskQue->pstrTcb;

    return pstrTcb;
}

/***********************************************************************************
函数功能: 将任务添加到任务调度表对应的优先级标志表中.
入口参数: pstrPrioFlag: 调度表对应的优先级标志表指针.
          ucTaskPrio: 任务的优先级.
返 回 值: none.
***********************************************************************************/
void MDS_TaskSetPrioFlag(M_PRIOFLAG* pstrPrioFlag, U8 ucTaskPrio)
{
#if PRIORITYNUM >= PRIORITY128
    U8 ucPrioFlagGrp1;
    U8 ucPrioFlagGrp2;
    U8 ucPosInGrp1;
    U8 ucPosInGrp2;
    U8 ucPosInGrp3;
#elif PRIORITYNUM >= PRIORITY16
    U8 ucPrioFlagGrp1;
    U8 ucPosInGrp1;
    U8 ucPosInGrp2;
#endif

    /* 设置调度表对应的优先级标志表 */
#if PRIORITYNUM >= PRIORITY128

    /* 获取优先级标志在第一组和第二组中的组号 */
    ucPrioFlagGrp1 = ucTaskPrio / 8;
    ucPrioFlagGrp2 = ucPrioFlagGrp1 / 8;

    /* 获取优先级标志在每一组中的位置 */
    ucPosInGrp1 = ucTaskPrio % 8;
    ucPosInGrp2 = ucPrioFlagGrp1 % 8;
    ucPosInGrp3 = ucPrioFlagGrp2;

    /* 在每一组中设置优先级标志 */
    pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1] |= (U8)(1 << ucPosInGrp1);
    pstrPrioFlag->aucPrioFlagGrp2[ucPrioFlagGrp2] |= (U8)(1 << ucPosInGrp2);
    pstrPrioFlag->ucPrioFlagGrp3 |= (U8)(1 << ucPosInGrp3);

#elif PRIORITYNUM >= PRIORITY16

    ucPrioFlagGrp1 = ucTaskPrio / 8;

    ucPosInGrp1 = ucTaskPrio % 8;
    ucPosInGrp2 = ucPrioFlagGrp1;

    pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1] |= (U8)(1 << ucPosInGrp1);
    pstrPrioFlag->ucPrioFlagGrp2 |= (U8)(1 << ucPosInGrp2);

#else

    pstrPrioFlag->ucPrioFlagGrp1 |= (U8)(1 << ucTaskPrio);

#endif
}

/***********************************************************************************
函数功能: 获取任务调度表中任务的最高优先级.
入口参数: pstrPrioFlag: 调度表的优先级标志表指针.
返 回 值: 任务调度表中的最高优先级.
***********************************************************************************/
U8 MDS_TaskGetHighestPrio(M_PRIOFLAG* pstrPrioFlag)
{
#if PRIORITYNUM >= PRIORITY128
    U8 ucPrioFlagGrp1;
    U8 ucPrioFlagGrp2;
    U8 ucHighestFlagInGrp1;
#elif PRIORITYNUM >= PRIORITY16
    U8 ucPrioFlagGrp1;
    U8 ucHighestFlagInGrp1;
#endif

    /* 获取任务调度表中的最高优先级 */
#if PRIORITYNUM >= PRIORITY128

    ucPrioFlagGrp2 = caucTaskPrioUnmapTab[pstrPrioFlag->ucPrioFlagGrp3];

    ucPrioFlagGrp1 =
                caucTaskPrioUnmapTab[pstrPrioFlag->aucPrioFlagGrp2[ucPrioFlagGrp2]];

    ucHighestFlagInGrp1 = caucTaskPrioUnmapTab[pstrPrioFlag->aucPrioFlagGrp1
                                             [ucPrioFlagGrp2 * 8 + ucPrioFlagGrp1]];

    return (U8)((ucPrioFlagGrp2 * 8 + ucPrioFlagGrp1) * 8 + ucHighestFlagInGrp1);

#elif PRIORITYNUM >= PRIORITY16

    ucPrioFlagGrp1 = caucTaskPrioUnmapTab[pstrPrioFlag->ucPrioFlagGrp2];

    ucHighestFlagInGrp1 =
                caucTaskPrioUnmapTab[pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1]];

    return (U8)(ucPrioFlagGrp1 * 8 + ucHighestFlagInGrp1);

#else

    return caucTaskPrioUnmapTab[pstrPrioFlag->ucPrioFlagGrp1];

#endif
}

/***********************************************************************************
函数功能: 设置用户.
入口参数: uiUser: 需要设置的用户.
                  USERROOT: ROOT用户, 具有所有权限.
                  USERGUEST: GUEST用户, 具有部分权限.
返 回 值: none.
***********************************************************************************/
void MDS_SetUser(U32 uiUser)
{
    guiUser = uiUser;
}

/***********************************************************************************
函数功能: 获取用户.
入口参数: none.
返 回 值: 获取到的用户.
          USERROOT: ROOT用户, 具有所有权限.
          USERGUEST: GUEST用户, 具有部分权限.
***********************************************************************************/
U32 MDS_GetUser(void)
{
    return guiUser;
}

