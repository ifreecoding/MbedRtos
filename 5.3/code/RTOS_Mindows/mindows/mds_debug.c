
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "mindows_inner.h"


#ifdef MDS_DEBUGCONTEXT

M_CONTHEAD* gpstrContext;               /* 栈切换信息头存放的地址 */

#endif


/***********************************************************************************
函数功能: 记录任务切换内容, 将切换前任务的寄存器组和栈信息保存到指定内存.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_SaveTaskContext(void)
{
#ifdef MDS_DEBUGCONTEXT

    U32 uiStack;
    U32 uiTcbAddr;
    U32 uiSaveLen;

    /* uiAbsAddr为0代表此功能不可用 */
    if(0 == gpstrContext->uiAbsAddr)
    {
        return;
    }

    /* 同一任务之间切换则不记录任务切换信息 */
    if(gpstrCurTaskReg == gpstrNextTaskReg)
    {
        return;
    }

    /* 获取切换前任务的SP寄存器数值 */
    uiStack = gpstrCurTaskReg->uiR13;

    /* 获取切换前任务的TCB存放的地址 */
    uiTcbAddr = (U32)gpstrCurTaskReg;

    /* 计算需要保存的数据长度, 包括栈结构体以及寄存器组和切换前任务的局部变量 */
    uiSaveLen = sizeof(M_CONTMSG) + (uiTcbAddr - uiStack);

    /* 要保存的数据长度大于总长度, 将uiAbsAddr置为0, 代表此功能不可用 */
    if(uiSaveLen > gpstrContext->uiLen)
    {
        gpstrContext->uiAbsAddr = 0;

        return;
    }

    while(1)
    {
        /* 要保存的数据长度大于剩余长度, 则覆盖最开始记录的数据 */
        if(uiSaveLen > gpstrContext->uiRemLen)
        {
            MDS_CoverFirstContext();
        }
        else /* 剩余长度能够容纳下要保存的数据 */
        {
            break;
        }
    }

    /* 保存数据 */
    MDS_SaveLastContext(uiTcbAddr - uiStack);

#endif
}

/***********************************************************************************
函数功能: 芯片触发了硬件故障中断服务程序, 在该函数中打印保存的栈数据.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_FaultIsrPrint(void)
{
#ifdef MDS_DEBUGCONTEXT

    /* 将gpstrCurTaskReg指向当前的寄存器组 */
    gpstrCurTaskReg = &gpstrCurTcb->strStackReg;

    /* 将gpstrNextTaskReg指向空, 以表明发生异常中断 */
    gpstrNextTaskReg = (STACKREG*)NULL;

    /* 将当前寄存器组和栈信息保存到指定内存中 */
    MDS_SaveTaskContext();

    /* 打印内存中记录的任务寄存器组和栈信息 */
    MDS_PrintTaskContext();

#endif
}

#ifdef MDS_DEBUGCONTEXT

/***********************************************************************************
函数功能: 初始化记录任务寄存器组和栈信息的功能.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_TaskContextInit(void)
{
    M_CONTMSG* pstrCurAddr;

    /* 确定存放栈信息的存放地址 */
    gpstrContext = (M_CONTHEAD*)((MDS_CONTEXTADDR + 3) & ALIGN4MASK);

    /* 记录的起始地址对齐到4字节, 为绝对地址 */
    gpstrContext->uiAbsAddr = (U32)gpstrContext + sizeof(M_CONTHEAD);

    /* uiAbsAddr为0代表此功能不可用 */
    if(0 == gpstrContext->uiAbsAddr)
    {
        return;
    }

    /* 记录的当前地址, 为相对地址 */
    gpstrContext->uiRelAddr = 0;

    /* 记录的起始和结束地址都需要对齐到4字节, 计算对齐后的长度 */
    gpstrContext->uiLen = ((MDS_CONTEXTADDR + MDS_CONTEXTLEN) & ALIGN4MASK)
                          - gpstrContext->uiAbsAddr;

    /* 如果长度过小, 此功能不可用 */
    if(gpstrContext->uiLen < sizeof(M_CONTMSG))
    {
        /* 将uiAbsAddr置为0, 代表此功能不可用 */
        gpstrContext->uiAbsAddr = 0;

        return;
    }

    /* 如果长度超出了32位机上限, 此功能不可用 */
    if(gpstrContext->uiAbsAddr > 0xFFFFFFFF - gpstrContext->uiLen)
    {
        /* 将uiAbsAddr置为0, 代表此功能不可用 */
        gpstrContext->uiAbsAddr = 0;

        return;
    }

    /* 剩余的可记录信息的长度 */
    gpstrContext->uiRemLen = gpstrContext->uiLen;

    /* 初始化当前记录的数据 */
    pstrCurAddr = (M_CONTMSG*)(gpstrContext->uiAbsAddr + gpstrContext->uiRelAddr);
    pstrCurAddr->uiLen = 0;
}

