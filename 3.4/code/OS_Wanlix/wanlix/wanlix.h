
#ifndef  WANLIX_H
#define  WANLIX_H


#define ROOTTASKSTACK       400     /* 根任务栈大小, 单位: 字节 */


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


/*********************************** 函数声明 *************************************/
extern W_TCB* WLX_TaskCreate(VFUNC vfFuncPointer, U8* pucTaskStack,
                             U32 uiStackSize);
extern void WLX_TaskSwitch(W_TCB* pstrTcb);


#endif

