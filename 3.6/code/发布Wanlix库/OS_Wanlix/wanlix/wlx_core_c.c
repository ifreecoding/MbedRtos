
#include "wlx_core_c.h"


W_TCB* gpstrCurTcb;                     /* 当前运行的任务的TCB */

W_TCB* gpstrRootTaskTcb;                /* 根任务的TCB指针 */


/***********************************************************************************
函数功能: 初始化Wanlix操作系统. 该函数创建系统任务并从非任务状态切换到root任务运行.
入口参数: pucTaskStack: root任务栈起始地址, 该地址是栈中的最低地址.
          uiStackSize: root任务栈大小, 单位: 字节.
返 回 值: none.
***********************************************************************************/
void WLX_WanlixInit(U8* pucTaskStack, U32 uiStackSize)
{
    /**************************************************************************/
    /*            更多资料请访问    www.ifreecoding.com                       */
    /*                              bbs.ifreecoding.com                       */
    /*                              blog.sina.com.cn/ifreecoding              */
    /**************************************************************************/

    /* 创建根任务 */
    gpstrRootTaskTcb = WLX_TaskCreate(WLX_RootTask, NULL, pucTaskStack,
                                      uiStackSize);

    /* 开始任务调度, 从根任务开始执行 */
    WLX_TaskStart(gpstrRootTaskTcb);
}

/***********************************************************************************
函数功能: 创建一个任务.
入口参数: vfFuncPointer: 创建任务所使用函数的指针.
          pvPara: 任务入口参数指针.
          pucTaskStack: 任务所使用栈的最低起始地址.
          uiStackSize: 栈大小, 单位: 字节.
返 回 值: NULL: 创建任务失败.
          其它: 任务的TCB指针.
***********************************************************************************/
W_TCB* WLX_TaskCreate(VFUNC vfFuncPointer, void* pvPara, U8* pucTaskStack,
                      U32 uiStackSize)
{
    W_TCB* pstrTcb;

    /* 对创建任务所使用函数的指针合法性进行检查 */
    if(NULL == vfFuncPointer)
    {
        /* 指针为空, 返回失败 */
        return (W_TCB*)NULL;
    }

    /* 对任务栈合法性进行检查 */
    if((NULL == pucTaskStack) || (0 == uiStackSize))
    {
        /* 栈不合法, 返回失败 */
        return (W_TCB*)NULL;
    }

    /* 初始化TCB */
    pstrTcb = WLX_TaskTcbInit(vfFuncPointer, pvPara, pucTaskStack, uiStackSize);

    return pstrTcb;
}

/***********************************************************************************
函数功能: 初始化任务TCB, 用来创建任务运行前TCB的状态.
入口参数: vfFuncPointer: 任务运行的函数.
          pvPara: 任务入口参数指针.
          pucTaskStack: 任务所使用的栈地址.
          uiStackSize: 栈大小, 单位: 字节.
返 回 值: 任务TCB的指针.
***********************************************************************************/
W_TCB* WLX_TaskTcbInit(VFUNC vfFuncPointer, void* pvPara, U8* pucTaskStack,
                       U32 uiStackSize)
{
    W_TCB* pstrTcb;
    U8* pucStackBy4;

    /* 创建任务时的栈满地址处存放TCB, 需要4字节对齐 */
    pucStackBy4 = (U8*)(((U32)pucTaskStack + uiStackSize) & ALIGN4MASK);

    /* TCB地址即运行时使用的栈开始地址, cortex内核使用8字节对齐 */
    pstrTcb = (W_TCB*)(((U32)pucStackBy4 - sizeof(W_TCB)) & ALIGN8MASK);

    /* 初始化任务栈 */
    WLX_TaskStackInit(pstrTcb, vfFuncPointer, pvPara);

    return pstrTcb;
}

/***********************************************************************************
函数功能: 初始化任务栈函数, 用来创建任务运行前的栈状态.
入口参数: pstrTcb: 任务的TCB指针.
          vfFuncPointer: 任务运行的函数.
          pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void WLX_TaskStackInit(W_TCB* pstrTcb, VFUNC vfFuncPointer, void* pvPara)
{
    STACKREG* pstrStackReg;

    /* 寄存器组地址 */
    pstrStackReg = &pstrTcb->strStackReg;

    /* 对TCB中的寄存器组初始化 */
    pstrStackReg->uiR0 = (U32)pvPara;           /* R0 */
    pstrStackReg->uiR4 = 0;                     /* R4 */
    pstrStackReg->uiR5 = 0;                     /* R5 */
    pstrStackReg->uiR6 = 0;                     /* R6 */
    pstrStackReg->uiR7 = 0;                     /* R7 */
    pstrStackReg->uiR8 = 0;                     /* R8 */
    pstrStackReg->uiR9 = 0;                     /* R9 */
    pstrStackReg->uiR10 = 0;                    /* R10 */
    pstrStackReg->uiR11 = 0;                    /* R11 */
    pstrStackReg->uiR12 = 0;                    /* R12 */
    pstrStackReg->uiR13 = (U32)pstrTcb;         /* R13 */
    pstrStackReg->uiR14 = (U32)vfFuncPointer;   /* R14 */
    pstrStackReg->uiXpsr = MODE_USR;            /* XPSR */
}

/***********************************************************************************
函数功能: 调用该函数将发生任务切换.
入口参数: pstrTcb: 即将运行的任务的TCB指针.
返 回 值: none.
***********************************************************************************/
void WLX_TaskSwitch(W_TCB* pstrTcb)
{
    STACKREG* pstrCurTaskStackRegAddr;
    STACKREG* pstrNextTaskStackRegAddr;

    /* 当前任务寄存器组的地址 */
    pstrCurTaskStackRegAddr = &gpstrCurTcb->strStackReg;

    /* 即将运行任务寄存器组的地址 */
    pstrNextTaskStackRegAddr = &pstrTcb->strStackReg;

    /* 保存即将运行任务的TCB */
    gpstrCurTcb = pstrTcb;

    /* 切换任务 */
    WLX_ContextSwitch(pstrCurTaskStackRegAddr, pstrNextTaskStackRegAddr);
}

/***********************************************************************************
函数功能: 开始进入任务调度.
入口参数: pstrTcb: 即将运行的任务的TCB指针.
返 回 值: none.
***********************************************************************************/
void WLX_TaskStart(W_TCB* pstrTcb)
{
    STACKREG* pstrNextTaskStackRegAddr;

    /* 即将运行任务寄存器组的地址 */
    pstrNextTaskStackRegAddr = &pstrTcb->strStackReg;

    /* 保存即将运行任务的TCB */
    gpstrCurTcb = pstrTcb;

    /* 切换到任务状态 */
    WLX_SwitchToTask(pstrNextTaskStackRegAddr);
}

/***********************************************************************************
函数功能: 获取Wanlix的版本号.
入口参数: none.
返 回 值: 存储版本号的字符串地址.
***********************************************************************************/
U8* WLX_GetWalixVersion(void)
{
    return WANLIX_VER;
}