/***********************************************************************************
函数功能: 打印任务切换内容, 将内存中记录的任务寄存器组和栈信息打印到串口.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_PrintTaskContext(void)
{
    /* uiAbsAddr为0代表此功能不可用 */
    if(0 == gpstrContext->uiAbsAddr)
    {
        return;
    }

    MDS_SendString("\r\n\r\n!!!!!EXCEPTION OCCUR!!!!!\n\r");

    /* 打印任务切换信息 */
    while(gpstrContext->uiLen > gpstrContext->uiRemLen)
    {
        MDS_PrintContext();
    }

    MDS_SendString("\r\n\r\n!!!!!EXCEPTION PRINT OVER!!!!!\n\r");
}

/***********************************************************************************
函数功能: 覆盖记录的第一条任务切换消息, 将其所占用的内存加入剩余空间.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void MDS_CoverFirstContext(void)
{
    M_CONTMSG* pstrLstMsg;
    M_CONTMSG* pstrFstMsg;
    U32 uiIdleAddr;
    U32 uiFstAddr;

    /* 最后一条保存的消息地址 */
    pstrLstMsg = (M_CONTMSG*)(gpstrContext->uiAbsAddr + gpstrContext->uiRelAddr);

    /* 最后一条消息后的空闲地址 */
    uiIdleAddr = gpstrContext->uiRelAddr + pstrLstMsg->uiLen;

    /* 若空闲地址大于等于保存区总长度说明空闲内存中将保存的消息不会跨越高低端内存 */
    if(uiIdleAddr >= gpstrContext->uiLen)
    {
        /* 将空闲的内存地址更新到保存区低端 */
        uiIdleAddr -= gpstrContext->uiLen;

        /* 获取已保存的第一条消息地址 */
        uiFstAddr = uiIdleAddr + gpstrContext->uiRemLen;
        pstrFstMsg = (M_CONTMSG*)(gpstrContext->uiAbsAddr + uiFstAddr);
    }
    else /* 空闲内存中将保存的消息跨越了高低端内存 */
    {
        /* 获取已保存的第一条消息地址 */
        uiFstAddr = uiIdleAddr + gpstrContext->uiRemLen;

        /* 若第一条消息的地址大于等于保存区总长度, 说明保存第一条消息的内存没有跨越
           高低端内存 */
        if(uiFstAddr >= gpstrContext->uiLen)
        {
            /* 将第一条消息的内存地址更新到保存区低端 */
            uiFstAddr -= gpstrContext->uiLen;
        }

        pstrFstMsg = (M_CONTMSG*)(gpstrContext->uiAbsAddr + uiFstAddr);
    }

    /* 覆盖掉第一条消息, 将第一条消息的空间加入空闲内存中 */
    gpstrContext->uiRemLen += pstrFstMsg->uiLen;
}

