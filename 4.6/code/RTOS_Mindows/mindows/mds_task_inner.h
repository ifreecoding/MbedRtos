
#ifndef  MDS_TASK_INNER_H
#define  MDS_TASK_INNER_H


/* �������ȼ� */
#define HIGHESTPRIO         0                   /* ����������ȼ� */
#define LOWESTPRIO          (PRIORITYNUM - 1)   /* ����������ȼ� */


/* �����־ */
#define DELAYQUEFLAG        0x00000001  /* ������delay���еı�־, 0: ����, 1: �� */


/*********************************** �������� *************************************/
#ifdef MDS_INCLUDETASKHOOK
extern VFHCRT gvfTaskCreateHook;
extern VFHSWT gvfTaskSwitchHook;
extern VFHDLT gvfTaskDeleteHook;
#endif


/*********************************** �������� *************************************/
extern M_TCB* MDS_TaskTcbInit(U8* pucTaskName, VFUNC vfFuncPointer, void* pvPara,
          U8* pucTaskStack, U32 uiStackSize, U8 ucTaskPrio, M_TASKOPT* pstrTaskOpt);

#ifdef MDS_INCLUDETASKHOOK
extern void MDS_TaskHookInit(void);
#endif


#endif

