
#ifndef MDS_SEM_INNER_H
#define MDS_SEM_INNER_H


/* 信号量参数掩码 */
#define SEMSCHEDOPTMASK     0x00000003      /* 信号量参数中调度方式的掩码 */
#define SEMTYPEMASK         0x0000001C      /* 信号量参数中信号量类型的掩码 */


/*********************************** 函数声明 *************************************/
extern U32 MDS_SemFlushValue(M_SEM* pstrSem, U32 uiRtnValue);


#endif

