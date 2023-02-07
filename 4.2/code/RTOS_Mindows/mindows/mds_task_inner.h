
#ifndef  MDS_TASK_INNER_H
#define  MDS_TASK_INNER_H


/* 任务优先级 */
#define HIGHESTPRIO         0                   /* 任务最高优先级 */
#define LOWESTPRIO          (PRIORITYNUM - 1)   /* 任务最低优先级 */


/*********************************** 函数声明 *************************************/
extern M_TCB* MDS_TaskTcbInit(VFUNC vfFuncPointer, void* pvPara, U8* pucTaskStack,
                              U32 uiStackSize, U8 ucTaskPrio);


#endif

