
#ifndef  MDS_TASK_INNER_H
#define  MDS_TASK_INNER_H


/* �������ȼ� */
#define HIGHESTPRIO         0                   /* ����������ȼ� */
#define LOWESTPRIO          (PRIORITYNUM - 1)   /* ����������ȼ� */


/*********************************** �������� *************************************/
extern M_TCB* MDS_TaskTcbInit(VFUNC vfFuncPointer, void* pvPara, U8* pucTaskStack,
                              U32 uiStackSize, U8 ucTaskPrio);


#endif

