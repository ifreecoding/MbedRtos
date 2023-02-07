
#ifndef  MDS_CHIP_INNER_H
#define  MDS_CHIP_INNER_H


/* 中断返回时的地址 */
#define RTN_HANDLER         0xFFFFFFF1  /* 返回handler模式 */
#define RTN_THREAD_MSP      0xFFFFFFF9  /* 返回thread模式, 并使用MSP */
#define RTN_THREAD_PSP      0xFFFFFFFD  /* 返回handler模式, 并使用PSP */


/* XPSR寄存器中中断的掩码位 */
#define XPSR_EXTMASK        0x1FF


/* 堆栈指针选择 */
#define MSP_STACK               0       /* 主堆栈指针 */
#define PSP_STACK               1       /* 进程堆栈指针 */

/* 系统工作模式 */
#define PRIVILEGED              0       /* 特权级模式 */
#define UNPRIVILEGED            1       /* 用户级模式 */


/*********************************** 变量声明 *************************************/
extern U32 guiIntLockCounter;


/*********************************** 函数声明 *************************************/
extern void MDS_TaskStackInit(M_TCB* pstrTcb, VFUNC vfFuncPointer, void* pvPara);
extern void MDS_SystemHardwareInit(void);
extern void MDS_TickTimerInit(void);
extern void MDS_PendSvIsrInit(void);
extern void MDS_SetChipWorkMode(U32 uiMode);
extern void MDS_TaskSwiSched(void);
extern void MDS_TaskOccurSwi(U32 uiSwiNo);


#endif

