
#ifndef  MDS_TASK_H
#define  MDS_TASK_H


/* 用户任务优先级 */
#define USERHIGHESTPRIO     (HIGHESTPRIO + 1)   /* 用户任务最高优先级 */
#define USERLOWESTPRIO      (LOWESTPRIO - 1)    /* 用户任务最低优先级 */


/* 任务状态 */
#define TASKRUNNING         0x01        /* 任务运行态 */
#define TASKREADY           0x02        /* 任务就绪态 */
#define TASKDELAY           0x04        /* 任务延迟态 */
#define TASKPEND            0x08        /* 任务阻塞态 */
#define TASKSUSPEND         0x10        /* 任务挂起态 */


/* 延迟任务返回值 */
#define RTN_TKDLTO          2           /* 任务延迟时间耗尽, 超时返回 */
#define RTN_TKDLBK          3           /* 任务延迟状态被中断, 任务返回 */


/* 延迟的时间 */
#define DELAYNOWAIT         0           /* 不等待 */
#define DELAYWAITFEV        0xFFFFFFFF  /* 永久等待 */


/*********************************** 函数声明 *************************************/
extern M_TCB* MDS_TaskCreate(VFUNC vfFuncPointer, void* pvPara, U8* pucTaskStack,
                            U32 uiStackSize, U8 ucTaskPrio, M_TASKOPT* pstrTaskOpt);
extern U32 MDS_TaskDelay(U32 uiDelayTick);
extern U32 MDS_TaskWake(M_TCB* pstrTcb);


#endif

