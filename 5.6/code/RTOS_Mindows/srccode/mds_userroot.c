
#include "mds_userroot.h"


/***********************************************************************************
函数功能: 根任务, 用户代码从这个函数开始执行, 优先级最高.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_RootTask(void)
{
    ;
}

/***********************************************************************************
函数功能: 空闲任务, CPU空闲时执行这个任务, 优先级最低.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void MDS_IdleTask(void* pvPara)
{
    while(1)
    {
        ;
    }
}

