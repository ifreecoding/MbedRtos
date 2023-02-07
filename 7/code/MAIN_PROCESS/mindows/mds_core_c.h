
#ifndef  MDS_CORE_C_H
#define  MDS_CORE_C_H


/*********************************** º¯ÊýÉùÃ÷ *************************************/
extern void MDS_TaskTick(void);
extern U32 MDS_GetSystemTick(void);
extern M_TCB* MDS_GetCurrentTcb(void);
extern M_TCB* MDS_GetRootTcb(void);
extern M_TCB* MDS_GetIdleTcb(void);
extern U8* MDS_GetMindowsVersion(void);


#endif