/***********************************************************************************
函数功能: 保存最后一条任务切换消息.
入口参数: uiStackLen: 需要保存的栈的长度.
返 回 值: none.
***********************************************************************************/
void MDS_SaveLastContext(U32 uiStackLen)
{
    M_CONTMSG* pstrLstMsg;
    U32* puiDataAddr;
    U32 uiLstAddr;
    U32 uiDataAddr;
    U32 uiTcbAddr;
    U32 i;

    /* 最后一条保存的消息 */
    pstrLstMsg = (M_CONTMSG*)(gpstrContext->uiAbsAddr + gpstrContext->uiRelAddr);

    /* 即将保存新消息, 更新最后一条保存消息的地址 */
    uiLstAddr = gpstrContext->uiRelAddr + pstrLstMsg->uiLen;

    /* 若新消息地址大于等于保存区总长度, 需要将新消息地址更新到低端内存 */
    if(uiLstAddr >= gpstrContext->uiLen)
    {
        uiLstAddr -= gpstrContext->uiLen;
    }

    /* 保存消息头结构 */
    /* 保存长度 */
    uiDataAddr = MDS_SaveOneContextData(uiLstAddr, sizeof(M_CONTMSG) + uiStackLen);

    /* 保存上一个记录的地址 */
    uiDataAddr = MDS_SaveOneContextData(uiDataAddr, gpstrContext->uiRelAddr);

    /* 保存上一个任务的TCB */
    uiTcbAddr = (U32)gpstrCurTaskReg;
    uiDataAddr = MDS_SaveOneContextData(uiDataAddr, uiTcbAddr);

    /* 保存下一个任务的TCB */
    if(NULL != gpstrNextTaskReg)
    {
        uiTcbAddr = (U32)gpstrNextTaskReg;
    }
    else /* gpstrNextTaskReg为NULL说明产生异常中断, 下个TCB记录为空 */
    {
        uiTcbAddr = 0;
    }

    uiDataAddr = MDS_SaveOneContextData(uiDataAddr, uiTcbAddr);

    /* 保存寄存器组数据 */
    puiDataAddr = &gpstrCurTaskReg->uiExc_Rtn;
    for(i = 0; i < (sizeof(STACKREG) / sizeof(U32)); i++)
    {
        uiDataAddr = MDS_SaveOneContextData(uiDataAddr, *puiDataAddr--);
    }

    /* 保存栈数据 */
    for(i = 0; i < (uiStackLen / sizeof(U32)); i++)
    {
        uiDataAddr = MDS_SaveOneContextData(uiDataAddr, *puiDataAddr--);
    }

    /* 更新全局变量 */
    gpstrContext->uiRelAddr = uiLstAddr;
    gpstrContext->uiRemLen -= (sizeof(M_CONTMSG) + uiStackLen);
}

/***********************************************************************************
函数功能: 保存一个数据.
入口参数: uiAddr: 保存数据的地址.
          uiData: 保存的数据.
返 回 值: 下个保存数据的地址.
***********************************************************************************/
U32 MDS_SaveOneContextData(U32 uiAddr, U32 uiData)
{
    ADDRVAL(gpstrContext->uiAbsAddr + uiAddr) = uiData;

    if(gpstrContext->uiLen != uiAddr + 4)
    {
        uiAddr += 4;
    }
    else
    {
        uiAddr = 0;
    }

    return uiAddr;
}

/***********************************************************************************
函数功能: 获取一个数据.
入口参数: uiAddr: 保存数据的地址.
          uiData: 保存的数据.
返 回 值: 下个获取数据的地址.
***********************************************************************************/
U32 MDS_GetOneContextData(U32 uiAddr, U32* puiData)
{
    *puiData = ADDRVAL(gpstrContext->uiAbsAddr + uiAddr);

    if(gpstrContext->uiLen != uiAddr + 4)
    {
        uiAddr += 4;
    }
    else
    {
        uiAddr = 0;
    }

    return uiAddr;
}

/***********************************************************************************
函数功能: 打印任务切换消息.
入口参数: uiAddr: 需要打印的起始地址.
返 回 值: none.
***********************************************************************************/
void MDS_PrintContext(void)
{
    U8* pucTaskName;
    U32 uiData;
    U32 uiLen;
    U32 uiDataAddr;
    U32 uiTcb;
    U32 i;

    uiDataAddr = gpstrContext->uiRelAddr;

    /* 获取消息头结构 */
    /* 获取长度 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiLen);

    /* 获取上个记录的地址 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    gpstrContext->uiRelAddr = uiData;

    /* 获取前一个任务的TCB */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiTcb);
    pucTaskName = ((M_TCB*)uiTcb)->pucTaskName;
    MDS_IfValidString(&pucTaskName);
    MDS_SendString("\r\n\r\nTask %s 0x%08X ", pucTaskName, uiTcb);

    /* 获取下一个任务的TCB */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    if(0 != uiData)
    {
        pucTaskName = ((M_TCB*)uiData)->pucTaskName;
        MDS_IfValidString(&pucTaskName);
        MDS_SendString("switch to task %s 0x%08X ", pucTaskName, uiData);
    }
    else /* 下个任务的TCB为空, 说明已经产生异常中断 */
    {
        MDS_SendString("occur exception!");
    }

    /* 获取寄存器组数据 */
    /* 获取Exc_Rtn寄存器 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString("\r\nExc_Rtn = 0x%08X", uiData);

    /* 获取XPSR寄存器 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString("\r\nXPSR= 0x%08X", uiData);

    /* 获取R15寄存器 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString("\r\nR15 = 0x%08X", uiData);

    /* 获取R14寄存器 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R14 = 0x%08X", uiData);

    /* 获取R13寄存器 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R13 = 0x%08X", uiData);

    /* 获取R12寄存器 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R12 = 0x%08X", uiData);

    /* 获取R11寄存器 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString("\r\nR11 = 0x%08X", uiData);

    /* 获取R10寄存器 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R10 = 0x%08X", uiData);

    /* 获取R9寄存器 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R9  = 0x%08X", uiData);

    /* 获取R8寄存器 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R8  = 0x%08X", uiData);

    /* 获取R7寄存器 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString("\r\nR7  = 0x%08X", uiData);

    /* 获取R6寄存器 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R6  = 0x%08X", uiData);

    /* 获取R5寄存器 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R5  = 0x%08X", uiData);

    /* 获取R4寄存器 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R4  = 0x%08X", uiData);

    /* 获取R3寄存器 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString("\r\nR3  = 0x%08X", uiData);

    /* 获取R2寄存器 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R2  = 0x%08X", uiData);

    /* 获取R1寄存器 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R1  = 0x%08X", uiData);

    /* 获取R0寄存器 */
    uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);
    MDS_SendString(" R0  = 0x%08X", uiData);

    /* 获取栈数据 */
    for(i = 0; i < (uiLen - sizeof(M_CONTMSG)) / sizeof(U32); i++)
    {
        uiTcb -= 4;

        uiDataAddr = MDS_GetOneContextData(uiDataAddr, &uiData);

        MDS_SendString("\r\n0x%08X: 0x%08X", uiTcb, uiData);
    }

    /* 更新全局变量 */
    gpstrContext->uiRemLen += uiLen;
}

