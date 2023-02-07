
#ifndef  MDS_CHIP_INNER_H
#define  MDS_CHIP_INNER_H


/* �жϷ���ʱ�ĵ�ַ */
#define RTN_HANDLER         0xFFFFFFF1  /* ����handlerģʽ */
#define RTN_THREAD_MSP      0xFFFFFFF9  /* ����threadģʽ, ��ʹ��MSP */
#define RTN_THREAD_PSP      0xFFFFFFFD  /* ����handlerģʽ, ��ʹ��PSP */


/* XPSR�Ĵ������жϵ�����λ */
#define XPSR_EXTMASK        0x1FF


/* ��ջָ��ѡ�� */
#define MSP_STACK               0       /* ����ջָ�� */
#define PSP_STACK               1       /* ���̶�ջָ�� */

/* ϵͳ����ģʽ */
#define PRIVILEGED              0       /* ��Ȩ��ģʽ */
#define UNPRIVILEGED            1       /* �û���ģʽ */


/*********************************** �������� *************************************/
extern U32 guiIntLockCounter;


/*********************************** �������� *************************************/
extern void MDS_TaskStackInit(M_TCB* pstrTcb, VFUNC vfFuncPointer, void* pvPara);
extern void MDS_SystemHardwareInit(void);
extern void MDS_TickTimerInit(void);
extern void MDS_PendSvIsrInit(void);
extern void MDS_SetChipWorkMode(U32 uiMode);
extern void MDS_TaskSwiSched(void);
extern void MDS_TaskOccurSwi(U32 uiSwiNo);


#endif

