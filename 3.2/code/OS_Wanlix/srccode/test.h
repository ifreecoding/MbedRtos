
#ifndef  TEST_H
#define  TEST_H


#include "global.h"


#define TASKSTACK           100     /* ����ջ��С, ��λ: 4�ֽ� */


/*********************************** �������� *************************************/
extern U32 gauiTask1Stack[TASKSTACK];
extern U32 gauiTask2Stack[TASKSTACK];


/*********************************** �������� *************************************/
extern void TEST_TestTask1(void);
extern void TEST_TestTask2(void);
extern U32* TEST_GetTaskInitSp(U8 ucTask);
extern void TEST_TaskRun(U32 uiMs);


#endif

