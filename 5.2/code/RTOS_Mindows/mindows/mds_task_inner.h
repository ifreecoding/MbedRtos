
#ifndef  MDS_TASK_INNER_H
#define  MDS_TASK_INNER_H


/* �������ȼ� */
#define HIGHESTPRIO         0                   /* ����������ȼ� */
#define LOWESTPRIO          (PRIORITYNUM - 1)   /* ����������ȼ� */


/* �����־ */
#define DELAYQUEFLAG        0x00000001  /* ������delay���еı�־, 0: ����, 1: �� */
#define TASKSTACKFLAG       0x00000002  /* ����ջ��־, 0: ������, 1: ���� */
#define TASKPRIINHFLAG      0x00000004  /* �������ȼ��̳б�־, 0: δ�̳�, 1: �̳� */


/*********************************** �������� *************************************/
#ifdef MDS_TASKROUNDROBIN
extern U32 guiTimeSlice;
extern U32 gauiSliceCnt[PRIORITYNUM];
#endif

#ifdef MDS_INCLUDETASKHOOK
extern VFHCRT gvfTaskCreateHook;
extern VFHSWT gvfTaskSwitchHook;
extern VFHDLT gvfTaskDeleteHook;
#endif


/*********************************** �������� *************************************/
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

