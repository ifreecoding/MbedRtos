
#ifndef  MDS_CORE_C_INNER_H
#define  MDS_CORE_C_INNER_H


/* ϵͳ״̬ */
#define SYSTEMSCHEDULE          0       /* �������ϵͳ״̬ */
#define SYSTEMNOTSCHEDULE       1       /* δ�������ϵͳ״̬ */


/* tick���ȱ�־ */
#define TICKSCHEDCLR            0       /* ��tick�жϵ���״̬ */
#define TICKSCHEDSET            1       /* tick�жϵ���״̬ */


/* ����ϵͳ��ʹ�õ��û� */
#define USERROOT                0       /* ROOT�û�, ��������Ȩ�� */
#define USERGUEST               1       /* GUEST�û�, ���в���Ȩ�� */


/*********************************** �������� *************************************/
extern U32 guiSystemStatus;
extern M_TASKSCHEDTAB gstrReadyTab;
extern M_DLIST gstrDelayTab;
extern U32 guiTick;
extern U8 gucTickSched;
extern STACKREG* gpstrCurTaskReg;
extern STACKREG* gpstrNextTaskReg;
extern M_TCB* gpstrCurTcb;
extern M_TCB* gpstrRootTaskTcb;
extern M_TCB* gpstrIdleTaskTcb;
extern U32 guiUser;
extern const U8 caucTaskPrioUnmapTab[256];


/*********************************** �������� *************************************/
extern void MDS_SystemVarInit(void);
extern void MDS_BeforeRootTask(void* pvPara);
extern void MDS_TaskSwitch(M_TCB* pstrTcb);
extern void MDS_TaskStart(M_TCB* pstrTcb);
extern void MDS_TaskSchedTabInit(M_TASKSCHEDTAB* pstrSchedTab);
extern void MDS_TaskAddToSchedTab(M_DLIST* pstrList, M_DLIST* pstrNode,
                                  M_PRIOFLAG* pstrPrioFlag, U8 ucTaskPrio);
extern void MDS_TaskAddToDelayTab(M_DLIST* pstrNode);
extern M_DLIST* MDS_TaskDelFromSchedTab(M_DLIST* pstrList, M_PRIOFLAG* pstrPrioFlag,
                                        U8 ucTaskPrio);
extern void MDS_TaskSched(void);
extern M_TCB* MDS_TaskReadyTabSched(void);
extern void MDS_TaskDelayTabSched(void);
extern void MDS_TaskSetPrioFlag(M_PRIOFLAG* pstrPrioFlag, U8 ucTaskPrio);
extern void MDS_TaskClrPrioFlag(M_PRIOFLAG* pstrPrioFlag, U8 ucTaskPrio);
extern U8 MDS_TaskGetHighestPrio(M_PRIOFLAG* pstrPrioFlag);
extern void MDS_SwitchToTask(void);
extern U32 MDS_GetXpsr(void);
extern void MDS_SetUser(U32 uiUser);
extern U32 MDS_GetUser(void);


#endif

