
#ifndef  MDS_CORE_C_INNER_H
#define  MDS_CORE_C_INNER_H


/* 操作系统所使用的用户 */
#define USERROOT                0       /* ROOT用户, 具有所有权限 */
#define USERGUEST               1       /* GUEST用户, 具有部分权限 */


/*********************************** 变量声明 *************************************/
extern U8 gaucRootTaskStack[ROOTTASKSTACK];
extern M_TASKSCHEDTAB gstrReadyTab;
extern STACKREG* gpstrCurTaskReg;
extern STACKREG* gpstrNextTaskReg;
extern M_TCB* gpstrCurTcb;
extern M_TCB* gpstrRootTaskTcb;
extern U32 guiUser;
extern const U8 caucTaskPrioUnmapTab[256];


/*********************************** 函数声明 *************************************/
extern void MDS_SystemVarInit(void);
extern void MDS_BeforeRootTask(void* pvPara);
extern void MDS_TaskSwitch(M_TCB* pstrTcb);
extern void MDS_TaskStart(M_TCB* pstrTcb);
extern void MDS_TaskSchedTabInit(M_TASKSCHEDTAB* pstrSchedTab);
extern void MDS_TaskAddToSchedTab(M_DLIST* pstrList, M_DLIST* pstrNode,
                                  M_PRIOFLAG* pstrPrioFlag, U8 ucTaskPrio);
extern void MDS_TaskSched(void);
extern M_TCB* MDS_TaskReadyTabSched(void);
extern void MDS_TaskSetPrioFlag(M_PRIOFLAG* pstrPrioFlag, U8 ucTaskPrio);
extern U8 MDS_TaskGetHighestPrio(M_PRIOFLAG* pstrPrioFlag);
extern void MDS_SwitchToTask(void);
extern void MDS_SetUser(U32 uiUser);
extern U32 MDS_GetUser(void);


#endif

