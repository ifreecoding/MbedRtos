
#ifndef  MDS_USERDEF_H
#define  MDS_USERDEF_H


/******************************* 用户可修改包含文件 *******************************/
#include "stm32f10x.h"


/******************************** 用户可修改宏定义 ********************************/
#define TICK                10          /* 操作系统调度周期, 单位: ms */
#define CORECLOCKPMS        72000000    /* 芯片内核振荡频率, 单位: Hz */


#define ROOTTASKSTACK       600         /* 根任务栈大小, 单位: 字节 */

#define STACKALIGNMASK      ALIGN8MASK  /* 栈对齐掩码, cortex内核使用8字节对齐 */


#define PRIORITYNUM         PRIORITY8   /* 操作系统支持的优先级数目 */


#endif

