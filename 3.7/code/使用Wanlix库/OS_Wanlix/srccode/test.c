
#include "test.h"


U8 gaucCheckPsgKeyStack[TASKSTACK]; /* 十字路口检测行人按键任务的堆栈 */
U8 gaucCrossStateStack[TASKSTACK];  /* 十字路口状态任务的堆栈 */
U8 gaucLedDisplayStack[TASKSTACK];  /* 十字路口灯显示任务的堆栈 */

W_TCB* gpstrCheckPsgKeyTcb;         /* 十字路口检测行人按键任务的TCB指针 */
W_TCB* gpstrCrossStateTcb;          /* 十字路口状态任务的TCB指针 */
W_TCB* gpstrLedDisplayTcb;          /* 十字路口灯显示任务的TCB指针 */


/* 十字路口各种状态 */
CROSSSTATESTR gastrCrossSta[CROSSSTATENUM] = CROSSINITVALUE;

/* 十字路口当前状态 */
CROSSSTATESTR gstrCurCrossSta;


U32 guiCurSta;          /* 十字路口当前所处的状态 */
U32 guiStaChange;       /*  0: 十字路口状态未改变, 其它: 状态改变 */


U32 guiSystemTick;      /* 系统时钟 */


/***********************************************************************************
函数功能: 主函数, C程序从此函数开始运行. 该函数启动操作系统并从非任务状态切换到root
          任务WLX_RootTask运行.
入口参数: none.
返 回 值: 返回值为0, 但系统不会运行到该函数返回.
***********************************************************************************/
S32 main(void)
{
    /**************************************************************************/
    /*            更多资料请访问    www.ifreecoding.com                       */
    /*                              bbs.ifreecoding.com                       */
    /*                              blog.sina.com.cn/ifreecoding              */
    /**************************************************************************/

    /* 初始化操作系统 */
    WLX_WanlixInit(gaucRootTaskStack, ROOTTASKSTACK);
}

/***********************************************************************************
函数功能: 十字路口变量初始化函数.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void TEST_CrossVarInit(void)
{
    guiSystemTick = 0;
    guiCurSta = CROSSSTATE1;
    guiStaChange = STATECHANGE;
}

/***********************************************************************************
函数功能: 检测十字路口行人按键任务.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void TEST_CheckPsgKeyTask(void* pvPara)
{
    while(1)
    {
        /* 有行人按键被按下 */
        if(KEYNULL != DEV_ReadKey())
        {
            /* 将十字路口改变为行人过马路状态 */
            if(CROSSSTATE1 == guiCurSta)
            {
                /* 状态1下触发行人中断, 置为状态2 */
                guiCurSta = CROSSSTATE2;

                /* 置为状态改变 */
                guiStaChange = STATECHANGE;
            }
            else if((CROSSSTATE3 == guiCurSta) || (CROSSSTATE4 == guiCurSta))
            {
                /* 状态3和状态4下触发行人中断, 置为状态3 */
                guiCurSta = CROSSSTATE3;

                /* 置为状态改变 */
                guiStaChange = STATECHANGE;
            }
        }

        /* 切换到控制灯状态的任务 */
        WLX_TaskSwitch(gpstrCrossStateTcb);
    }
}

/***********************************************************************************
函数功能: 改变十字路口灯状态任务.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void TEST_CrossStateTask(void* pvPara)
{
    U32 uiStaEndTick;
    U32 uiFlashEndTick;
    U32 i;

    uiStaEndTick = 0xFFFFFFFF;
    uiFlashEndTick = 0;

    while(1)
    {
        /* 状态改变 */
        if(STATEUNCHANGE != guiStaChange)
        {
            guiStaChange = STATEUNCHANGE;

            /* 获取新状态的参数 */
            gstrCurCrossSta = gastrCrossSta[guiCurSta];

            /* 计算该状态结束时的Tick. Tick溢出后会从0重新开始 */
            uiStaEndTick = guiSystemTick + gstrCurCrossSta.uiRunTime;

            /* 计算闪烁状态灯亮度切换的Tick. Tick溢出后会从0重新开始 */
            uiFlashEndTick = guiSystemTick + FLASHTIME;
        }

        /* 当前状态时间到, 需要切换到下一个状态 */
        if(uiStaEndTick == guiSystemTick)
        {
            /* 置为状态改变 */
            guiStaChange = STATECHANGE;

            /* 寻找下一个状态 */
            guiCurSta = ++guiCurSta % CROSSSTATENUM;
        }
        else
        {
            /* 闪烁状态灯亮度切换时间到, 需要切换灯亮度 */
            if(uiFlashEndTick == guiSystemTick)
            {
                /* 循环判断各个灯的闪烁状态 */
                for(i = 0; i < LEDNUM; i++)
                {
                    /* 该灯不是闪烁状态则进行下个灯的判断 */
                    if(LEDSTATE_FLASH != gstrCurCrossSta.astrLed[i].uiLedState)
                    {
                        continue;
                    }
                    else /* 闪烁状态 */
                    {
                        /* 改变灯的亮度, 亮灭互转 */
                        if(LED_DARK == gstrCurCrossSta.astrLed[i].uiBrightness)
                        {
                            gstrCurCrossSta.astrLed[i].uiBrightness = LED_LIGHT;
                        }
                        else
                        {
                            gstrCurCrossSta.astrLed[i].uiBrightness = LED_DARK;
                        }

                        /* 更新下个闪烁时间 */
                        uiFlashEndTick = guiSystemTick + FLASHTIME;
                    }
                }
            }
        }

        /* 切换到控制灯亮度的任务 */
        WLX_TaskSwitch(gpstrLedDisplayTcb);
    }
}

/***********************************************************************************
函数功能: 改变十字路口灯显示任务.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void TEST_LedDisplayTask(void* pvPara)
{
    LAMPNO i;

    while(1)
    {
        /* 根据各个灯的状态控制灯的亮度 */
        for(i = MASTER_RED; i < LEDNUM; i++)
        {
            /* 设置灯的亮度 */
            DEV_LampSet(i, gstrCurCrossSta.astrLed[i].uiBrightness);
        }

        /* 切换到检测行人按键的任务 */
        WLX_TaskSwitch(gpstrCheckPsgKeyTcb);
    }
}

