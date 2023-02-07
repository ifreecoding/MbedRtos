
#ifndef  MDS_DEBUG_INNER_H
#define  MDS_DEBUG_INNER_H


/* ջ���������ֵ */
#ifdef MDS_DEBUGSTACKCHECK
 #define STACKCHECKVALUE    0xCCCCCCCC
#endif


#ifdef MDS_DEBUGCONTEXT

#define VALIDTASKNAMEMAXLEN         20              /* �������ƵĺϷ���󳤶� */

/* �������ռ䷶Χ */
#define ROMVALIDADDRSTART           0x08000000
#define ROMVALIDADDRLEN             0x40000
#define ROMVALIDADDREND             (ROMVALIDADDRSTART + ROMVALIDADDRLEN - 1)


/* ջ��ӡ��Ϣͷ�ṹ */
typedef struct m_conthead
{
    U32 uiAbsAddr;          /* ��¼�Ĵ������ջ��Ϣ���ڴ���ʼ��ַ, ���Ե�ַ */
    U32 uiRelAddr;          /* ��¼�Ĵ������ջ��Ϣ�ĵ�ǰ��ַ, ��Ե�ַ */
    U32 uiLen;              /* ��¼�Ĵ������ջ��Ϣ�ĳ��� */
    U32 uiRemLen;           /* ��¼�Ĵ������ջ��Ϣ��ʣ�೤�� */
    VFUNC1 vfSendChar;      /* �����ַ��Ĺ��ӱ��� */
}M_CONTHEAD;

/* ����Ĵ������ջ��Ϣ����Ϣ�ṹ */
typedef struct m_contmsg
{
    U32 uiLen;              /* �������ݵĳ��� */
    U32 uiPreAddr;          /* �ϸ���¼��ַ */
    U32 uiPreTcb;           /* ǰһ�������TCB */
    U32 uiNextTcb;          /* ��һ�������TCB */
    STACKREG strReg;        /* ����ļĴ����� */
}M_CONTMSG;

#endif


/*********************************** �������� *************************************/
#ifdef MDS_DEBUGCONTEXT
extern M_CONTHEAD* gpstrContext;
#endif


/*********************************** �������� *************************************/
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

