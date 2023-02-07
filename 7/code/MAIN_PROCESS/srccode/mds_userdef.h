
#ifndef  MDS_USERDEF_H
#define  MDS_USERDEF_H


/******************************* 用户可修改包含文件 *******************************/
#include "stm32f10x.h"


/******************************** 用户可修改宏定义 ********************************/
/* 任务钩子功能宏定义, 只有定义该宏才可使用 */
//#define MDS_INCLUDETASKHOOK

/* 任务优先级继承宏定义, 只有定义该宏才可使用 */
//#define MDS_TASKPRIOINHER

/* 任务时间片轮转调度宏定义, 只有定义该宏才可使用 */
#define MDS_TASKROUNDROBIN

/* 记录任务寄存器组和栈信息功能的宏定义, 只有定义该宏才可使用 */
//#define MDS_DEBUGCONTEXT

#ifdef MDS_DEBUGCONTEXT
 #define MDS_CONTEXTADDR    0x2000B000  /* 记录的内存起始地址 */
 #define MDS_CONTEXTLEN     0x1000      /* 记录的长度, 单位: 字节 */
#endif

/* 检查栈空间功能的宏定义, 只有定义该宏才可使用 */
//#define MDS_DEBUGSTACKCHECK

/* CPU占有率功能的宏定义, 只有定义该宏才可使用 */
//#define MDS_CPUSHARE


#define TICK                10          /* 操作系统调度周期, 单位: ms */
#define CORECLOCKPMS        72000000    /* 芯片内核振荡频率, 单位: Hz */


#define ROOTTASKNAME        "Root"      /* 根任务名称 */
#define IDLETASKNAME        "Idle"      /* 空闲任务名称 */

#define ROOTTASKSTACK       600         /* 根任务栈大小, 单位: 字节 */
#define IDLETASKSTACK       600         /* 空闲任务栈大小, 单位: 字节 */

#define STACKALIGNMASK      ALIGN8MASK  /* 栈对齐掩码, cortex内核使用8字节对齐 */


#define PRIORITYNUM         PRIORITY8   /* 操作系统支持的优先级数目 */


#endif

