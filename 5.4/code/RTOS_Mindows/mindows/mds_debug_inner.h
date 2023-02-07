
#ifndef  MDS_DEBUG_INNER_H
#define  MDS_DEBUG_INNER_H


/* 栈检查填充的数值 */
#ifdef MDS_DEBUGSTACKCHECK
 #define STACKCHECKVALUE    0xCCCCCCCC
#endif


#ifdef MDS_DEBUGCONTEXT

#define VALIDTASKNAMEMAXLEN         20              /* 任务名称的合法最大长度 */

/* 定义程序空间范围 */
#define ROMVALIDADDRSTART           0x08000000
#define ROMVALIDADDRLEN             0x40000
#define ROMVALIDADDREND             (ROMVALIDADDRSTART + ROMVALIDADDRLEN - 1)


/* 栈打印信息头结构 */
typedef struct m_conthead
{
    U32 uiAbsAddr;          /* 记录寄存器组和栈信息的内存起始地址, 绝对地址 */
    U32 uiRelAddr;          /* 记录寄存器组和栈信息的当前地址, 相对地址 */
    U32 uiLen;              /* 记录寄存器组和栈信息的长度 */
    U32 uiRemLen;           /* 记录寄存器组和栈信息的剩余长度 */
    VFUNC1 vfSendChar;      /* 发送字符的钩子变量 */
}M_CONTHEAD;

/* 保存寄存器组和栈信息的消息结构 */
typedef struct m_contmsg
{
    U32 uiLen;              /* 保存数据的长度 */
    U32 uiPreAddr;          /* 上个记录地址 */
    U32 uiPreTcb;           /* 前一个任务的TCB */
    U32 uiNextTcb;          /* 下一个任务的TCB */
    STACKREG strReg;        /* 保存的寄存器组 */
}M_CONTMSG;

#endif


/*********************************** 变量声明 *************************************/
#ifdef MDS_DEBUGCONTEXT
extern M_CONTHEAD* gpstrContext;
#endif


/*********************************** 函数声明 *************************************/
extern void MDS_SaveTaskContext(void);
extern void MDS_FaultIsrPrint(void);

#ifdef MDS_DEBUGCONTEXT
extern void MDS_TaskContextInit(void);
extern void MDS_PrintTaskContext(void);
extern void MDS_CoverFirstContext(void);
extern void MDS_SaveLastContext(U32 uiStackLen);
extern U32 MDS_SaveOneContextData(U32 uiAddr, U32 uiData);
extern U32 MDS_GetOneContextData(U32 uiAddr, U32* puiData);
extern void MDS_PrintContext(void);
extern void MDS_SendChar(U8 ucChar);
extern void MDS_SendString(U8* pvStringPt, ...);
extern void MDS_IfValidString(U8** pucString);
#endif

#ifdef MDS_DEBUGSTACKCHECK
void MDS_TaskStackCheckInit(M_TCB* pstrTcb);
#endif


#endif

