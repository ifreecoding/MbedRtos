
#ifndef  TEST_H
#define  TEST_H


#include "global.h"


#define TASKSTACK           400     /* 任务栈大小, 单位: 字节 */


/*********************************** 变量声明 *************************************/
extern U8 gaucTask1Stack[TASKSTACK];
extern U8 gaucTask2Stack[TASKSTACK];
extern U8 gaucTask3Stack[TASKSTACK];
extern W_TCB* gpstrTask1Tcb;
extern W_TCB* gpstrTask2Tcb;
extern W_TCB* gpstrTask3Tcb;


/*********************************** 函数声明 *************************************/
extern void TEST_TestTask1(void);
extern void TEST_TestTask2(void);
extern void TEST_TestTask3(void);
extern void TEST_TaskRun(U32 uiMs);


#endif

