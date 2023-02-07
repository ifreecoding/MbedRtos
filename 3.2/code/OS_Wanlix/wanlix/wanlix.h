
#ifndef  WANLIX_H
#define  WANLIX_H


/* 定义变量类型 */
typedef char                U8;     /* 本编译器char型为无符号型 */
typedef unsigned short      U16;
typedef unsigned int        U32;
typedef signed char         S8;
typedef short               S16;
typedef int                 S32;
typedef void                (*VFUNC)(void);


#ifndef NULL
 #define NULL               ((void*)0)
#endif


/* TCB中备份寄存器组的结构体, 用来临时保存任务前换的寄存器 */
typedef struct stackreg
{
    U32 uiR4;
    U32 uiR5;
    U32 uiR6;
    U32 uiR7;
    U32 uiR8;
    U32 uiR9;
    U32 uiR10;
    U32 uiR11;
    U32 uiR12;
    U32 uiR13;
    U32 uiR14;
    U32 uiXpsr;
}STACKREG;

/* TCB结构体 */
typedef struct w_tcb
{
    STACKREG strStackReg;           /* 备份寄存器组 */
}W_TCB;


/*********************************** 变量声明 *************************************/
extern W_TCB* gpstrTask1Tcb;
extern W_TCB* gpstrTask2Tcb;


/*********************************** 函数声明 *************************************/
extern W_TCB* WLX_TaskInit(VFUNC vfFuncPointer, U32* puiTaskStack);
extern void WLX_TaskSwitch(void);
extern void WLX_TaskStart(void);


#endif

