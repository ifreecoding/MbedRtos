
#ifndef  MDS_TASK_H
#define  MDS_TASK_H


/* �û��������ȼ� */
#define USERHIGHESTPRIO     (HIGHESTPRIO + 1)   /* �û�����������ȼ� */
#define USERLOWESTPRIO      (LOWESTPRIO - 1)    /* �û�����������ȼ� */


/* ����״̬ */
#define TASKRUNNING         0x01        /* ��������̬ */
#define TASKREADY           0x02        /* �������̬ */
#define TASKDELAY           0x04        /* �����ӳ�̬ */
#define TASKPEND            0x08        /* ��������̬ */
#define TASKSUSPEND         0x10        /* �������̬ */


/* �ӳ����񷵻�ֵ */
#define RTN_TKDLTO          2           /* �����ӳ�ʱ��ľ�, ��ʱ���� */
#define RTN_TKDLBK          3           /* �����ӳ�״̬���ж�, ���񷵻� */


/* �ӳٵ�ʱ�� */
#define DELAYNOWAIT         0           /* ���ȴ� */
#define DELAYWAITFEV        0xFFFFFFFF  /* ���õȴ� */


/*********************************** �������� *************************************/
extern M_TCB* MDS_TaskCreate(VFUNC vfFuncPointer, void* pvPara, U8* pucTaskStack,
                            U32 uiStackSize, U8 ucTaskPrio, M_TASKOPT* pstrTaskOpt);
extern U32 MDS_TaskDelay(U32 uiDelayTick);
extern U32 MDS_TaskWake(M_TCB* pstrTcb);


#endif

