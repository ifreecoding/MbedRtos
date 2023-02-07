
#ifndef  MDS_TASK_INNER_H
#define  MDS_TASK_INNER_H


/* 任务优先级 */
#define HIGHESTPRIO         0                   /* 任务最高优先级 */
#define LOWESTPRIO          (PRIORITYNUM - 1)   /* 任务最低优先级 */


/* 任务标志 */
#define DELAYQUEFLAG        0x00000001  /* 任务在delay表中的标志, 0: 不在, 1: 在 */


/*********************************** 变量声明 *************************************/
#ifdef MDS_INCLUDETASKHOOK
extern VFHSWT gvfTaskSwitchHook;
#endif


/*********************************** 函数声明 *************************************/
extern M_TCB* MDS_TaskTcbInit(U8* pucTaskName, VFUNC vfFuncPointer, void* pvPara,
          U8* pucTaskStack, U32 uiStackSize, U8 ucTaskPrio, M_TASKOPT* pstrTaskOpt);

#ifdef MDS_INCLUDETASKHOOK
extern void MDS_TaskHookInit(void);
#endif


#endif

