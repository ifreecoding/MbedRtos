
#ifndef  MDS_TASK_INNER_H
#define  MDS_TASK_INNER_H


/* 任务优先级 */
#define HIGHESTPRIO         0                   /* 任务最高优先级 */
#define LOWESTPRIO          (PRIORITYNUM - 1)   /* 任务最低优先级 */


/* 任务标志 */
#define DELAYQUEFLAG        0x00000001  /* 任务在delay表中的标志, 0: 不在, 1: 在 */
#define TASKSTACKFLAG       0x00000002  /* 申请栈标志, 0: 不申请, 1: 申请 */
#define TASKPRIINHFLAG      0x00000004  /* 任务优先级继承标志, 0: 未继承, 1: 继承 */


/*********************************** 变量声明 *************************************/
#ifdef MDS_TASKROUNDROBIN
extern U32 guiTimeSlice;
extern U32 gauiSliceCnt[PRIORITYNUM];
#endif

#ifdef MDS_INCLUDETASKHOOK
extern VFHCRT gvfTaskCreateHook;
extern VFHSWT gvfTaskSwitchHook;
extern VFHDLT gvfTaskDeleteHook;
#endif


/*********************************** 函数声明 *************************************/
extern M_TCB* MDS_TaskTcbInit(U8* pucTaskName, VFUNC vfFuncPointer, void* pvPara,
          U8* pucTaskStack, U32 uiStackSize, U8 ucTaskPrio, M_TASKOPT* pstrTaskOpt);
extern U32 MDS_TaskPend(M_SEM* pstrSem, U32 uiDelayTick);

#ifdef MDS_TASKPRIOINHER
extern void MDS_TaskPrioInheritance(M_TCB* pstrTcb, U8 ucTaskPrio);
extern void MDS_TaskPrioResume(M_TCB* pstrTcb);
#endif

#ifdef MDS_INCLUDETASKHOOK
extern void MDS_TaskHookInit(void);
#endif


#endif

