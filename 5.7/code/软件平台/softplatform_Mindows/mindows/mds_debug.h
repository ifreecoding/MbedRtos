
#ifndef  MDS_DEBUG_H
#define  MDS_DEBUG_H


/*********************************** �������� *************************************/
#ifdef MDS_DEBUGCONTEXT
extern void MDS_SendCharFuncInit(VFUNC1 vfFuncPointer);
#endif

#ifdef MDS_DEBUGSTACKCHECK
extern U32 MDS_TaskStackCheck(M_TCB* pstrTcb);
#endif

#ifdef MDS_CPUSHARE
extern U32 MDS_GetCpuShare(M_TCB* pstrTcb);
#endif


#endif

