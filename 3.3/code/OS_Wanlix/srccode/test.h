
#ifndef  TEST_H
#define  TEST_H


#include "global.h"


#define TASKSTACK           400     /* ����ջ��С, ��λ: �ֽ� */


/*********************************** �������� *************************************/
extern U8 gaucTask1Stack[TASKSTACK];
extern U8 gaucTask2Stack[TASKSTACK];
extern U8 gaucTask3Stack[TASKSTACK];
extern W_TCB* gpstrTask1Tcb;
extern W_TCB* gpstrTask2Tcb;
extern W_TCB* gpstrTask3Tcb;


/*********************************** �������� *************************************/
extern void TEST_TestTask1(void);
extern void TEST_TestTask2(void);
extern void TEST_TestTask3(void);
extern void TEST_TaskRun(U32 uiMs);


#endif

