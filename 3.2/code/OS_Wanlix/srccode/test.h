
#ifndef  TEST_H
#define  TEST_H


#include "global.h"


#define TASKSTACK           100     /* 任务栈大小, 单位: 4字节 */


/*********************************** 变量声明 *************************************/
extern U32 gauiTask1Stack[TASKSTACK];
extern U32 gauiTask2Stack[TASKSTACK];


/*********************************** 函数声明 *************************************/
extern void TEST_TestTask1(void);
extern void TEST_TestTask2(void);
extern U32* TEST_GetTaskInitSp(U8 ucTask);
extern void TEST_TaskRun(U32 uiMs);


#endif