/***********************************************************************************
函数功能: 挂接发送字符的函数.
入口参数: vfFuncPointer: 需要挂接的发送字符函数, 该函数的原型为VFUNC1.
返 回 值: none.
***********************************************************************************/
void MDS_SendCharFuncInit(VFUNC1 vfFuncPointer)
{
    gpstrContext->vfSendChar = vfFuncPointer;
}

/***********************************************************************************
函数功能: 挂接后调用的发送字符函数.
入口参数: ucChar: 需要发送的字符.
返 回 值: none.
***********************************************************************************/
void MDS_SendChar(U8 ucChar)
{
    if(NULL == gpstrContext->vfSendChar)
    {
        return;
    }

    gpstrContext->vfSendChar(ucChar);
}

/***********************************************************************************
函数功能: 打印字符串函数.
入口参数: pvStringPt: 第一个参数的指针, 必须为字符串指针.
          ...: 其它参数.
返 回 值: none.
***********************************************************************************/
void MDS_SendString(U8* pvStringPt, ...)
{
#define BUFLEN      200     /* 消息缓冲长度 */
    U8 aucBuf[BUFLEN];
    U32 i;
    va_list args;

    /* 将字符串格式化到内存 */
    va_start(args, pvStringPt);
    (void)vsprintf(aucBuf, pvStringPt, args);
    va_end(args);

    /* 将内存中的消息打印到串口 */
    for(i = 0; NULL != aucBuf[i]; i++)
    {
        MDS_SendChar(aucBuf[i]);
    }
}

/***********************************************************************************
函数功能: 判断字符串是否有效, 合法的字符串需要是不为空, 是可打印的字符, 长度为有限长
          度. 对于无效的字符串则将存储字符串地址的指针修改指向"BAD DATA!".
入口参数: pucString: 存储字符串地址的指针.
返 回 值: none.
***********************************************************************************/
void MDS_IfValidString(U8** pucString)
{
    U8* pucStr;
    U32 i;

    /* 入口参数为空, 返回失败 */
    if(NULL == pucString)
    {
        goto BAD_RTN;
    }

    /* 取出字符串地址 */
    pucStr = *pucString;

    /* 字符串地址为非法地址, 返回失败 */
    if(((U32)pucStr < ROMVALIDADDRSTART) || ((U32)pucStr > ROMVALIDADDREND)
       || (NULL == pucStr))
    {
        goto BAD_RTN;
    }

    /* 在规定的长度内查询字符串是否为可打印字符 */
    for(i = 0; i < VALIDTASKNAMEMAXLEN; i++)
    {
        /* 遇到字符串结束符, 返回成功 */
        if(NULL == pucStr[i])
        {
            return;
        }

        /* 字符串包含不可打印字符, 返回失败 */
        if((pucStr[i] < ' ') || (pucStr[i] > '~'))
        {
            goto BAD_RTN;
        }
    }

    /* 字符串长度超长, 返回失败 */

BAD_RTN:

    *pucString = "BAD DATA!";

    return;
}

#endif

