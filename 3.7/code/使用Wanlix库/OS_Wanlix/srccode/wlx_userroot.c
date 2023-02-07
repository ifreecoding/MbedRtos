
#include "wlx_userroot.h"


U8 gaucRootTaskStack[ROOTTASKSTACK];    /* 根任务的栈 */


/***********************************************************************************
函数功能: 根任务, 用户代码从这个函数开始执行.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void WLX_RootTask(void* pvPara)
{
    /* 初始化软件 */
    DEV_SoftwareInit();

    /* 初始化硬件 */
    DEV_HardwareInit();

    /* 创建任务 */
    gpstrCheckPsgKeyTcb = WLX_TaskCreate((VFUNC)TEST_CheckPsgKeyTask, NULL,
                                        gaucCheckPsgKeyStack, TASKSTACK);
    gpstrCrossStateTcb = WLX_TaskCreate((VFUNC)TEST_CrossStateTask, NULL,
                                        gaucCrossStateStack, TASKSTACK);
    gpstrLedDisplayTcb = WLX_TaskCreate((VFUNC)TEST_LedDisplayTask, NULL,
                                        gaucLedDisplayStack, TASKSTACK);

    /* 切换到检测行人按键的任务 */
    WLX_TaskSwitch(gpstrCheckPsgKeyTcb);
}

