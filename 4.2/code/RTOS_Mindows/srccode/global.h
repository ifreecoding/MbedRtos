
#ifndef  GLOBAL_H
#define  GLOBAL_H


#include "mindows.h"


/* 软件版本号 */
#define SOFTWARE_VER            "001.001.002.000"


#define TASKSTACK               1024    /* 任务栈大小, 单位: 字节 */


/*********************************** 变量声明 *************************************/
extern U8 gaucTask1Stack[TASKSTACK];
extern U8 gaucTask2Stack[TASKSTACK];
extern U8 gaucTask3Stack[TASKSTACK];


/*********************************** 函数声明 *************************************/
extern void DEV_SoftwareInit(void);
extern void DEV_HardwareInit(void);
extern void DEV_PutString(U8* pucChar);
extern void DEV_DelayMs(U32 uiMs);
extern void TEST_TestTask1(void* pvPara);
extern void TEST_TestTask2(void* pvPara);
extern void TEST_TestTask3(void* pvPara);


#endif

