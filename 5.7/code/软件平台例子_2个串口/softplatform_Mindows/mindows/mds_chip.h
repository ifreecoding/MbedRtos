
#ifndef  MDS_CHIP_H
#define  MDS_CHIP_H


/* 软中断服务号, 0x00000000~0x1FFFFFFF为操作系统保留 */
#define SWI_TASKSCHED           0x10001001  /* 任务调度 */


/*********************************** 函数声明 *************************************/
extern void MDS_IntPendSvSet(void);
extern U32 MDS_RunInInt(void);
extern U32 MDS_IntLock(void);
extern U32 MDS_IntUnlock(void);


#endif

